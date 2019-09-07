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

/* Errors. */
#define PBTOOLS_BAD_WIRE_TYPE                                   1
#define PBTOOLS_OUT_OF_DATA                                     2
#define PBTOOLS_OUT_OF_MEMORY                                   3
#define PBTOOLS_ENCODE_BUFFER_FULL                              4
#define PBTOOLS_BAD_FIELD_NUMBER                                5
#define PBTOOLS_VARINT_OVERFLOW                                 6
#define PBTOOLS_SEEK_OVERFLOW                                   7
#define PBTOOLS_LENGTH_DELIMITED_OVERFLOW                       8
#define PBTOOLS_STRING_TOO_LONG                                 9
#define PBTOOLS_BYTES_TOO_LONG                                 10

/* Wire types. */
#define PBTOOLS_WIRE_TYPE_VARINT            0
#define PBTOOLS_WIRE_TYPE_FIXED_64          1
#define PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED  2
#define PBTOOLS_WIRE_TYPE_FIXED_32          5

struct pbtools_heap_t {
    char *buf_p;
    int size;
    int pos;
};

struct pbtools_message_base_t {
    struct pbtools_heap_t *heap_p;
    struct pbtools_message_base_t *next_p;
};

struct pbtools_repeated_message_t {
    int length;
    struct pbtools_message_base_t **items_pp;
    struct pbtools_message_base_t *head_p;
    struct pbtools_message_base_t *tail_p;
};

struct pbtools_int32_t {
    struct pbtools_int32_t *next_p;
    int32_t value;
};

struct pbtools_int64_t {
    struct pbtools_int64_t *next_p;
    int64_t value;
};

struct pbtools_uint32_t {
    struct pbtools_uint32_t *next_p;
    uint32_t value;
};

struct pbtools_uint64_t {
    struct pbtools_uint64_t *next_p;
    uint64_t value;
};

struct pbtools_float_t {
    struct pbtools_float_t *next_p;
    float value;
};

struct pbtools_double_t {
    struct pbtools_double_t *next_p;
    double value;
};

struct pbtools_bool_t {
    struct pbtools_bool_t *next_p;
    bool value;
};

struct pbtools_bytes_t {
    struct pbtools_bytes_t *next_p;
    uint8_t *buf_p;
    size_t size;
};

struct pbtools_repeated_int32_t {
    int length;
    struct pbtools_int32_t **items_pp;
    struct pbtools_int32_t *head_p;
    struct pbtools_int32_t *tail_p;
};

struct pbtools_repeated_int64_t {
    int length;
    struct pbtools_int64_t **items_pp;
    struct pbtools_int64_t *head_p;
    struct pbtools_int64_t *tail_p;
};

struct pbtools_repeated_uint32_t {
    int length;
    struct pbtools_uint32_t **items_pp;
    struct pbtools_uint32_t *head_p;
    struct pbtools_uint32_t *tail_p;
};

struct pbtools_repeated_uint64_t {
    int length;
    struct pbtools_uint64_t **items_pp;
    struct pbtools_uint64_t *head_p;
    struct pbtools_uint64_t *tail_p;
};

struct pbtools_repeated_float_t {
    int length;
    struct pbtools_float_t **items_pp;
    struct pbtools_float_t *head_p;
    struct pbtools_float_t *tail_p;
};

struct pbtools_repeated_double_t {
    int length;
    struct pbtools_double_t **items_pp;
    struct pbtools_double_t *head_p;
    struct pbtools_double_t *tail_p;
};

struct pbtools_repeated_bool_t {
    int length;
    struct pbtools_bool_t **items_pp;
    struct pbtools_bool_t *head_p;
    struct pbtools_bool_t *tail_p;
};

struct pbtools_repeated_string_t {
    int length;
    struct pbtools_bytes_t **items_pp;
    struct pbtools_bytes_t *head_p;
    struct pbtools_bytes_t *tail_p;
};

struct pbtools_repeated_bytes_t {
    int length;
    struct pbtools_bytes_t **items_pp;
    struct pbtools_bytes_t *head_p;
    struct pbtools_bytes_t *tail_p;
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

typedef void (*pbtools_message_init_t)(void *self_p,
                                       struct pbtools_heap_t *heap_p,
                                       void *next_p);

typedef int (*pbtools_message_encode_inner_t)(
    struct pbtools_encoder_t *encoder_p,
    void *self_p);

typedef int (*pbtools_message_decode_inner_t)(
    struct pbtools_decoder_t *decoder_p,
    void *self_p);

struct pbtools_heap_t *pbtools_heap_new(void *buf_p,
                                        size_t size);

void *pbtools_heap_alloc(struct pbtools_heap_t *self_p,
                         int size);

void *pbtools_decoder_heap_alloc(struct pbtools_decoder_t *self_p,
                                 int size);

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
                                  uint64_t value);

void pbtools_encoder_write_tagged_varint(struct pbtools_encoder_t *self_p,
                                         int field_number,
                                         int wire_type,
                                         uint64_t value);

