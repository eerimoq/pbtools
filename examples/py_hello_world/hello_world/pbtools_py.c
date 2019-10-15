/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Erik Moqvist
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "pbtools_py.h"

int pbtools_py_init(pbtools_py_new_t new,
                    void **message_pp,
                    void **workspace_pp,
                    size_t workspace_size)
{
    *workspace_pp = PyMem_Malloc(workspace_size);

    if (*workspace_pp == NULL) {
        return (-1);
    }

    *message_pp = new(*workspace_pp, workspace_size);

    if (*message_pp == NULL) {
        PyMem_Free(*workspace_pp);

        return (-1);
    }

    return (0);
}

PyObject *pbtools_py_encode(pbtools_py_encode_t encode,
                            void *message_p,
                            size_t size)
{
    int res;
    void *encoded_p;
    PyObject *bytes_p;

    encoded_p = PyMem_Malloc(size);

    if (encoded_p == NULL) {
        return (NULL);
    }

    res = encode(message_p, encoded_p, size);

    if (res < 0) {
        PyMem_Free(encoded_p);

        return (NULL);
    }

    bytes_p = PyBytes_FromStringAndSize(encoded_p, res);
    PyMem_Free(encoded_p);

    return (bytes_p);
}

PyObject *pbtools_py_decode(pbtools_py_decode_t decode,
                            void *message_p,
                            PyObject *bytes_p)
{
    Py_ssize_t size;
    char *encoded_p;
    int res;

    res = PyBytes_AsStringAndSize(bytes_p, &encoded_p, &size);

    if (res == -1) {
        return (NULL);
    }

    size = decode(message_p, (uint8_t *)encoded_p, size);

    if (size < 0) {
        return (NULL);
    }

    Py_INCREF(Py_None);

    return (Py_None);
}

PyObject *pbtools_py_getint32(int32_t value)
{
    return (PyLong_FromLong(value));
}

int pbtools_py_setint32(int32_t *dst_p, PyObject *value_p)
{
    long value;

    if (value_p == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the last attribute");

        return (-1);
    }

    value = PyLong_AsLong(value_p);

    if ((value == -1) && PyErr_Occurred()) {
        return (-1);
    }

    if ((value < -0x80000000L) || (value > 0x7fffffffL)) {
        PyErr_SetString(PyExc_ValueError, "int32 out of range");

        return (-1);
    }

    *dst_p = value;

    return (0);
}
