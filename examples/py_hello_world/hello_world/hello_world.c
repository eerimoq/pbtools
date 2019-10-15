#include <Python.h>

static PyMethodDef module_methods[] = {
    { NULL }
};

static PyModuleDef module = {
   PyModuleDef_HEAD_INIT,
   .m_name = "hello_world",
   .m_doc = NULL,
   .m_size = -1,
   .m_methods = module_methods
};

PyMODINIT_FUNC PyInit_hello_world(void)
{
    PyObject *m_p;

    m_p = PyModule_Create(&module);

    if (m_p == NULL) {
        return (NULL);
    }

    return (m_p);
}
