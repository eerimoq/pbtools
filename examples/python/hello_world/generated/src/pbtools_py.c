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
        goto out1;
    }

    return (0);

 out1:
    PyMem_Free(*workspace_pp);

    return (-1);
}

PyObject *pbtools_py_encode(pbtools_py_encode_t encode,
                            void *message_p,
                            void *workspace_p,
                            size_t size)
{
    int res;
    void *encoded_p;
    PyObject *bytes_p;

    encoded_p = PyMem_Malloc(size);

    if (encoded_p == NULL) {
        goto out1;
    }

    res = encode(message_p, encoded_p, size);

    if (res < 0) {
        goto out2;
    }

    bytes_p = PyBytes_FromStringAndSize(encoded_p, res);
    PyMem_Free(encoded_p);
    PyMem_Free(workspace_p);

    return (bytes_p);

 out2:
    PyMem_Free(encoded_p);

 out1:
    PyMem_Free(workspace_p);

    return (NULL);
}

int pbtools_py_decode(pbtools_py_decode_t decode,
                      void *message_p,
                      PyObject *bytes_p)
{
    int res;
    Py_ssize_t size;
    char *encoded_p;

    res = PyBytes_AsStringAndSize(bytes_p, &encoded_p, &size);

    if (res == -1) {
        return (-1);
    }

    return (decode(message_p, (uint8_t *)encoded_p, size));
}

void pbtools_py_set_string(char **dst_pp,
                           PyObject *decoded_p,
                           const char *key_p)
{
    PyObject *value_p;
    const char *string_p;

    value_p = PyDict_GetItemString(decoded_p, key_p);

    if (value_p != NULL) {
        string_p = PyUnicode_AsUTF8(value_p);

        if (string_p != NULL) {
            *dst_pp = (char *)string_p;
        }
    }
}

void pbtools_py_get_string(char *src_p,
                           PyObject *decoded_p,
                           const char *key_p)
{
    PyObject *value_p;

    value_p = PyUnicode_FromString(src_p);

    if (value_p != NULL) {
        PyDict_SetItemString(decoded_p, key_p, value_p);
    }
}

void pbtools_py_set_int32(int32_t *dst_p,
                          PyObject *decoded_p,
                          const char *key_p)
{
    PyObject *value_p;
    long value;

    value_p = PyDict_GetItemString(decoded_p, key_p);

    if (value_p != NULL) {
        value = PyLong_AsLong(value_p);

        if ((value == -1) && PyErr_Occurred()) {
            return;
        }

        if ((value < -0x80000000L) || (value > 0x7fffffffL)) {
            PyErr_SetString(PyExc_ValueError, "int32 out of range");

            return;
        }

        *dst_p = value;
    }
}

void pbtools_py_get_int32(int32_t src,
                          PyObject *decoded_p,
                          const char *key_p)
{
    PyObject *value_p;

    value_p = PyLong_FromLong(src);

    if (value_p != NULL) {
        PyDict_SetItemString(decoded_p, key_p, value_p);
    }
}

void pbtools_py_set_repeated(void *message_p,
                             pbtools_py_alloc_t alloc,
                             pbtools_py_set_t set,
                             struct pbtools_repeated_message_t *repeated_p,
                             PyObject *decoded_p,
                             const char *key_p)
{
    int res;
    Py_ssize_t i;
    Py_ssize_t size;
    PyObject *value_p;

    value_p = PyDict_GetItemString(decoded_p, key_p);

    if (value_p != NULL) {
        size = PyList_Size(value_p);

        if (size == -1) {
            return;
        }

        res = alloc(message_p, size);

        if (res != 0) {
            return;
        }

        for (i = 0; i < size; i++) {
            set(repeated_p->items_pp[i], PyList_GET_ITEM(value_p, i));
        }
    }
}

void pbtools_py_get_repeated(void *message_p,
                             pbtools_py_get_t get,
                             struct pbtools_repeated_message_t *repeated_p,
                             PyObject *decoded_p,
                             const char *key_p)
{
    PyObject *list_p;
    int i;

    list_p = PyList_New(repeated_p->length);

    if (list_p == NULL) {
        return;
    }

    for (i = 0; i < repeated_p->length; i++) {
        PyList_SET_ITEM(list_p, i, get(repeated_p->items_pp[i]));
    }

    PyDict_SetItemString(decoded_p, key_p, list_p);
}
