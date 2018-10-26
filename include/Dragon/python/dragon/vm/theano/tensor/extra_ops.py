# --------------------------------------------------------
# Theano @ Dragon
# Copyright(c) 2017 SeetaTech
# Written by Ting Pan
# --------------------------------------------------------

from dragon.core.tensor import Tensor
import dragon.ops as ops

def cumsum(x, axis=None):
    """Compute the cumulative sum along the given axis.

    Parameters
    ----------
    x : Tensor
        The input tensor.
    axis : int
        The axis to sum. Default is ``None`` (Along all axes).

    """
    raise NotImplementedError()


def cumprod(x, axis=None):
    """Compute the cumulative product along the given axis.

    Parameters
    ----------
    x : Tensor
        The input tensor.
    axis : int
        The axis to sum. Default is ``None`` (Along all axes).

    """
    raise NotImplementedError()


def to_one_hot(y, nb_class, **kwargs):
    """Generate a matrix where each row corresponds to the one hot encoding.

    The ``y`` should be a 1d vector.

    Parameters
    ----------
    y: Tensor
        The input tensor.
    nb_class : int
        The number of classes.

    Returns
    -------
    Tensor
        The one hot matrix.

    """
    flat_y = ops.Flatten(y, keep_axes=1)
    return ops.OneHot(flat_y, depth=nb_class)
