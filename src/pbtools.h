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

#ifndef PBTOOLS_H
#define PBTOOLS_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define PBTOOLS_BAD_WIRE_TYPE                                   1
#define PBTOOLS_OUT_OF_DATA                                     2
#define PBTOOLS_OUT_OF_MEMORY                                   3
#define PBTOOLS_ENCODE_BUFFER_FULL                              4

struct pbtools_heap_t {
    char *buf_p;
    int size;
    int pos;
};

struct pbtools_int32_t {
    int32_t value;
    struct pbtools_int32_t *next_p;
};

struct pbtools_int64_t {
    int64_t value;
    struct pbtools_int64_t *next_p;
};

struct pbtools_uint32_t {
    uint32_t value;
    struct pbtools_uint32_t *next_p;
};

struct pbtools_uint64_t {
    uint64_t value;
    struct pbtools_uint64_t *next_p;
};

struct pbtools_bool_t {
    bool value;
    struct pbtools_bool_t *next_p;
};

struct pbtools_string_t {
    char *value_p;
    struct pbtools_string_t *next_p;
};

struct pbtools_bytes_t {
    uint8_t *buf_p;
    size_t size;
    struct pbtools_bytes_t *next_p;
};

struct pbtools_encoder_t {
    uint8_t *buf_p;
    int size;
    int pos;
};

struct pbtools_decoder_t {
    const uint8_t *buf_p;
    int size;
    int pos;
    struct pbtools_heap_t *heap_p;
};

struct pbtools_heap_t *pbtools_heap_new(void *buf_p,
                                        size_t size);

void *pbtools_heap_alloc(struct pbtools_heap_t *self_p,
                         size_t size);

void pbtools_encoder_init(struct pbtools_encoder_t *self_p,
                          uint8_t *buf_p,
                          size_t size);

int pbtools_encoder_get_result(struct pbtools_encoder_t *self_p);

void pbtools_encoder_abort(struct pbtools_encoder_t *self_p,
                           int error);

void pbtools_encoder_put(struct pbtools_encoder_t *self_p,
                         uint8_t value);

void pbtools_encoder_write(struct pbtools_encoder_t *self_p,
                           uint8_t *buf_p,
                           int size);

void pbtools_encoder_write_tag(struct pbtools_encoder_t *self_p,
                               int field_number,
                               int wire_type);

void pbtools_encoder_write_varint(struct pbtools_encoder_t *self_p,
                                  int field_number,
                                  int wire_type,
                                  uint64_t value);

void pbtools_encoder_write_int32(struct pbtools_encoder_t *self_p,
                                 int field_number,
                                 int32_t value);

void pbtools_encoder_write_int64(struct pbtools_encoder_t *self_p,
                                 int field_number,
                                 int64_t value);

void pbtools_encoder_write_sint32(struct pbtools_encoder_t *self_p,
                                  int field_number,
                                  int32_t value);

void pbtools_encoder_write_sint64(struct pbtools_encoder_t *self_p,
                                  int field_number,
                                  int64_t value);

void pbtools_encoder_write_uint32(struct pbtools_encoder_t *self_p,
                                  int field_number,
                                  uint32_t value);

void pbtools_encoder_write_uint64(struct pbtools_encoder_t *self_p,
                                  int field_number,
                                  uint64_t value);

void pbtools_encoder_write_fixed32(struct pbtools_encoder_t *self_p,
                                   int field_number,
                                   uint32_t value);

void pbtools_encoder_write_fixed64(struct pbtools_encoder_t *self_p,
                                   int field_number,
                                   uint64_t value);

void pbtools_encoder_write_sfixed32(struct pbtools_encoder_t *self_p,
                                    int field_number,
                                    int32_t value);

void pbtools_encoder_write_sfixed64(struct pbtools_encoder_t *self_p,
                                    int field_number,
                                    int64_t value);

void pbtools_encoder_write_float(struct pbtools_encoder_t *self_p,
                                 int field_number,
                                 float value);

void pbtools_encoder_write_double(struct pbtools_encoder_t *self_p,
                                  int field_number,
                                  double value);

void pbtools_encoder_write_bool(struct pbtools_encoder_t *self_p,
                                int field_number,
                                bool value);

void pbtools_encoder_write_string(struct pbtools_encoder_t *self_p,
                                  int field_number,
                                  char *value_p);

void pbtools_encoder_write_bytes(struct pbtools_encoder_t *self_p,
                                 int field_number,
                                 struct pbtools_bytes_t *value_p);

void pbtools_decoder_init(struct pbtools_decoder_t *self_p,
                          const uint8_t *buf_p,
                          size_t size,
                          struct pbtools_heap_t *heap_p);

int pbtools_decoder_get_result(struct pbtools_decoder_t *self_p);

void pbtools_decoder_abort(struct pbtools_decoder_t *self_p,
                           int error);

bool pbtools_decoder_available(struct pbtools_decoder_t *self_p);

uint8_t pbtools_decoder_get(struct pbtools_decoder_t *self_p);

void pbtools_decoder_read(struct pbtools_decoder_t *self_p,
                          uint8_t *buf_p,
                          int size);

uint64_t pbtools_decoder_read_varint(struct pbtools_decoder_t *self_p,
                                     int wire_type);

int pbtools_decoder_read_tag(struct pbtools_decoder_t *self_p,
                             int *wire_type_p);

int32_t pbtools_decoder_read_int32(struct pbtools_decoder_t *self_p,
                                   int wire_type);

int64_t pbtools_decoder_read_int64(struct pbtools_decoder_t *self_p,
                                   int wire_type);

int32_t pbtools_decoder_read_sint32(struct pbtools_decoder_t *self_p,
                                    int wire_type);

int64_t pbtools_decoder_read_sint64(struct pbtools_decoder_t *self_p,
                                    int wire_type);

uint32_t pbtools_decoder_read_uint32(struct pbtools_decoder_t *self_p,
                                     int wire_type);

uint64_t pbtools_decoder_read_uint64(struct pbtools_decoder_t *self_p,
                                     int wire_type);

uint32_t pbtools_decoder_read_fixed32(struct pbtools_decoder_t *self_p,
                                      int wire_type);

uint64_t pbtools_decoder_read_fixed64(struct pbtools_decoder_t *self_p,
                                      int wire_type);

int32_t pbtools_decoder_read_sfixed32(struct pbtools_decoder_t *self_p,
                                      int wire_type);

int64_t pbtools_decoder_read_sfixed64(struct pbtools_decoder_t *self_p,
                                      int wire_type);

float pbtools_decoder_read_float(struct pbtools_decoder_t *self_p,
                                 int wire_type);

double pbtools_decoder_read_double(struct pbtools_decoder_t *self_p,
                                   int wire_type);

bool pbtools_decoder_read_bool(struct pbtools_decoder_t *self_p,
                               int wire_type);

char *pbtools_decoder_read_string(struct pbtools_decoder_t *self_p,
                                  int wire_type);

void pbtools_decoder_read_bytes(struct pbtools_decoder_t *self_p,
                                int wire_type,
                                struct pbtools_bytes_t *bytes_p);

#endif