void pbtools_encoder_write_length_delimited(struct pbtools_encoder_t *self_p,
                                            int field_number,
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

void pbtools_encoder_write_enum(struct pbtools_encoder_t *self_p,
                                int field_number,
                                int value);

void pbtools_encoder_write_string(struct pbtools_encoder_t *self_p,
                                  int field_number,
                                  struct pbtools_bytes_t *string_p);

void pbtools_encoder_write_bytes(struct pbtools_encoder_t *self_p,
                                 int field_number,
                                 struct pbtools_bytes_t *value_p);

void pbtools_encoder_write_repeated_int32(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_int32_t *repeated_p);

void pbtools_encoder_write_repeated_int64(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_int64_t *repeated_p);

void pbtools_encoder_write_repeated_sint32(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_int32_t *repeated_p);

void pbtools_encoder_write_repeated_sint64(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_int64_t *repeated_p);

void pbtools_encoder_write_repeated_uint32(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_uint32_t *repeated_p);

void pbtools_encoder_write_repeated_uint64(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_uint64_t *repeated_p);

void pbtools_encoder_write_repeated_fixed32(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_uint32_t *repeated_p);

void pbtools_encoder_write_repeated_fixed64(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_uint64_t *repeated_p);

void pbtools_encoder_write_repeated_sfixed32(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_int32_t *repeated_p);

void pbtools_encoder_write_repeated_sfixed64(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_int64_t *repeated_p);

void pbtools_encoder_write_repeated_float(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_float_t *repeated_p);

void pbtools_encoder_write_repeated_double(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_double_t *repeated_p);

void pbtools_encoder_write_repeated_bool(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_bool_t *repeated_p);

void pbtools_encoder_write_repeated_string(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_string_t *repeated_p);

void pbtools_encoder_write_repeated_bytes(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_bytes_t *repeated_p);

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
                          size_t size);

uint64_t pbtools_decoder_read_varint(struct pbtools_decoder_t *self_p);

uint64_t pbtools_decoder_read_varint_check_wire_type(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    int expected_wire_type);

