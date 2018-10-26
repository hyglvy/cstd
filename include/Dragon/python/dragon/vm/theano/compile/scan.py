# --------------------------------------------------------
# Theano @ Dragon
# Copyright(c) 2017 SeetaTech
# Written by Ting Pan
# --------------------------------------------------------

import inspect
import copy
from dragon.core.tensor import Tensor
import dragon.protos.dragon_pb2 as pb

def scan(fn, sequences, outputs_info, n_steps=None, axis=0):
    """Run a dynamic loop of the given one step function.

    Parameters
    ----------
    fn : lambda
        The function to execute at each step.
    sequences : Tensor or list or Tensor
        The sequences.
    outputs_info : Tensor or list of Tensor
        The outputs.
    n_steps : int or Tensor
        The steps of loop.
    axis : int
        The axis of sequences.

    Returns
    -------
    Tensor or list of Tensor
        The outputs.

    Examples
    --------
    >>> x = Tensor('x', dtype='float32')
    >>> x.set_value([1, 2, 3, 4, 5])
    >>> zero = Tensor('zero', dtype='float32').Variable()
    >>> zero.set_value(0)
    >>> prefix_sum = scan(fn=lambda x_i, y_i : x_i + y_i, sequences=x,
                          outputs_info=zero, n_steps=x.shape[0], axis=0)
    >>> f = theano.function(outputs=prefix_sum)
    >>> print(f())
    >>> [  1.   3.   6.  10.  15.]

    """
    if not isinstance(sequences, list): sequences = [sequences]
    if not isinstance(outputs_info, list): outputs_info = [outputs_info]

    # 1. exact default outputs
    fn_nargs = len(inspect.getargspec(fn)[0])
    default_outputs = []
    for output in outputs_info:
        if output is not None: default_outputs.append(output)
    if len(sequences) + len(default_outputs) < fn_nargs:
        raise RuntimeError('Expect {} args of fn, but at most {} args can be used\n'
                           'sequences provide {}, outputs_info provide {}.'. \
                           format(fn_nargs, len(sequences) + len(default_outputs),
                                  len(sequences), len(default_outputs)))

    # 2. simulate specific function
    fn_inputs = [x for x in sequences] + default_outputs
    fn_inputs = copy.deepcopy(fn_inputs)
    # clear to avoid importing external expressions into template function
    for input in fn_inputs: input.expressions = {}
    outputs = fn(*fn_inputs)
    if not isinstance(outputs, tuple): outputs = [outputs]
    else: outputs = list(outputs)
    if len(outputs) != len(outputs_info):
        raise RuntimeError('Expect {} outputs of fn, but len of outputs_info is {}.'
                            .format(len(outputs), len(outputs_info)))

    # 3. make GraphDef
    graph_def = pb.GraphDef(); all_exprs = {}
    for output in outputs:
        graph_def.target.extend([output._name])
        all_exprs = dict(all_exprs, **output.expressions)
    all_exprs = sorted(all_exprs.items(), key=lambda d:d[0])
    forward_ops = copy.deepcopy([v for k,v in all_exprs])
    graph_def.op.extend(forward_ops)

    # 4. exact external inputs
    external_inputs = []; internal_outputs = []
    internal_inputs = [tensor.name for tensor in fn_inputs]
    for op in graph_def.op:
        for input in op.input:
            if input not in internal_inputs:
                if input not in internal_outputs:
                    external_inputs.append(input)
        for output in op.output:
            internal_outputs.append(output)

    # 5. collect inputs (sequences + default + external)
    default_outputs = [elem.name if elem is not None else '' for elem in outputs_info]
    inputs = fn_inputs + [Tensor(name) for name in external_inputs]

    kwargs = {'axis': axis, 'nseqs': len(sequences),
              'default_outputs': default_outputs, 'func_str': str(graph_def)}

    if isinstance(n_steps, int):
        kwargs['nsteps'] = n_steps
        kwargs['step_type'] = 'Static'
    elif isinstance(n_steps, Tensor):
        kwargs['extra_inputs'] = [n_steps]
        kwargs['step_tensor'] = n_steps.name
        kwargs['step_type'] = 'Dynamic'
    else: kwargs['step_type'] = 'Default'
    kwargs['inputs_name'] = [t.name for t in inputs]
    kwargs['outputs_name'] = [t.name for t in outputs]

    return Tensor.CreateOperator(inputs, existing_outputs=outputs, op_type='Scan', **kwargs)