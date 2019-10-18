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

#ifndef PBTOOLS_PY_H
#define PBTOOLS_PY_H

#include <Python.h>
#include "c/pbtools.h"

typedef void *(*pbtools_py_new_t)(void *workspace_p, size_t size);
typedef int (*pbtools_py_encode_t)(void *message_p,
                                   uint8_t *encoded_p,
                                   size_t size);
typedef int (*pbtools_py_decode_t)(void *message_p,
                                   uint8_t *encoded_p,
                                   size_t size);
typedef int (*pbtools_py_alloc_t)(void *message_p, size_t size);
typedef void (*pbtools_py_set_t)(void *message_p, PyObject *decoded_p);
typedef PyObject *(*pbtools_py_get_t)(void *message_p);

int pbtools_py_init(pbtools_py_new_t new,
                    void **message_pp,
                    void **workspace_pp,
                    size_t workspace_size);

PyObject *pbtools_py_encode(pbtools_py_encode_t encode,
                            void *message_p,
                            void *workspace_p,
                            size_t size);

int pbtools_py_decode(pbtools_py_decode_t decode,
                      void *message_p,
                      PyObject *bytes_p);

void pbtools_py_set_string(char **dst_pp,
                           PyObject *decoded_p,
                           const char *key_p);

void pbtools_py_get_string(char *src_p,
                           PyObject *decoded_p,
                           const char *key_p);

void pbtools_py_set_int32(int32_t *dst_p,
                          PyObject *decoded_p,
                          const char *key_p);

void pbtools_py_get_int32(int32_t src,
                          PyObject *decoded_p,
                          const char *key_p);

void pbtools_py_set_repeated(void *message_p,
                             pbtools_py_alloc_t alloc,
                             pbtools_py_set_t set,
                             struct pbtools_repeated_message_t *repeated_p,
                             PyObject *decoded_p,
                             const char *key_p);

void pbtools_py_get_repeated(void *message_p,
                             pbtools_py_get_t get,
                             struct pbtools_repeated_message_t *repeated_p,
                             PyObject *decoded_p,
                             const char *key_p);

#endif
