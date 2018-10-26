==============
:mod:`Compile`
==============

.. toctree::
   :hidden:


Quick Shortcut
--------------

==============================      =======================================================================
List                                Brief
==============================      =======================================================================
`function`_                         Return a callable function that will compute outputs.
`shared`_                           Construct a Tensor initialized with numerical values.
`scan`_                             Run a dynamic loop of the given one step function.
==============================      =======================================================================


API Reference
-------------

.. automodule:: dragon.vm.theano.compile.function
    :members:

.. automodule:: dragon.vm.theano.compile.sharedvalue
    :members:

.. automodule:: dragon.vm.theano.compile.scan
    :members:

.. _config.SetDebugMode(*args, **kwargs): ../../config.html#dragon.config.SetDebugMode
.. _memonger.share_grads(*args, **kwargs): ../../memonger.html#dragon.memonger.share_grads
.. _config.EnableCPU(): ../../config.html#dragon.config.EnableCPU
.. _config.EnableCUDA(*args, **kwargs): ../../config.html#dragon.config.EnableCUDA
.. _config.SetRandomSeed(*args, **kwargs): ../../config.html#dragon.config.SetRandomSeed
.. _T.grad(*args, **kwargs): tensor.html#dragon.vm.theano.gradient.grad

.. _function: #dragon.vm.theano.compile.function.function
.. _shared: #dragon.vm.theano.compile.sharedvalue.shared
.. _scan: #dragon.vm.theano.compile.scan.scan