uint64_t pbtools_decoder_read_varint_check_wire_type_varint(
    struct pbtools_decoder_t *self_p,
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

int pbtools_decoder_read_enum(struct pbtools_decoder_t *self_p,
                              int wire_type);

void pbtools_decoder_read_string(struct pbtools_decoder_t *self_p,
                                 int wire_type,
                                 struct pbtools_bytes_t *string_p);

void pbtools_decoder_read_bytes(struct pbtools_decoder_t *self_p,
                                int wire_type,
                                struct pbtools_bytes_t *bytes_p);

int pbtools_alloc_repeated_int32(struct pbtools_message_base_t *self_p,
                                 int length,
                                 struct pbtools_repeated_int32_t *repeated_p);

void pbtools_decoder_read_repeated_int32(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_int32_t *repeated_p);

void pbtools_decoder_finalize_repeated_int32(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_int32_t *repeated_p);

int pbtools_alloc_repeated_int64(struct pbtools_message_base_t *self_p,
                                 int length,
                                 struct pbtools_repeated_int64_t *repeated_p);

void pbtools_decoder_read_repeated_int64(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_int64_t *repeated_p);

void pbtools_decoder_finalize_repeated_int64(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_int64_t *repeated_p);

int pbtools_alloc_repeated_uint32(struct pbtools_message_base_t *self_p,
                                  int length,
                                  struct pbtools_repeated_uint32_t *repeated_p);

void pbtools_decoder_read_repeated_uint32(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_uint32_t *repeated_p);

void pbtools_decoder_finalize_repeated_uint32(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_uint32_t *repeated_p);

int pbtools_alloc_repeated_uint64(struct pbtools_message_base_t *self_p,
                                  int length,
                                  struct pbtools_repeated_uint64_t *repeated_p);

void pbtools_decoder_read_repeated_uint64(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_uint64_t *repeated_p);

void pbtools_decoder_finalize_repeated_uint64(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_uint64_t *repeated_p);

int pbtools_alloc_repeated_sint32(struct pbtools_message_base_t *self_p,
                                  int length,
                                  struct pbtools_repeated_int32_t *repeated_p);

void pbtools_decoder_read_repeated_sint32(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_int32_t *repeated_p);

void pbtools_decoder_finalize_repeated_sint32(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_int32_t *repeated_p);

int pbtools_alloc_repeated_sint64(struct pbtools_message_base_t *self_p,
                                  int length,
                                  struct pbtools_repeated_int64_t *repeated_p);

void pbtools_decoder_read_repeated_sint64(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_int64_t *repeated_p);

void pbtools_decoder_finalize_repeated_sint64(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_int64_t *repeated_p);

int pbtools_alloc_repeated_fixed32(struct pbtools_message_base_t *self_p,
                                   int length,
                                   struct pbtools_repeated_uint32_t *repeated_p);

void pbtools_decoder_read_repeated_fixed32(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_uint32_t *repeated_p);

void pbtools_decoder_finalize_repeated_fixed32(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_uint32_t *repeated_p);

int pbtools_alloc_repeated_fixed64(struct pbtools_message_base_t *self_p,
                                   int length,
                                   struct pbtools_repeated_uint64_t *repeated_p);

void pbtools_decoder_read_repeated_fixed64(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_uint64_t *repeated_p);

void pbtools_decoder_finalize_repeated_fixed64(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_uint64_t *repeated_p);

int pbtools_alloc_repeated_sfixed32(struct pbtools_message_base_t *self_p,
                                    int length,
                                    struct pbtools_repeated_int32_t *repeated_p);

void pbtools_decoder_read_repeated_sfixed32(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_int32_t *repeated_p);

void pbtools_decoder_finalize_repeated_sfixed32(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_int32_t *repeated_p);

int pbtools_alloc_repeated_sfixed64(struct pbtools_message_base_t *self_p,
                                    int length,
                                    struct pbtools_repeated_int64_t *repeated_p);

void pbtools_decoder_read_repeated_sfixed64(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_int64_t *repeated_p);

void pbtools_decoder_finalize_repeated_sfixed64(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_int64_t *repeated_p);

int pbtools_alloc_repeated_float(struct pbtools_message_base_t *self_p,
                                 int length,
                                 struct pbtools_repeated_float_t *repeated_p);

void pbtools_decoder_read_repeated_float(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_float_t *repeated_p);

void pbtools_decoder_finalize_repeated_float(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_float_t *repeated_p);

int pbtools_alloc_repeated_double(struct pbtools_message_base_t *self_p,
                                 int length,
                                 struct pbtools_repeated_double_t *repeated_p);

void pbtools_decoder_read_repeated_double(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_double_t *repeated_p);

void pbtools_decoder_finalize_repeated_double(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_double_t *repeated_p);

int pbtools_alloc_repeated_bool(struct pbtools_message_base_t *self_p,
                                int length,
                                struct pbtools_repeated_bool_t *repeated_p);

void pbtools_decoder_read_repeated_bool(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_bool_t *repeated_p);

void pbtools_decoder_finalize_repeated_bool(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_bool_t *repeated_p);

int pbtools_alloc_repeated_string(struct pbtools_message_base_t *self_p,
                                  int length,
                                  struct pbtools_repeated_string_t *repeated_p);

void pbtools_decoder_read_repeated_string(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_string_t *repeated_p);

void pbtools_decoder_finalize_repeated_string(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_string_t *repeated_p);

int pbtools_alloc_repeated_bytes(struct pbtools_message_base_t *self_p,
                                 int length,
                                 struct pbtools_repeated_bytes_t *repeated_p);

void pbtools_decoder_read_repeated_bytes(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_bytes_t *repeated_p);

void pbtools_decoder_finalize_repeated_bytes(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_bytes_t *repeated_p);

void pbtools_decoder_init_slice(struct pbtools_decoder_t *self_p,
                                struct pbtools_decoder_t *parent_p,
                                int size);

void pbtools_decoder_seek(struct pbtools_decoder_t *self_p,
                          int offset);

void pbtools_decoder_skip_field(struct pbtools_decoder_t *self_p,
                                int wire_type);

void pbtools_set_string(struct pbtools_bytes_t *self_p,
                        char *string_p);

char *pbtools_get_string(struct pbtools_bytes_t *self_p);

void pbtools_string_init(struct pbtools_bytes_t *self_p);

void pbtools_bytes_init(struct pbtools_bytes_t *self_p);

void *pbtools_message_new(void *workspace_p,
                          size_t size,
                          size_t message_size,
                          pbtools_message_init_t message_init);

int pbtools_message_encode(struct pbtools_message_base_t *self_p,
                           uint8_t *encoded_p,
                           size_t size,
                           pbtools_message_encode_inner_t message_encode_inner);

int pbtools_message_decode(struct pbtools_message_base_t *self_p,
                           const uint8_t *encoded_p,
                           size_t size,
                           pbtools_message_decode_inner_t message_decode_inner);

void pbtools_encode_repeated_inner(
    struct pbtools_encoder_t *encoder_p,
    int field_number,
    struct pbtools_repeated_message_t *repeated_p,
    pbtools_message_encode_inner_t message_encode_inner);

void pbtools_decode_repeated_inner(
    struct pbtools_decoder_t *decoder_p,
    int wire_type,
    struct pbtools_repeated_message_t *repeated_p,
    size_t item_size,
    pbtools_message_init_t message_init,
    pbtools_message_decode_inner_t message_decode_inner);

void pbtools_finalize_repeated_inner(
    struct pbtools_decoder_t *decoder_p,
    struct pbtools_repeated_message_t *repeated_p);

#if 0
#    include <stdio.h>
#    define PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#    define PRINTF(fmt, ...)
#endif

#endif
