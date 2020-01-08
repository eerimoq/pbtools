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

#ifdef __cplusplus
extern "C" {{
#endif

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Compile time configuration. */

/* Support for float and double if 1. */
#ifndef PBTOOLS_CONFIG_FLOAT
#    define PBTOOLS_CONFIG_FLOAT                                1
#endif

/* Support for 64-bit numbers if 1, otherwise limit to 32-bit. */
#ifndef PBTOOLS_CONFIG_64BIT
#    define PBTOOLS_CONFIG_64BIT                                1
#endif

/* Errors. */
#define PBTOOLS_BAD_WIRE_TYPE                                   1
#define PBTOOLS_OUT_OF_DATA                                     2
#define PBTOOLS_OUT_OF_MEMORY                                   3
#define PBTOOLS_ENCODE_BUFFER_FULL                              4
#define PBTOOLS_BAD_FIELD_NUMBER                                5
#define PBTOOLS_VARINT_OVERFLOW                                 6
#define PBTOOLS_SEEK_OVERFLOW                                   7
#define PBTOOLS_LENGTH_DELIMITED_OVERFLOW                       8

/* Wire types. */
#define PBTOOLS_WIRE_TYPE_VARINT                                0
#define PBTOOLS_WIRE_TYPE_64_BIT                                1
#define PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED                      2
#define PBTOOLS_WIRE_TYPE_32_BIT                                5

struct pbtools_heap_t {
    char *buf_p;
    int size;
    int pos;
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

struct pbtools_repeated_info_t {
    struct pbtools_decoder_t decoder;
    int length;
    int tag;
    int wire_type;
};

struct pbtools_bytes_t {
    uint8_t *buf_p;
    size_t size;
};

struct pbtools_message_base_t {
    struct pbtools_heap_t *heap_p;
};

struct pbtools_repeated_int32_t {
    int length;
    int32_t *items_p;
};

struct pbtools_repeated_int64_t {
    int length;
    int64_t *items_p;
};

struct pbtools_repeated_uint32_t {
    int length;
    uint32_t *items_p;
};

struct pbtools_repeated_uint64_t {
    int length;
    uint64_t *items_p;
};

#if PBTOOLS_CONFIG_FLOAT == 1

struct pbtools_repeated_float_t {
    int length;
    float *items_p;
};

struct pbtools_repeated_double_t {
    int length;
    double *items_p;
};

#endif

struct pbtools_repeated_bool_t {
    int length;
    bool *items_p;
};

struct pbtools_repeated_string_t {
    int length;
    char **items_pp;
};

struct pbtools_repeated_bytes_t {
    int length;
    struct pbtools_bytes_t *items_p;
};

struct pbtools_repeated_message_t {
    int length;
    void *items_p;
};

typedef void (*pbtools_message_init_t)(void *self_p,
                                       struct pbtools_heap_t *heap_p);

typedef void (*pbtools_message_encode_inner_t)(
    struct pbtools_encoder_t *encoder_p,
    void *self_p);

typedef void (*pbtools_message_decode_inner_t)(
    struct pbtools_decoder_t *decoder_p,
    void *self_p);

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

#if PBTOOLS_CONFIG_FLOAT == 1

void pbtools_encoder_write_float(struct pbtools_encoder_t *self_p,
                                 int field_number,
                                 float value);

void pbtools_encoder_write_double(struct pbtools_encoder_t *self_p,
                                  int field_number,
                                  double value);

#endif

void pbtools_encoder_write_bool(struct pbtools_encoder_t *self_p,
                                int field_number,
                                bool value);

void pbtools_encoder_write_enum(struct pbtools_encoder_t *self_p,
                                int field_number,
                                int value);

void pbtools_encoder_write_string(struct pbtools_encoder_t *self_p,
                                  int field_number,
                                  char *value_p);

void pbtools_encoder_write_bytes(struct pbtools_encoder_t *self_p,
                                 int field_number,
                                 struct pbtools_bytes_t *value_p);

void pbtools_encoder_write_int32_always(struct pbtools_encoder_t *self_p,
                                        int field_number,
                                        int32_t value);

void pbtools_encoder_write_int64_always(struct pbtools_encoder_t *self_p,
                                        int field_number,
                                        int64_t value);

void pbtools_encoder_write_sint32_always(struct pbtools_encoder_t *self_p,
                                         int field_number,
                                         int32_t value);

void pbtools_encoder_write_sint64_always(struct pbtools_encoder_t *self_p,
                                         int field_number,
                                         int64_t value);

void pbtools_encoder_write_uint32_always(struct pbtools_encoder_t *self_p,
                                         int field_number,
                                         uint32_t value);

void pbtools_encoder_write_uint64_always(struct pbtools_encoder_t *self_p,
                                         int field_number,
                                         uint64_t value);

void pbtools_encoder_write_fixed32_always(struct pbtools_encoder_t *self_p,
                                          int field_number,
                                          uint32_t value);

void pbtools_encoder_write_fixed64_always(struct pbtools_encoder_t *self_p,
                                          int field_number,
                                          uint64_t value);

void pbtools_encoder_write_sfixed32_always(struct pbtools_encoder_t *self_p,
                                           int field_number,
                                           int32_t value);

void pbtools_encoder_write_sfixed64_always(struct pbtools_encoder_t *self_p,
                                           int field_number,
                                           int64_t value);

#if PBTOOLS_CONFIG_FLOAT == 1

void pbtools_encoder_write_float_always(struct pbtools_encoder_t *self_p,
                                        int field_number,
                                        float value);

void pbtools_encoder_write_double_always(struct pbtools_encoder_t *self_p,
                                         int field_number,
                                         double value);

#endif

void pbtools_encoder_write_bool_always(struct pbtools_encoder_t *self_p,
                                       int field_number,
                                       bool value);

void pbtools_encoder_write_enum_always(struct pbtools_encoder_t *self_p,
                                       int field_number,
                                       int value);

void pbtools_encoder_write_string_always(struct pbtools_encoder_t *self_p,
                                         int field_number,
                                         char *value_p);

void pbtools_encoder_write_bytes_always(struct pbtools_encoder_t *self_p,
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

#if PBTOOLS_CONFIG_FLOAT == 1

void pbtools_encoder_write_repeated_float(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_float_t *repeated_p);

void pbtools_encoder_write_repeated_double(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_double_t *repeated_p);

#endif

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

bool pbtools_decoder_available(struct pbtools_decoder_t *self_p);

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

#if PBTOOLS_CONFIG_FLOAT == 1

float pbtools_decoder_read_float(struct pbtools_decoder_t *self_p,
                                 int wire_type);

double pbtools_decoder_read_double(struct pbtools_decoder_t *self_p,
                                   int wire_type);

#endif

bool pbtools_decoder_read_bool(struct pbtools_decoder_t *self_p,
                               int wire_type);

int pbtools_decoder_read_enum(struct pbtools_decoder_t *self_p,
                              int wire_type);

void pbtools_decoder_read_string(struct pbtools_decoder_t *self_p,
                                 int wire_type,
                                 char **value_pp);

void pbtools_decoder_read_bytes(struct pbtools_decoder_t *self_p,
                                int wire_type,
                                struct pbtools_bytes_t *bytes_p);

int pbtools_alloc_repeated_int32(struct pbtools_message_base_t *self_p,
                                 int length,
                                 struct pbtools_repeated_int32_t *repeated_p);

void pbtools_repeated_info_decode_int32(struct pbtools_repeated_info_t *self_p,
                                        struct pbtools_decoder_t *decoder_p,
                                        int wire_type);

void pbtools_decoder_decode_repeated_int32(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_int32_t *repeated_p);

int pbtools_alloc_repeated_int64(struct pbtools_message_base_t *self_p,
                                 int length,
                                 struct pbtools_repeated_int64_t *repeated_p);

void pbtools_repeated_info_decode_int64(struct pbtools_repeated_info_t *self_p,
                                        struct pbtools_decoder_t *decoder_p,
                                        int wire_type);

void pbtools_decoder_decode_repeated_int64(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_int64_t *repeated_p);

int pbtools_alloc_repeated_uint32(struct pbtools_message_base_t *self_p,
                                  int length,
                                  struct pbtools_repeated_uint32_t *repeated_p);

void pbtools_repeated_info_decode_uint32(struct pbtools_repeated_info_t *self_p,
                                         struct pbtools_decoder_t *decoder_p,
                                         int wire_type);

void pbtools_decoder_decode_repeated_uint32(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_uint32_t *repeated_p);

int pbtools_alloc_repeated_uint64(struct pbtools_message_base_t *self_p,
                                  int length,
                                  struct pbtools_repeated_uint64_t *repeated_p);

void pbtools_repeated_info_decode_uint64(struct pbtools_repeated_info_t *self_p,
                                         struct pbtools_decoder_t *decoder_p,
                                         int wire_type);

void pbtools_decoder_decode_repeated_uint64(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_uint64_t *repeated_p);

int pbtools_alloc_repeated_sint32(struct pbtools_message_base_t *self_p,
                                  int length,
                                  struct pbtools_repeated_int32_t *repeated_p);

void pbtools_repeated_info_decode_sint32(struct pbtools_repeated_info_t *self_p,
                                         struct pbtools_decoder_t *decoder_p,
                                         int wire_type);

void pbtools_decoder_decode_repeated_sint32(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_int32_t *repeated_p);

int pbtools_alloc_repeated_sint64(struct pbtools_message_base_t *self_p,
                                  int length,
                                  struct pbtools_repeated_int64_t *repeated_p);

void pbtools_repeated_info_decode_sint64(struct pbtools_repeated_info_t *self_p,
                                         struct pbtools_decoder_t *decoder_p,
                                         int wire_type);

void pbtools_decoder_decode_repeated_sint64(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_int64_t *repeated_p);

int pbtools_alloc_repeated_fixed32(struct pbtools_message_base_t *self_p,
                                   int length,
                                   struct pbtools_repeated_uint32_t *repeated_p);

void pbtools_repeated_info_decode_fixed32(struct pbtools_repeated_info_t *self_p,
                                          struct pbtools_decoder_t *decoder_p,
                                          int wire_type);

void pbtools_decoder_decode_repeated_fixed32(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_uint32_t *repeated_p);

int pbtools_alloc_repeated_fixed64(struct pbtools_message_base_t *self_p,
                                   int length,
                                   struct pbtools_repeated_uint64_t *repeated_p);

void pbtools_repeated_info_decode_fixed64(struct pbtools_repeated_info_t *self_p,
                                          struct pbtools_decoder_t *decoder_p,
                                          int wire_type);

void pbtools_decoder_decode_repeated_fixed64(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_uint64_t *repeated_p);

int pbtools_alloc_repeated_sfixed32(struct pbtools_message_base_t *self_p,
                                    int length,
                                    struct pbtools_repeated_int32_t *repeated_p);

void pbtools_repeated_info_decode_sfixed32(struct pbtools_repeated_info_t *self_p,
                                           struct pbtools_decoder_t *decoder_p,
                                           int wire_type);

void pbtools_decoder_decode_repeated_sfixed32(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_int32_t *repeated_p);

int pbtools_alloc_repeated_sfixed64(struct pbtools_message_base_t *self_p,
                                    int length,
                                    struct pbtools_repeated_int64_t *repeated_p);

void pbtools_repeated_info_decode_sfixed64(struct pbtools_repeated_info_t *self_p,
                                           struct pbtools_decoder_t *decoder_p,
                                           int wire_type);

void pbtools_decoder_decode_repeated_sfixed64(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_int64_t *repeated_p);

#if PBTOOLS_CONFIG_FLOAT == 1

int pbtools_alloc_repeated_float(struct pbtools_message_base_t *self_p,
                                 int length,
                                 struct pbtools_repeated_float_t *repeated_p);

void pbtools_repeated_info_decode_float(struct pbtools_repeated_info_t *self_p,
                                        struct pbtools_decoder_t *decoder_p,
                                        int wire_type);

void pbtools_decoder_decode_repeated_float(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_float_t *repeated_p);

int pbtools_alloc_repeated_double(struct pbtools_message_base_t *self_p,
                                 int length,
                                 struct pbtools_repeated_double_t *repeated_p);

void pbtools_repeated_info_decode_double(struct pbtools_repeated_info_t *self_p,
                                         struct pbtools_decoder_t *decoder_p,
                                         int wire_type);

void pbtools_decoder_decode_repeated_double(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_double_t *repeated_p);

#endif

int pbtools_alloc_repeated_bool(struct pbtools_message_base_t *self_p,
                                int length,
                                struct pbtools_repeated_bool_t *repeated_p);

void pbtools_repeated_info_decode_bool(struct pbtools_repeated_info_t *self_p,
                                       struct pbtools_decoder_t *decoder_p,
                                       int wire_type);

void pbtools_decoder_decode_repeated_bool(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_bool_t *repeated_p);

int pbtools_alloc_repeated_string(struct pbtools_message_base_t *self_p,
                                  int length,
                                  struct pbtools_repeated_string_t *repeated_p);

void pbtools_repeated_info_decode_string(struct pbtools_repeated_info_t *self_p,
                                         struct pbtools_decoder_t *decoder_p,
                                         int wire_type);

void pbtools_decoder_decode_repeated_string(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_string_t *repeated_p);

int pbtools_alloc_repeated_bytes(struct pbtools_message_base_t *self_p,
                                 int length,
                                 struct pbtools_repeated_bytes_t *repeated_p);

void pbtools_repeated_info_decode_bytes(struct pbtools_repeated_info_t *self_p,
                                        struct pbtools_decoder_t *decoder_p,
                                        int wire_type);

void pbtools_decoder_decode_repeated_bytes(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_bytes_t *repeated_p);

void pbtools_decoder_skip_field(struct pbtools_decoder_t *self_p,
                                int wire_type);

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

int pbtools_sub_message_alloc(
    struct pbtools_message_base_t **message_pp,
    struct pbtools_heap_t *heap_p,
    size_t sub_message_size,
    pbtools_message_init_t message_init);

int pbtools_alloc_repeated(
    struct pbtools_repeated_message_t *repeated_p,
    int length,
    struct pbtools_heap_t *heap_p,
    size_t item_size,
    pbtools_message_init_t message_init);

void pbtools_encode_repeated_inner(
    struct pbtools_encoder_t *encoder_p,
    int field_number,
    struct pbtools_repeated_message_t *repeated_p,
    size_t item_size,
    pbtools_message_encode_inner_t message_encode_inner);

void pbtools_decode_repeated_inner(
    struct pbtools_decoder_t *decoder_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_message_t *repeated_p,
    size_t item_size,
    pbtools_message_init_t message_init,
    pbtools_message_decode_inner_t message_decode_inner);

void pbtools_encoder_sub_message_encode(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_message_base_t *message_p,
    pbtools_message_encode_inner_t encode_inner);

void pbtools_encoder_sub_message_encode_always(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_message_base_t *message_p,
    pbtools_message_encode_inner_t encode_inner);

void pbtools_decoder_sub_message_decode(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_message_base_t **message_pp,
    size_t sub_message_size,
    pbtools_message_init_t message_init,
    pbtools_message_decode_inner_t decode_inner);

const char *pbtools_error_code_to_string(int code);

 void pbtools_repeated_info_init(struct pbtools_repeated_info_t *self_p,
                                 int tag);

void pbtools_repeated_info_decode(struct pbtools_repeated_info_t *self_p,
                                  struct pbtools_decoder_t *decoder_p,
                                  int wire_type);

void pbtools_repeated_numeric_scalar_info_decode(
    struct pbtools_repeated_info_t *self_p,
    struct pbtools_decoder_t *decoder_p,
    int wire_type);

void pbtools_repeated_varint_info_decode(struct pbtools_repeated_info_t *self_p,
                                         struct pbtools_decoder_t *decoder_p,
                                         int wire_type);

#ifdef __cplusplus
}}
#endif

#endif
