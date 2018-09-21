
#include <Python.h>

#pragma comment(lib,"python36.lib")

int test_python_class()
{
  Py_Initialize();

  PyObject * pModule = NULL;
  PyObject * pFunc = NULL;
  PyObject * pArg = NULL;
  PyObject * pClass = NULL;
  PyObject * pObject = NULL;

  pModule = PyImport_ImportModule("test2");
  pClass = PyObject_GetAttrString(pModule, "TestClass");//�õ��Ǹ���
  pArg = PyTuple_New(1);
  PyTuple_SetItem(pArg, 0, Py_BuildValue("s", "Jacky"));
  pObject = PyEval_CallObject(pClass, pArg);//����һ�����󣬻��߽���ʵ��

  pFunc = PyObject_GetAttrString(pObject, "printName");//�õ���ʵ���ĳ�Ա����
  PyEval_CallObject(pFunc, NULL);//ִ�и�ʵ���ĳ�Ա����

  Py_Finalize();
  return 0;
}

#include <boost/python.hpp>
#include "caffe\proto\caffe.pb.cc"

namespace bp = boost::python;

int test_python_caffe() {
  Py_Initialize();
  caffe::LayerParameter param;
  try {
    bp::object module = bp::import("pythonLayer");
    bp::object layer = module.attr("Data_Layer_train")(param);
    return 0;
  }
  catch (bp::error_already_set) {
    PyErr_Print();
    throw;
  }
  return 0;
}
