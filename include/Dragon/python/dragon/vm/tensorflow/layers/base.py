# --------------------------------------------------------
# TensorFlow @ Dragon
# Copyright(c) 2017 SeetaTech
# Written by Ting Pan
# --------------------------------------------------------

import re
import collections
import weakref

from dragon.core.scope import get_tensor_scope

from dragon.vm.tensorflow.framework import ops
from dragon.vm.tensorflow.framework import dtypes
from dragon.vm.tensorflow.ops import var_scope as vs
from dragon.vm.tensorflow.ops import variables as tf_variables
from dragon.vm.tensorflow.util import nest


class Layer(object):
    def __init__(self, trainable=True, name=None, dtype=dtypes.float32, **kwargs):
        allowed_kwargs = {'_scope',
                          '_reuse'}
        for kwarg in kwargs:
            if kwarg not in allowed_kwargs:
                raise TypeError('Keyword argument not understood:', kwarg)

        self.trainable = trainable
        self.built = False
        self._trainable_weights = []
        self._non_trainable_weights = []
        self._updates = []
        self._losses = []
        self._reuse = kwargs.get('_reuse')
        self._graph = ops.get_default_graph()
        self._per_input_losses = {}
        self._per_input_updates = {}
        self.dtype = dtypes.as_dtype(dtype)
        self.input_spec = None

        # Determine layer name
        if name is None:
            base_name = _to_snake_case(self.__class__.__name__)
            self.name = _unique_layer_name(base_name)
        else:
            base_name = name
            self.name = name

        self._base_name = base_name

        # Determine variable scope.
        self._scope = None

    def build(self, _):
        self.built = True

    def call(self, inputs, *args, **kwargs):
        raise NotImplementedError

    @property
    def updates(self):
        return self._updates

    def __call__(self, inputs, *args, **kwargs):
        with vs.variable_scope(self._scope,
                               reuse=self.built or self._reuse) as scope:
            with ops.name_scope(scope.original_name_scope):
                if not self.built:
                    input_shapes = [x.get_shape()
                                    for x in nest.flatten(inputs)]
                    if len(input_shapes) == 1:
                        self.build(input_shapes[0])
                    else:
                        self.build(input_shapes)
                outputs = self.call(inputs, *args, **kwargs)
                # # Apply activity regularization.
                # # Note that it should be applied every time the layer creates a new
                # # output, since it is output-specific.
                # if hasattr(self, 'activity_regularizer') and self.activity_regularizer:
                #     output_list = _to_list(outputs)
                #     for output in output_list:
                #         with ops.name_scope('ActivityRegularizer'):
                #             activity_regularization = self.activity_regularizer(output)
                #         self.add_loss(activity_regularization)
                #         _add_elements_to_collection(
                #             activity_regularization, ops.GraphKeys.REGULARIZATION_LOSSES)
        # Update global default collections.
        _add_elements_to_collection(self.updates, ops.GraphKeys.UPDATE_OPS)
        self.built = True
        return outputs

    def add_variable(self, name, shape, dtype=None, trainable=True,
                     initializer=None, regularizer=None):
        if dtype is None:
            dtype = self.dtype
        existing_variables = set(tf_variables.global_variables())

        with vs.variable_scope(self._scope,
                               reuse=self.built or self._reuse) as scope:
            with ops.name_scope(scope.original_name_scope):

                full_name = get_tensor_scope() + name
                variable = vs.get_variable(name,
                                           shape=shape,
                                           initializer=initializer,
                                           dtype=dtypes.as_dtype(dtype),
                                           trainable=trainable and self.trainable)
                if variable in existing_variables:
                    # Work only if the layer is built
                    return variable
                if regularizer:
                    raise NotImplementedError()

        if trainable:
            self._trainable_weights.append(variable)
        else:
            self._non_trainable_weights.append(variable)
        return variable

    def apply(self, inputs, *args, **kwargs):
        return self.__call__(inputs, *args, **kwargs)


class InputSpec(object):
    def __init__(self, dtype=None, shape=None,
                 ndim=None, max_ndim=None, min_ndim=None, axes=None):
        self.dtype = dtype
        self.shape = shape
        if shape is not None:
            self.ndim = len(shape)
        else:
            self.ndim = ndim
        self.max_ndim = max_ndim
        self.min_ndim = min_ndim
        self.axes = axes or {}


def _to_snake_case(name):
    intermediate = re.sub('(.)([A-Z][a-z0-9]+)', r'\1_\2', name)
    insecure = re.sub('([a-z])([A-Z])', r'\1_\2', intermediate).lower()
    if insecure[0] != '_': return insecure
    return 'private' + insecure


PER_GRAPH_LAYER_NAME_UIDS = weakref.WeakKeyDictionary()


def _unique_layer_name(name):
    graph = ops.get_default_graph()
    if graph not in PER_GRAPH_LAYER_NAME_UIDS:
        PER_GRAPH_LAYER_NAME_UIDS[graph] = collections.defaultdict(int)
    layer_name_uids = PER_GRAPH_LAYER_NAME_UIDS[graph]
    layer_name_uids[name] += 1


def _to_list(x):
    if isinstance(x, (list, tuple)):
        return list(x)
    return [x]


def _add_elements_to_collection(elements, collection_list):
    elements = _to_list(elements)
    collection_list = _to_list(collection_list)
    for name in collection_list:
        collection = ops.get_collection_ref(name)
        collection_set = set(collection)
        for element in elements:
            if element not in collection_set:
                collection.append(element)
