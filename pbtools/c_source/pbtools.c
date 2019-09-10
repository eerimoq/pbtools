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

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include "pbtools.h"

#define WRITE_REPEATED_SCALAR_VALUE_TYPE(type)                          \
    write_repeated_scalar_value_type(                                   \
        self_p,                                                         \
        field_number,                                                   \
        (struct pbtools_repeated_scalar_value_type_t *)repeated_p,      \
        write_repeated_ ## type ## _item);

#define READ_REPEATED_SCALAR_VALUE_TYPE(func_type, type)                \
    read_repeated_scalar_value_type(                                    \
        self_p,                                                         \
        wire_type,                                                      \
        (struct pbtools_repeated_scalar_value_type_t *)repeated_p,      \
        sizeof(struct pbtools_ ## type ## _t),                          \
        read_repeated_ ## func_type ## _item);

typedef void (*pbtools_repeated_scalar_value_type_write_t)(
    struct pbtools_encoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p);

typedef void (*pbtools_repeated_scalar_value_type_read_t)(
    struct pbtools_decoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p);

struct pbtools_heap_t *pbtools_heap_new(void *buf_p,
                                        size_t size)
{
    struct pbtools_heap_t *heap_p;

    if (size >= sizeof(*heap_p)) {
        heap_p = (struct pbtools_heap_t *)buf_p;
        heap_p->buf_p = buf_p;
        heap_p->size = size;
        heap_p->pos = sizeof(*heap_p);
    } else {
        heap_p = NULL;
    }

    return (heap_p);
}

void *pbtools_heap_alloc(struct pbtools_heap_t *self_p,
                         int size)
{
    void *buf_p;
    int left;

    left = (self_p->size - self_p->pos);

    if (size <= left) {
        buf_p = &self_p->buf_p[self_p->pos];
        self_p->pos += size;
    } else {
        buf_p = NULL;
    }

    return (buf_p);
}

void *pbtools_decoder_heap_alloc(struct pbtools_decoder_t *self_p,
                                 int size)
{
    void *buf_p;

    buf_p = pbtools_heap_alloc(self_p->heap_p, size);

    if (buf_p == NULL) {
        pbtools_decoder_abort(self_p, PBTOOLS_OUT_OF_MEMORY);
    }

    return (buf_p);
}

void pbtools_encoder_init(struct pbtools_encoder_t *self_p,
                          uint8_t *buf_p,
                          size_t size)
{
    self_p->buf_p = buf_p;
    self_p->size = size;
    self_p->pos = (size - 1);
}

int pbtools_encoder_get_result(struct pbtools_encoder_t *self_p)
{
    int length;

    PRINTF("pbtools_encoder_get_result(): pos: %d\n", self_p->pos);

    if (self_p->pos >= 0) {
        length = (self_p->size - self_p->pos - 1);
        memmove(self_p->buf_p,
                &self_p->buf_p[self_p->pos + 1],
                length);
    } else {
        length = self_p->pos;
    }

    return (length);
}

void pbtools_encoder_abort(struct pbtools_encoder_t *self_p,
                           int error)
{
    if (self_p->size >= 0) {
        PRINTF("pbtools_encoder_abort(): error: %d\n", error);
        self_p->size = -error;
        self_p->pos = -error;
    }
}

void pbtools_encoder_write(struct pbtools_encoder_t *self_p,
                           uint8_t *buf_p,
                           int size)
{
    if (self_p->pos >= size) {
        self_p->pos -= size;
        memcpy(&self_p->buf_p[self_p->pos + 1], buf_p, size);
    } else {
        pbtools_encoder_abort(self_p, PBTOOLS_ENCODE_BUFFER_FULL);
    }
}

void pbtools_encoder_write_tag(struct pbtools_encoder_t *self_p,
                               int field_number,
                               int wire_type)
{
    uint8_t buf[5];
    int pos;
    uint32_t value;

    value = ((field_number << 3) | wire_type);
    pos = 0;

    while (value > 0) {
        buf[pos++] = (value | 0x80);
        value >>= 7;
    }

    buf[pos - 1] &= 0x7f;
    pbtools_encoder_write(self_p, &buf[0], pos);
}

void pbtools_encoder_write_varint(struct pbtools_encoder_t *self_p,
                                  uint64_t value)
{
    uint8_t buf[10];
    int pos;

    PRINTF("pbtools_encoder_write_varint(): value: 0x%llx\n",
           (unsigned long long)value);

    pos = 0;

    do {
        buf[pos++] = (value | 0x80);
        value >>= 7;
    } while (value > 0);

    buf[pos - 1] &= 0x7f;
    pbtools_encoder_write(self_p, &buf[0], pos);

    PRINTF("pbtools_encoder_write_varint(): done\n");
}

void pbtools_encoder_write_tagged_varint(struct pbtools_encoder_t *self_p,
                                         int field_number,
                                         int wire_type,
                                         uint64_t value)
{
    if (value == 0) {
        return;
    }

    pbtools_encoder_write_varint(self_p, value);
    pbtools_encoder_write_tag(self_p, field_number, wire_type);
}

void pbtools_encoder_write_length_delimited(struct pbtools_encoder_t *self_p,
                                            int field_number,
                                            uint64_t value)
{
    pbtools_encoder_write_varint(self_p, value);
    pbtools_encoder_write_tag(self_p,
                              field_number,
                              PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED);
}

void pbtools_encoder_write_int32(struct pbtools_encoder_t *self_p,
                                 int field_number,
                                 int32_t value)
{
    pbtools_encoder_write_tagged_varint(self_p,
                                        field_number,
                                        PBTOOLS_WIRE_TYPE_VARINT,
                                        (uint64_t)(int64_t)value);
}

void pbtools_encoder_write_int64(struct pbtools_encoder_t *self_p,
                                 int field_number,
                                 int64_t value)
{
    pbtools_encoder_write_tagged_varint(self_p,
                                        field_number,
                                        PBTOOLS_WIRE_TYPE_VARINT,
                                        (uint64_t)value);
}

void pbtools_encoder_write_sint32(struct pbtools_encoder_t *self_p,
                                  int field_number,
                                  int32_t value)
{
    pbtools_encoder_write_sint64(self_p, field_number, value);
}

void pbtools_encoder_write_sint64(struct pbtools_encoder_t *self_p,
                                  int field_number,
                                  int64_t value)
{
    uint64_t data;

    if (value < 0) {
        data = ~((uint64_t)value << 1);
    } else {
        data = ((uint64_t)value << 1);
    }

    pbtools_encoder_write_uint64(self_p, field_number, data);
}

void pbtools_encoder_write_uint32(struct pbtools_encoder_t *self_p,
                                  int field_number,
                                  uint32_t value)
{
    pbtools_encoder_write_uint64(self_p, field_number, value);
}

void pbtools_encoder_write_uint64(struct pbtools_encoder_t *self_p,
                                  int field_number,
                                  uint64_t value)
{
    pbtools_encoder_write_tagged_varint(self_p,
                                        field_number,
                                        PBTOOLS_WIRE_TYPE_VARINT,
                                        value);
}

void pbtools_encoder_write_fixed32_value(struct pbtools_encoder_t *self_p,
                                         uint32_t value)
{
    uint8_t buf[4];

    buf[0] = (value & 0xff);
    buf[1] = ((value >> 8) & 0xff);
    buf[2] = ((value >> 16) & 0xff);
    buf[3] = ((value >> 24) & 0xff);
    pbtools_encoder_write(self_p, &buf[0], 4);
}

void pbtools_encoder_write_fixed32(struct pbtools_encoder_t *self_p,
                                   int field_number,
                                   uint32_t value)
{
    uint8_t buf[4];

    if (value != 0) {
        buf[0] = (value & 0xff);
        buf[1] = ((value >> 8) & 0xff);
        buf[2] = ((value >> 16) & 0xff);
        buf[3] = ((value >> 24) & 0xff);
        pbtools_encoder_write(self_p, &buf[0], 4);
        pbtools_encoder_write_tag(self_p,
                                  field_number,
                                  PBTOOLS_WIRE_TYPE_FIXED_32);
    }
}

void pbtools_encoder_write_fixed64_value(struct pbtools_encoder_t *self_p,
                                         uint64_t value)
{
    uint8_t buf[8];

    buf[0] = (value & 0xff);
    buf[1] = ((value >> 8) & 0xff);
    buf[2] = ((value >> 16) & 0xff);
    buf[3] = ((value >> 24) & 0xff);
    buf[4] = ((value >> 32) & 0xff);
    buf[5] = ((value >> 40) & 0xff);
    buf[6] = ((value >> 48) & 0xff);
    buf[7] = ((value >> 56) & 0xff);
    pbtools_encoder_write(self_p, &buf[0], 8);
}

void pbtools_encoder_write_fixed64(struct pbtools_encoder_t *self_p,
                                   int field_number,
                                   uint64_t value)
{
    uint8_t buf[8];

    if (value != 0) {
        buf[0] = (value & 0xff);
        buf[1] = ((value >> 8) & 0xff);
        buf[2] = ((value >> 16) & 0xff);
        buf[3] = ((value >> 24) & 0xff);
        buf[4] = ((value >> 32) & 0xff);
        buf[5] = ((value >> 40) & 0xff);
        buf[6] = ((value >> 48) & 0xff);
        buf[7] = ((value >> 56) & 0xff);
        pbtools_encoder_write(self_p, &buf[0], 8);
        pbtools_encoder_write_tag(self_p,
                                  field_number,
                                  PBTOOLS_WIRE_TYPE_FIXED_64);
    }
}

void pbtools_encoder_write_sfixed32(struct pbtools_encoder_t *self_p,
                                    int field_number,
                                    int32_t value)
{
    pbtools_encoder_write_fixed32(self_p, field_number, value);
}

void pbtools_encoder_write_sfixed64(struct pbtools_encoder_t *self_p,
                                    int field_number,
                                    int64_t value)
{
    pbtools_encoder_write_fixed64(self_p, field_number, value);
}

void pbtools_encoder_write_float(struct pbtools_encoder_t *self_p,
                                 int field_number,
                                 float value)
{
    uint32_t data;

    memcpy(&data, &value, sizeof(data));
    pbtools_encoder_write_fixed32(self_p, field_number, data);
}

void pbtools_encoder_write_double(struct pbtools_encoder_t *self_p,
                                  int field_number,
                                  double value)
{
    uint64_t data;

    memcpy(&data, &value, sizeof(data));
    pbtools_encoder_write_fixed64(self_p, field_number, data);
}

void pbtools_encoder_write_bool(struct pbtools_encoder_t *self_p,
                                int field_number,
                                bool value)
{
    if (value) {
        pbtools_encoder_write_int32(self_p, field_number, 1);
    }
}

void pbtools_encoder_write_enum(struct pbtools_encoder_t *self_p,
                                int field_number,
                                int value)
{
    pbtools_encoder_write_int32(self_p, field_number, value);
}

void pbtools_encoder_write_string(struct pbtools_encoder_t *self_p,
                                  int field_number,
                                  char *value_p)
{
    size_t length;

    length = strlen(value_p);

    if (length > 0) {
        pbtools_encoder_write(self_p, (uint8_t *)value_p, length);
        pbtools_encoder_write_tagged_varint(self_p,
                                            field_number,
                                            PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED,
                                            length);
    }
}

void pbtools_encoder_write_bytes(struct pbtools_encoder_t *self_p,
                                 int field_number,
                                 struct pbtools_bytes_t *value_p)
{
    if (value_p->size > 0) {
        pbtools_encoder_write(self_p, value_p->buf_p, value_p->size);
        pbtools_encoder_write_tagged_varint(self_p,
                                            field_number,
                                            PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED,
                                            value_p->size);
    }
}

static void write_repeated_scalar_value_type(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_scalar_value_type_t *repeated_p,
    pbtools_repeated_scalar_value_type_write_t member_write)
{
    int i;
    int pos;

    if (repeated_p->length == 0) {
        return;
    }

    pos = self_p->pos;

    for (i = repeated_p->length - 1; i >= 0; i--) {
        member_write(self_p, repeated_p->items_pp[i]);
    }

    pbtools_encoder_write_tagged_varint(self_p,
                                        field_number,
                                        PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED,
                                        pos - self_p->pos);
}

static void write_repeated_int32_item(
    struct pbtools_encoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    pbtools_encoder_write_varint(self_p,
                                 ((struct pbtools_int32_t *)item_p)->value);
}

void pbtools_encoder_write_repeated_int32(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_int32_t *repeated_p)
{
    WRITE_REPEATED_SCALAR_VALUE_TYPE(int32);
}

static void write_repeated_int64_item(
    struct pbtools_encoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    pbtools_encoder_write_varint(self_p,
                                 ((struct pbtools_int64_t *)item_p)->value);
}

void pbtools_encoder_write_repeated_int64(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_int64_t *repeated_p)
{
    WRITE_REPEATED_SCALAR_VALUE_TYPE(int64);
}

static void write_repeated_sint32_item(
    struct pbtools_encoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    int32_t value;
    uint32_t data;

    value = ((struct pbtools_int32_t *)item_p)->value;

    if (value < 0) {
        data = ~((uint32_t)value << 1);
    } else {
        data = ((uint32_t)value << 1);
    }

    pbtools_encoder_write_varint(self_p, data);
}

void pbtools_encoder_write_repeated_sint32(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_int32_t *repeated_p)
{
    WRITE_REPEATED_SCALAR_VALUE_TYPE(sint32);
}

static void write_repeated_sint64_item(
    struct pbtools_encoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    int64_t value;
    uint64_t data;

    value = ((struct pbtools_int64_t *)item_p)->value;

    if (value < 0) {
        data = ~((uint64_t)value << 1);
    } else {
        data = ((uint64_t)value << 1);
    }

    pbtools_encoder_write_varint(self_p, data);
}

void pbtools_encoder_write_repeated_sint64(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_int64_t *repeated_p)
{
    WRITE_REPEATED_SCALAR_VALUE_TYPE(sint64);
}

static void write_repeated_uint32_item(
    struct pbtools_encoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    pbtools_encoder_write_varint(self_p,
                                 ((struct pbtools_uint32_t *)item_p)->value);
}

void pbtools_encoder_write_repeated_uint32(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_uint32_t *repeated_p)
{
    WRITE_REPEATED_SCALAR_VALUE_TYPE(uint32);
}

static void write_repeated_uint64_item(
    struct pbtools_encoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    pbtools_encoder_write_varint(self_p,
                                 ((struct pbtools_uint64_t *)item_p)->value);
}

void pbtools_encoder_write_repeated_uint64(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_uint64_t *repeated_p)
{
    WRITE_REPEATED_SCALAR_VALUE_TYPE(uint64);
}

static void write_repeated_fixed32_item(
    struct pbtools_encoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    pbtools_encoder_write_fixed32_value(self_p,
                                        ((struct pbtools_uint32_t *)item_p)->value);
}

void pbtools_encoder_write_repeated_fixed32(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_uint32_t *repeated_p)
{
    WRITE_REPEATED_SCALAR_VALUE_TYPE(fixed32);
}

static void write_repeated_fixed64_item(
    struct pbtools_encoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    pbtools_encoder_write_fixed64_value(self_p,
                                        ((struct pbtools_uint64_t *)item_p)->value);
}

void pbtools_encoder_write_repeated_fixed64(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_uint64_t *repeated_p)
{
    WRITE_REPEATED_SCALAR_VALUE_TYPE(fixed64);
}

static void write_repeated_sfixed32_item(
    struct pbtools_encoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    pbtools_encoder_write_fixed32_value(self_p,
                                        ((struct pbtools_int32_t *)item_p)->value);
}

void pbtools_encoder_write_repeated_sfixed32(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_int32_t *repeated_p)
{
    WRITE_REPEATED_SCALAR_VALUE_TYPE(sfixed32);
}

static void write_repeated_sfixed64_item(
    struct pbtools_encoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    pbtools_encoder_write_fixed64_value(self_p,
                                        ((struct pbtools_int64_t *)item_p)->value);
}

void pbtools_encoder_write_repeated_sfixed64(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_int64_t *repeated_p)
{
    WRITE_REPEATED_SCALAR_VALUE_TYPE(sfixed64);
}

static void write_repeated_float_item(
    struct pbtools_encoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    uint32_t data;

    memcpy(&data, &(((struct pbtools_float_t *)item_p)->value), sizeof(data));
    pbtools_encoder_write_fixed32_value(self_p, data);
}

void pbtools_encoder_write_repeated_float(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_float_t *repeated_p)
{
    WRITE_REPEATED_SCALAR_VALUE_TYPE(float);
}

static void write_repeated_double_item(
    struct pbtools_encoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    uint64_t data;

    memcpy(&data, &(((struct pbtools_double_t *)item_p)->value), sizeof(data));
    pbtools_encoder_write_fixed64_value(self_p, data);
}

void pbtools_encoder_write_repeated_double(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_double_t *repeated_p)
{
    WRITE_REPEATED_SCALAR_VALUE_TYPE(double);
}

static void write_repeated_bool_item(
    struct pbtools_encoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    if (((struct pbtools_bool_t *)item_p)->value) {
        pbtools_encoder_write_varint(self_p, 1);
    } else {
        pbtools_encoder_write_varint(self_p, 0);
    }
}

void pbtools_encoder_write_repeated_bool(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_bool_t *repeated_p)
{
    WRITE_REPEATED_SCALAR_VALUE_TYPE(bool);
}

void pbtools_encoder_write_repeated_string(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_string_t *repeated_p)
{
    int i;

    if (repeated_p->length == 0) {
        return;
    }

    for (i = repeated_p->length - 1; i >= 0; i--) {
        pbtools_encoder_write_string(self_p,
                                     field_number,
                                     repeated_p->items_pp[i]->value_p);
    }
}

void pbtools_encoder_write_repeated_bytes(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_bytes_t *repeated_p)
{
    int i;

    if (repeated_p->length == 0) {
        return;
    }

    for (i = repeated_p->length - 1; i >= 0; i--) {
        pbtools_encoder_write_bytes(self_p,
                                    field_number,
                                    repeated_p->items_pp[i]);
    }
}

void pbtools_decoder_init(struct pbtools_decoder_t *self_p,
                          const uint8_t *buf_p,
                          size_t size,
                          struct pbtools_heap_t *heap_p)
{
    self_p->buf_p = buf_p;
    self_p->size = size;
    self_p->pos = 0;
    self_p->heap_p = heap_p;
}

int pbtools_decoder_get_result(struct pbtools_decoder_t *self_p)
{
    return (self_p->pos);
}

void pbtools_decoder_abort(struct pbtools_decoder_t *self_p,
                           int error)
{
    if (self_p->pos >= 0) {
        self_p->size = -error;
        self_p->pos = -error;
    }
}

bool pbtools_decoder_available(struct pbtools_decoder_t *self_p)
{
    return (self_p->pos < self_p->size);
}

uint8_t pbtools_decoder_get(struct pbtools_decoder_t *self_p)
{
    uint8_t value;

    if (pbtools_decoder_available(self_p)) {
        value = self_p->buf_p[self_p->pos];
        PRINTF("pbtools_decoder_get(): 0x%02x\n", value);
        self_p->pos++;
    } else {
        PRINTF("pbtools_decoder_get(): failed\n");
        pbtools_decoder_abort(self_p, PBTOOLS_OUT_OF_DATA);
        value = 0;
    }

    return (value);
}

void pbtools_decoder_read(struct pbtools_decoder_t *self_p,
                          uint8_t *buf_p,
                          size_t size)
{
    if ((self_p->size - self_p->pos) >= size) {
        memcpy(buf_p, &self_p->buf_p[self_p->pos], size);
        self_p->pos += size;
    } else {
        memset(buf_p, 0, size);
        pbtools_decoder_abort(self_p, PBTOOLS_OUT_OF_DATA);
    }
}

uint64_t pbtools_decoder_read_varint(struct pbtools_decoder_t *self_p)
{
    uint64_t value;
    uint8_t byte;
    int offset;

    value = 0;
    offset = 0;

    do {
        byte = pbtools_decoder_get(self_p);
        value |= (((uint64_t)byte & 0x7f) << offset);
        offset += 7;
    } while ((offset < 64) && (byte & 0x80));

    if (byte & 0x80) {
        PRINTF("overflow\n");
        pbtools_decoder_abort(self_p, PBTOOLS_VARINT_OVERFLOW);
        value = 0;
    }

    PRINTF("pbtools_decoder_read_varint(): value: 0x%llx\n",
           (unsigned long long)value);

    return (value);
}

uint64_t pbtools_decoder_read_varint_check_wire_type(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    int expected_wire_type)
{
    if (wire_type != expected_wire_type) {
        pbtools_decoder_abort(self_p, PBTOOLS_BAD_WIRE_TYPE);

        return (0);
    }

    return (pbtools_decoder_read_varint(self_p));
}

uint64_t pbtools_decoder_read_varint_check_wire_type_varint(
    struct pbtools_decoder_t *self_p,
    int wire_type)
{
    return (pbtools_decoder_read_varint_check_wire_type(
                self_p,
                wire_type,
                PBTOOLS_WIRE_TYPE_VARINT));
}

int pbtools_decoder_read_tag(struct pbtools_decoder_t *self_p,
                             int *wire_type_p)
{
    uint32_t value;
    int field_number;

    value = pbtools_decoder_read_varint(self_p);
    field_number = (value >> 3);
    *wire_type_p = (value & 0x7);

    if (field_number == 0) {
        pbtools_decoder_abort(self_p, PBTOOLS_BAD_FIELD_NUMBER);
    }

    PRINTF("pbtools_decoder_read_tag(): %d %d\n",
           field_number,
           *wire_type_p);

    return (field_number);
}

int32_t pbtools_decoder_read_int32(struct pbtools_decoder_t *self_p,
                                   int wire_type)
{
    return (pbtools_decoder_read_int64(self_p, wire_type));
}

int64_t pbtools_decoder_read_int64(struct pbtools_decoder_t *self_p,
                                   int wire_type)
{
    return (pbtools_decoder_read_varint_check_wire_type_varint(self_p,
                                                               wire_type));
}

int32_t pbtools_decoder_read_sint32(struct pbtools_decoder_t *self_p,
                                    int wire_type)
{
    return (pbtools_decoder_read_sint64(self_p, wire_type));
}

int64_t pbtools_decoder_read_sint64(struct pbtools_decoder_t *self_p,
                                    int wire_type)
{
    uint64_t value;

    value = pbtools_decoder_read_varint_check_wire_type_varint(self_p,
                                                               wire_type);

    if (value & 0x1) {
        value >>= 1;
        value = ~value;
    } else {
        value >>= 1;
    }

    return (value);
}

uint32_t pbtools_decoder_read_uint32(struct pbtools_decoder_t *self_p,
                                     int wire_type)
{
    return (pbtools_decoder_read_uint64(self_p, wire_type));
}

uint64_t pbtools_decoder_read_uint64(struct pbtools_decoder_t *self_p,
                                     int wire_type)
{
    return (pbtools_decoder_read_varint_check_wire_type_varint(self_p,
                                                               wire_type));
}

uint32_t pbtools_decoder_read_fixed32_value(struct pbtools_decoder_t *self_p)
{
    uint32_t value;
    uint8_t buf[4];

    pbtools_decoder_read(self_p, &buf[0], sizeof(buf));
    value = ((uint32_t)buf[0] << 0);
    value |= ((uint32_t)buf[1] << 8);
    value |= ((uint32_t)buf[2] << 16);
    value |= ((uint32_t)buf[3] << 24);

    return (value);
}

uint32_t pbtools_decoder_read_fixed32(struct pbtools_decoder_t *self_p,
                                      int wire_type)
{
    uint32_t value;
    uint8_t buf[4];

    if (wire_type != PBTOOLS_WIRE_TYPE_FIXED_32) {
        pbtools_decoder_abort(self_p, PBTOOLS_BAD_WIRE_TYPE);

        return (0);
    }

    pbtools_decoder_read(self_p, &buf[0], sizeof(buf));
    value = ((uint32_t)buf[0] << 0);
    value |= ((uint32_t)buf[1] << 8);
    value |= ((uint32_t)buf[2] << 16);
    value |= ((uint32_t)buf[3] << 24);

    return (value);
}

uint64_t pbtools_decoder_read_fixed64_value(struct pbtools_decoder_t *self_p)
{
    uint64_t value;
    uint8_t buf[8];

    pbtools_decoder_read(self_p, &buf[0], sizeof(buf));
    value = ((uint64_t)buf[0] << 0);
    value |= ((uint64_t)buf[1] << 8);
    value |= ((uint64_t)buf[2] << 16);
    value |= ((uint64_t)buf[3] << 24);
    value |= ((uint64_t)buf[4] << 32);
    value |= ((uint64_t)buf[5] << 40);
    value |= ((uint64_t)buf[6] << 48);
    value |= ((uint64_t)buf[7] << 56);

    return (value);
}

uint64_t pbtools_decoder_read_fixed64(struct pbtools_decoder_t *self_p,
                                      int wire_type)
{
    uint64_t value;
    uint8_t buf[8];

    if (wire_type != PBTOOLS_WIRE_TYPE_FIXED_64) {
        pbtools_decoder_abort(self_p, PBTOOLS_BAD_WIRE_TYPE);

        return (0);
    }

    pbtools_decoder_read(self_p, &buf[0], sizeof(buf));
    value = ((uint64_t)buf[0] << 0);
    value |= ((uint64_t)buf[1] << 8);
    value |= ((uint64_t)buf[2] << 16);
    value |= ((uint64_t)buf[3] << 24);
    value |= ((uint64_t)buf[4] << 32);
    value |= ((uint64_t)buf[5] << 40);
    value |= ((uint64_t)buf[6] << 48);
    value |= ((uint64_t)buf[7] << 56);

    return (value);
}

int32_t pbtools_decoder_read_sfixed32(struct pbtools_decoder_t *self_p,
                                      int wire_type)
{
    return (pbtools_decoder_read_fixed32(self_p, wire_type));
}

int64_t pbtools_decoder_read_sfixed64(struct pbtools_decoder_t *self_p,
                                      int wire_type)
{
    return (pbtools_decoder_read_fixed64(self_p, wire_type));
}

float pbtools_decoder_read_float(struct pbtools_decoder_t *self_p,
                                 int wire_type)
{
    uint32_t data;
    float value;

    data = pbtools_decoder_read_fixed32(self_p, wire_type);
    memcpy(&value, &data, sizeof(value));

    return (value);
}

double pbtools_decoder_read_double(struct pbtools_decoder_t *self_p,
                                   int wire_type)
{
    uint64_t data;
    double value;

    data = pbtools_decoder_read_fixed64(self_p, wire_type);
    memcpy(&value, &data, sizeof(value));

    return (value);
}

bool pbtools_decoder_read_bool(struct pbtools_decoder_t *self_p,
                               int wire_type)
{
    return (pbtools_decoder_read_int32(self_p, wire_type) != 0);
}

int pbtools_decoder_read_enum(struct pbtools_decoder_t *self_p,
                              int wire_type)
{
    return (pbtools_decoder_read_int32(self_p, wire_type));
}

void pbtools_decoder_read_string(struct pbtools_decoder_t *self_p,
                                 int wire_type,
                                 char **value_pp)
{
    uint64_t size;

    if (wire_type != PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED) {
        pbtools_decoder_abort(self_p, PBTOOLS_BAD_WIRE_TYPE);

        return;
    }

    size = pbtools_decoder_read_varint(self_p);

    if (size >= INT_MAX) {
        pbtools_decoder_abort(self_p, PBTOOLS_STRING_TOO_LONG);

        return;
    }

    *value_pp = pbtools_decoder_heap_alloc(self_p, size + 1);

    if (*value_pp == NULL) {
        return;
    }

    pbtools_decoder_read(self_p, (uint8_t *)*value_pp, size);
    (*value_pp)[size] = '\0';
}

void pbtools_decoder_read_bytes(struct pbtools_decoder_t *self_p,
                                int wire_type,
                                struct pbtools_bytes_t *bytes_p)
{
    uint64_t size;

    if (wire_type != PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED) {
        pbtools_decoder_abort(self_p, PBTOOLS_BAD_WIRE_TYPE);

        return;
    }

    size = pbtools_decoder_read_varint(self_p);

    if (size >= INT_MAX) {
        pbtools_decoder_abort(self_p, PBTOOLS_BYTES_TOO_LONG);

        return;
    }

    bytes_p->size = size;
    bytes_p->buf_p = pbtools_decoder_heap_alloc(self_p, bytes_p->size);

    if (bytes_p->buf_p == NULL) {
        return;
    }

    pbtools_decoder_read(self_p, bytes_p->buf_p, bytes_p->size);
}

static void *alloc_repeated(void ***items_ppp,
                            struct pbtools_heap_t *heap_p,
                            int length,
                            size_t item_size)
{
    void *items_p;

    items_p = NULL;
    *items_ppp = pbtools_heap_alloc(heap_p, sizeof(items_p) * length);

    if (*items_ppp != NULL) {
        items_p = pbtools_heap_alloc(heap_p, item_size * length);
    }

    return (items_p);
}

int pbtools_alloc_repeated_int32(struct pbtools_message_base_t *self_p,
                                 int length,
                                 struct pbtools_repeated_int32_t *repeated_p)
{
    int i;
    struct pbtools_int32_t *items_p;

    items_p = alloc_repeated((void ***)&repeated_p->items_pp,
                             self_p->heap_p,
                             length,
                             sizeof(*items_p));

    if (items_p == NULL) {
        return (-PBTOOLS_OUT_OF_MEMORY);
    }

    for (i = 0; i < length; i++) {
        items_p[i].value = 0;
        items_p[i].next_p = &items_p[i + 1];
        repeated_p->items_pp[i] = &items_p[i];
    }

    items_p[length - 1].next_p = NULL;
    repeated_p->length = length;
    repeated_p->head_p = &items_p[0];
    repeated_p->tail_p = &items_p[length - 1];

    return (0);
}

static void read_repeated_scalar_value_type(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_scalar_value_type_t *repeated_p,
    size_t item_size,
    pbtools_repeated_scalar_value_type_read_t member_read)
{
    size_t size;
    int pos;
    struct pbtools_scalar_value_type_base_t *item_p;

    size = pbtools_decoder_read_varint_check_wire_type(
        self_p,
        wire_type,
        PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED);
    pos = self_p->pos;

    while (self_p->pos < (pos + size)) {
        item_p = pbtools_decoder_heap_alloc(self_p, item_size);

        if (item_p == NULL) {
            return;
        }

        member_read(self_p, item_p);
        item_p->next_p = NULL;

        if (repeated_p->length == 0) {
            repeated_p->head_p = item_p;
        } else {
            repeated_p->tail_p->next_p = item_p;
        }

        repeated_p->tail_p = item_p;
        repeated_p->length++;
    }
}

static void read_repeated_int32_item(
    struct pbtools_decoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    ((struct pbtools_int32_t *)item_p)->value = pbtools_decoder_read_varint(self_p);
}

void pbtools_decoder_read_repeated_int32(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_int32_t *repeated_p)
{
    READ_REPEATED_SCALAR_VALUE_TYPE(int32, int32);
}

void pbtools_decoder_finalize_repeated_int32(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_int32_t *repeated_p)
{
    struct pbtools_int32_t *item_p;
    int i;

    if (repeated_p->length > 0) {
        repeated_p->items_pp = pbtools_decoder_heap_alloc(
            self_p,
            sizeof(item_p) * repeated_p->length);

        if (repeated_p->items_pp == NULL) {
            return;
        }

        item_p = repeated_p->head_p;

        for (i = 0; i < repeated_p->length; i++) {
            repeated_p->items_pp[i] = item_p;
            item_p = item_p->next_p;
        }
    }
}

int pbtools_alloc_repeated_int64(struct pbtools_message_base_t *self_p,
                                 int length,
                                 struct pbtools_repeated_int64_t *repeated_p)
{
    int i;
    struct pbtools_int64_t *items_p;

    items_p = alloc_repeated((void ***)&repeated_p->items_pp,
                             self_p->heap_p,
                             length,
                             sizeof(*items_p));

    if (items_p == NULL) {
        return (-PBTOOLS_OUT_OF_MEMORY);
    }

    for (i = 0; i < length; i++) {
        items_p[i].value = 0;
        items_p[i].next_p = &items_p[i + 1];
        repeated_p->items_pp[i] = &items_p[i];
    }

    items_p[length - 1].next_p = NULL;
    repeated_p->length = length;
    repeated_p->head_p = &items_p[0];
    repeated_p->tail_p = &items_p[length - 1];

    return (0);
}

static void read_repeated_int64_item(
    struct pbtools_decoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    ((struct pbtools_int64_t *)item_p)->value = pbtools_decoder_read_varint(self_p);
}

void pbtools_decoder_read_repeated_int64(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_int64_t *repeated_p)
{
    READ_REPEATED_SCALAR_VALUE_TYPE(int64, int64);
}

void pbtools_decoder_finalize_repeated_int64(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_int64_t *repeated_p)
{
    struct pbtools_int64_t *item_p;
    int i;

    if (repeated_p->length > 0) {
        repeated_p->items_pp = pbtools_decoder_heap_alloc(
            self_p,
            sizeof(item_p) * repeated_p->length);

        if (repeated_p->items_pp == NULL) {
            return;
        }

        item_p = repeated_p->head_p;

        for (i = 0; i < repeated_p->length; i++) {
            repeated_p->items_pp[i] = item_p;
            item_p = item_p->next_p;
        }
    }
}

int pbtools_alloc_repeated_uint32(struct pbtools_message_base_t *self_p,
                                  int length,
                                  struct pbtools_repeated_uint32_t *repeated_p)
{
    int i;
    struct pbtools_uint32_t *items_p;

    items_p = alloc_repeated((void ***)&repeated_p->items_pp,
                             self_p->heap_p,
                             length,
                             sizeof(*items_p));

    if (items_p == NULL) {
        return (-PBTOOLS_OUT_OF_MEMORY);
    }

    for (i = 0; i < length; i++) {
        items_p[i].value = 0;
        items_p[i].next_p = &items_p[i + 1];
        repeated_p->items_pp[i] = &items_p[i];
    }

    items_p[length - 1].next_p = NULL;
    repeated_p->length = length;
    repeated_p->head_p = &items_p[0];
    repeated_p->tail_p = &items_p[length - 1];

    return (0);
}

static void read_repeated_uint32_item(
    struct pbtools_decoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    ((struct pbtools_uint32_t *)item_p)->value = pbtools_decoder_read_varint(self_p);
}

void pbtools_decoder_read_repeated_uint32(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_uint32_t *repeated_p)
{
    READ_REPEATED_SCALAR_VALUE_TYPE(uint32, uint32);
}

void pbtools_decoder_finalize_repeated_uint32(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_uint32_t *repeated_p)
{
    struct pbtools_uint32_t *item_p;
    int i;

    if (repeated_p->length > 0) {
        repeated_p->items_pp = pbtools_decoder_heap_alloc(
            self_p,
            sizeof(item_p) * repeated_p->length);

        if (repeated_p->items_pp == NULL) {
            return;
        }

        item_p = repeated_p->head_p;

        for (i = 0; i < repeated_p->length; i++) {
            repeated_p->items_pp[i] = item_p;
            item_p = item_p->next_p;
        }
    }
}

int pbtools_alloc_repeated_uint64(struct pbtools_message_base_t *self_p,
                                  int length,
                                  struct pbtools_repeated_uint64_t *repeated_p)
{
    int i;
    struct pbtools_uint64_t *items_p;

    items_p = alloc_repeated((void ***)&repeated_p->items_pp,
                             self_p->heap_p,
                             length,
                             sizeof(*items_p));

    if (items_p == NULL) {
        return (-PBTOOLS_OUT_OF_MEMORY);
    }

    for (i = 0; i < length; i++) {
        items_p[i].value = 0;
        items_p[i].next_p = &items_p[i + 1];
        repeated_p->items_pp[i] = &items_p[i];
    }

    items_p[length - 1].next_p = NULL;
    repeated_p->length = length;
    repeated_p->head_p = &items_p[0];
    repeated_p->tail_p = &items_p[length - 1];

    return (0);
}

static void read_repeated_uint64_item(
    struct pbtools_decoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    ((struct pbtools_uint64_t *)item_p)->value = pbtools_decoder_read_varint(self_p);
}

void pbtools_decoder_read_repeated_uint64(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_uint64_t *repeated_p)
{
    READ_REPEATED_SCALAR_VALUE_TYPE(uint64, uint64);
}

void pbtools_decoder_finalize_repeated_uint64(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_uint64_t *repeated_p)
{
    struct pbtools_uint64_t *item_p;
    int i;

    if (repeated_p->length > 0) {
        repeated_p->items_pp = pbtools_decoder_heap_alloc(
            self_p,
            sizeof(item_p) * repeated_p->length);

        if (repeated_p->items_pp == NULL) {
            return;
        }

        item_p = repeated_p->head_p;

        for (i = 0; i < repeated_p->length; i++) {
            repeated_p->items_pp[i] = item_p;
            item_p = item_p->next_p;
        }
    }
}

int pbtools_alloc_repeated_sint32(struct pbtools_message_base_t *self_p,
                                  int length,
                                  struct pbtools_repeated_int32_t *repeated_p)
{
    return (pbtools_alloc_repeated_int32(self_p, length, repeated_p));
}

static void read_repeated_sint32_item(
    struct pbtools_decoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    uint64_t value;

    value = pbtools_decoder_read_varint(self_p);

    if (value & 0x1) {
        value >>= 1;
        value = ~value;
    } else {
        value >>= 1;
    }

    ((struct pbtools_int32_t *)item_p)->value = value;
}

void pbtools_decoder_read_repeated_sint32(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_int32_t *repeated_p)
{
    READ_REPEATED_SCALAR_VALUE_TYPE(sint32, int32);
}

void pbtools_decoder_finalize_repeated_sint32(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_int32_t *repeated_p)
{
    pbtools_decoder_finalize_repeated_int32(self_p, repeated_p);
}

int pbtools_alloc_repeated_sint64(struct pbtools_message_base_t *self_p,
                                  int length,
                                  struct pbtools_repeated_int64_t *repeated_p)
{
    return (pbtools_alloc_repeated_int64(self_p, length, repeated_p));
}

static void read_repeated_sint64_item(
    struct pbtools_decoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    uint64_t value;

    value = pbtools_decoder_read_varint(self_p);

    if (value & 0x1) {
        value >>= 1;
        value = ~value;
    } else {
        value >>= 1;
    }

    ((struct pbtools_int64_t *)item_p)->value = value;
}

void pbtools_decoder_read_repeated_sint64(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_int64_t *repeated_p)
{
    READ_REPEATED_SCALAR_VALUE_TYPE(sint64, int64);
}

void pbtools_decoder_finalize_repeated_sint64(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_int64_t *repeated_p)
{
    pbtools_decoder_finalize_repeated_int64(self_p, repeated_p);
}

int pbtools_alloc_repeated_fixed32(struct pbtools_message_base_t *self_p,
                                   int length,
                                   struct pbtools_repeated_uint32_t *repeated_p)
{
    return (pbtools_alloc_repeated_uint32(self_p, length, repeated_p));
}

static void read_repeated_fixed32_item(
    struct pbtools_decoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    ((struct pbtools_uint32_t *)item_p)->value =
        pbtools_decoder_read_fixed32_value(self_p);
}

void pbtools_decoder_read_repeated_fixed32(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_uint32_t *repeated_p)
{
    READ_REPEATED_SCALAR_VALUE_TYPE(fixed32, uint32);
}

void pbtools_decoder_finalize_repeated_fixed32(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_uint32_t *repeated_p)
{
    pbtools_decoder_finalize_repeated_uint32(self_p, repeated_p);
}

int pbtools_alloc_repeated_fixed64(struct pbtools_message_base_t *self_p,
                                   int length,
                                   struct pbtools_repeated_uint64_t *repeated_p)
{
    return (pbtools_alloc_repeated_uint64(self_p, length, repeated_p));
}

static void read_repeated_fixed64_item(
    struct pbtools_decoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    ((struct pbtools_uint64_t *)item_p)->value =
        pbtools_decoder_read_fixed64_value(self_p);
}

void pbtools_decoder_read_repeated_fixed64(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_uint64_t *repeated_p)
{
    READ_REPEATED_SCALAR_VALUE_TYPE(fixed64, uint64);
}

void pbtools_decoder_finalize_repeated_fixed64(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_uint64_t *repeated_p)
{
    pbtools_decoder_finalize_repeated_uint64(self_p, repeated_p);
}

int pbtools_alloc_repeated_sfixed32(struct pbtools_message_base_t *self_p,
                                    int length,
                                    struct pbtools_repeated_int32_t *repeated_p)
{
    return (pbtools_alloc_repeated_int32(self_p, length, repeated_p));
}

static void read_repeated_sfixed32_item(
    struct pbtools_decoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    ((struct pbtools_int32_t *)item_p)->value =
        pbtools_decoder_read_fixed32_value(self_p);
}

void pbtools_decoder_read_repeated_sfixed32(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_int32_t *repeated_p)
{
    READ_REPEATED_SCALAR_VALUE_TYPE(sfixed32, int32);
}

void pbtools_decoder_finalize_repeated_sfixed32(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_int32_t *repeated_p)
{
    pbtools_decoder_finalize_repeated_int32(self_p, repeated_p);
}

int pbtools_alloc_repeated_sfixed64(struct pbtools_message_base_t *self_p,
                                    int length,
                                    struct pbtools_repeated_int64_t *repeated_p)
{
    return (pbtools_alloc_repeated_int64(self_p, length, repeated_p));
}

static void read_repeated_sfixed64_item(
    struct pbtools_decoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    ((struct pbtools_int64_t *)item_p)->value =
        pbtools_decoder_read_fixed64_value(self_p);
}

void pbtools_decoder_read_repeated_sfixed64(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_int64_t *repeated_p)
{
    READ_REPEATED_SCALAR_VALUE_TYPE(sfixed64, int64);
}

void pbtools_decoder_finalize_repeated_sfixed64(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_int64_t *repeated_p)
{
    pbtools_decoder_finalize_repeated_int64(self_p, repeated_p);
}

int pbtools_alloc_repeated_float(struct pbtools_message_base_t *self_p,
                                 int length,
                                 struct pbtools_repeated_float_t *repeated_p)
{
    int i;
    struct pbtools_float_t *items_p;

    items_p = alloc_repeated((void ***)&repeated_p->items_pp,
                             self_p->heap_p,
                             length,
                             sizeof(*items_p));

    if (items_p == NULL) {
        return (-PBTOOLS_OUT_OF_MEMORY);
    }

    for (i = 0; i < length; i++) {
        items_p[i].value = 0;
        items_p[i].next_p = &items_p[i + 1];
        repeated_p->items_pp[i] = &items_p[i];
    }

    items_p[length - 1].next_p = NULL;
    repeated_p->length = length;
    repeated_p->head_p = &items_p[0];
    repeated_p->tail_p = &items_p[length - 1];

    return (0);
}

static void read_repeated_float_item(
    struct pbtools_decoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    uint32_t data;

    data = pbtools_decoder_read_fixed32_value(self_p);
    memcpy(&(((struct pbtools_float_t *)item_p)->value), &data, sizeof(data));
}

void pbtools_decoder_read_repeated_float(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_float_t *repeated_p)
{
    READ_REPEATED_SCALAR_VALUE_TYPE(float, float);
}

void pbtools_decoder_finalize_repeated_float(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_float_t *repeated_p)
{
    struct pbtools_float_t *item_p;
    int i;

    if (repeated_p->length > 0) {
        repeated_p->items_pp = pbtools_decoder_heap_alloc(
            self_p,
            sizeof(item_p) * repeated_p->length);

        if (repeated_p->items_pp == NULL) {
            return;
        }

        item_p = repeated_p->head_p;

        for (i = 0; i < repeated_p->length; i++) {
            repeated_p->items_pp[i] = item_p;
            item_p = item_p->next_p;
        }
    }
}

int pbtools_alloc_repeated_double(struct pbtools_message_base_t *self_p,
                                 int length,
                                 struct pbtools_repeated_double_t *repeated_p)
{
    int i;
    struct pbtools_double_t *items_p;

    items_p = alloc_repeated((void ***)&repeated_p->items_pp,
                             self_p->heap_p,
                             length,
                             sizeof(*items_p));

    if (items_p == NULL) {
        return (-PBTOOLS_OUT_OF_MEMORY);
    }

    for (i = 0; i < length; i++) {
        items_p[i].value = 0;
        items_p[i].next_p = &items_p[i + 1];
        repeated_p->items_pp[i] = &items_p[i];
    }

    items_p[length - 1].next_p = NULL;
    repeated_p->length = length;
    repeated_p->head_p = &items_p[0];
    repeated_p->tail_p = &items_p[length - 1];

    return (0);
}

static void read_repeated_double_item(
    struct pbtools_decoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    uint64_t data;

    data = pbtools_decoder_read_fixed64_value(self_p);
    memcpy(&(((struct pbtools_double_t *)item_p)->value), &data, sizeof(data));
}

void pbtools_decoder_read_repeated_double(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_double_t *repeated_p)
{
    READ_REPEATED_SCALAR_VALUE_TYPE(double, double);
}

void pbtools_decoder_finalize_repeated_double(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_double_t *repeated_p)
{
    struct pbtools_double_t *item_p;
    int i;

    if (repeated_p->length > 0) {
        repeated_p->items_pp = pbtools_decoder_heap_alloc(
            self_p,
            sizeof(item_p) * repeated_p->length);

        if (repeated_p->items_pp == NULL) {
            return;
        }

        item_p = repeated_p->head_p;

        for (i = 0; i < repeated_p->length; i++) {
            repeated_p->items_pp[i] = item_p;
            item_p = item_p->next_p;
        }
    }
}

int pbtools_alloc_repeated_bool(struct pbtools_message_base_t *self_p,
                                int length,
                                struct pbtools_repeated_bool_t *repeated_p)
{
    int i;
    struct pbtools_bool_t *items_p;

    items_p = alloc_repeated((void ***)&repeated_p->items_pp,
                             self_p->heap_p,
                             length,
                             sizeof(*items_p));

    if (items_p == NULL) {
        return (-PBTOOLS_OUT_OF_MEMORY);
    }

    for (i = 0; i < length; i++) {
        items_p[i].value = 0;
        items_p[i].next_p = &items_p[i + 1];
        repeated_p->items_pp[i] = &items_p[i];
    }

    items_p[length - 1].next_p = NULL;
    repeated_p->length = length;
    repeated_p->head_p = &items_p[0];
    repeated_p->tail_p = &items_p[length - 1];

    return (0);
}

void pbtools_decoder_read_repeated_bool(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_bool_t *repeated_p)
{
    size_t size;
    int pos;
    struct pbtools_bool_t *item_p;

    size = pbtools_decoder_read_varint_check_wire_type(
        self_p,
        wire_type,
        PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED);
    pos = self_p->pos;

    while (self_p->pos < (pos + size)) {
        item_p = pbtools_decoder_heap_alloc(self_p, sizeof(*item_p));

        if (item_p == NULL) {
            return;
        }

        item_p->value = (pbtools_decoder_read_varint(self_p) != 0);
        item_p->next_p = NULL;

        if (repeated_p->length == 0) {
            repeated_p->head_p = item_p;
        } else {
            repeated_p->tail_p->next_p = item_p;
        }

        repeated_p->tail_p = item_p;
        repeated_p->length++;
    }
}

void pbtools_decoder_finalize_repeated_bool(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_bool_t *repeated_p)
{
    struct pbtools_bool_t *item_p;
    int i;

    if (repeated_p->length > 0) {
        repeated_p->items_pp = pbtools_decoder_heap_alloc(
            self_p,
            sizeof(item_p) * repeated_p->length);

        if (repeated_p->items_pp == NULL) {
            return;
        }

        item_p = repeated_p->head_p;

        for (i = 0; i < repeated_p->length; i++) {
            repeated_p->items_pp[i] = item_p;
            item_p = item_p->next_p;
        }
    }
}

int pbtools_alloc_repeated_string(struct pbtools_message_base_t *self_p,
                                  int length,
                                  struct pbtools_repeated_string_t *repeated_p)
{
    int i;
    struct pbtools_string_t *items_p;

    items_p = alloc_repeated((void ***)&repeated_p->items_pp,
                             self_p->heap_p,
                             length,
                             sizeof(*items_p));

    if (items_p == NULL) {
        return (-PBTOOLS_OUT_OF_MEMORY);
    }

    for (i = 0; i < length; i++) {
        items_p[i].value_p = "";
        items_p[i].next_p = &items_p[i + 1];
        repeated_p->items_pp[i] = &items_p[i];
    }

    items_p[length - 1].next_p = NULL;
    repeated_p->length = length;
    repeated_p->head_p = &items_p[0];
    repeated_p->tail_p = &items_p[length - 1];

    return (0);
}

void pbtools_decoder_read_repeated_string(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_string_t *repeated_p)
{
    struct pbtools_string_t *item_p;

    item_p = pbtools_decoder_heap_alloc(self_p, sizeof(*item_p));

    if (item_p == NULL) {
        return;
    }

    pbtools_decoder_read_string(self_p, wire_type, &item_p->value_p);

    if (repeated_p->length == 0) {
        repeated_p->head_p = item_p;
    } else {
        repeated_p->tail_p->next_p = item_p;
    }

    repeated_p->tail_p = item_p;
    repeated_p->length++;
}

void pbtools_decoder_finalize_repeated_string(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_string_t *repeated_p)
{
    struct pbtools_string_t *item_p;
    int i;

    if (repeated_p->length > 0) {
        repeated_p->items_pp = pbtools_decoder_heap_alloc(
            self_p,
            sizeof(item_p) * repeated_p->length);

        if (repeated_p->items_pp == NULL) {
            return;
        }

        item_p = repeated_p->head_p;

        for (i = 0; i < repeated_p->length; i++) {
            repeated_p->items_pp[i] = item_p;
            item_p = item_p->next_p;
        }
    }
}

int pbtools_alloc_repeated_bytes(struct pbtools_message_base_t *self_p,
                                 int length,
                                 struct pbtools_repeated_bytes_t *repeated_p)
{
    int i;
    struct pbtools_bytes_t *items_p;

    items_p = alloc_repeated((void ***)&repeated_p->items_pp,
                             self_p->heap_p,
                             length,
                             sizeof(*items_p));

    if (items_p == NULL) {
        return (-PBTOOLS_OUT_OF_MEMORY);
    }

    for (i = 0; i < length; i++) {
        items_p[i].size = 0;
        items_p[i].next_p = &items_p[i + 1];
        repeated_p->items_pp[i] = &items_p[i];
    }

    items_p[length - 1].next_p = NULL;
    repeated_p->length = length;
    repeated_p->head_p = &items_p[0];
    repeated_p->tail_p = &items_p[length - 1];

    return (0);
}

void pbtools_decoder_read_repeated_bytes(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_bytes_t *repeated_p)
{
    struct pbtools_bytes_t *item_p;

    item_p = pbtools_decoder_heap_alloc(self_p, sizeof(*item_p));

    if (item_p == NULL) {
        return;
    }

    pbtools_decoder_read_bytes(self_p, wire_type, item_p);

    if (repeated_p->length == 0) {
        repeated_p->head_p = item_p;
    } else {
        repeated_p->tail_p->next_p = item_p;
    }

    repeated_p->tail_p = item_p;
    repeated_p->length++;
}

void pbtools_decoder_finalize_repeated_bytes(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_bytes_t *repeated_p)
{
    struct pbtools_bytes_t *item_p;
    int i;

    if (repeated_p->length > 0) {
        repeated_p->items_pp = pbtools_decoder_heap_alloc(
            self_p,
            sizeof(item_p) * repeated_p->length);

        if (repeated_p->items_pp == NULL) {
            return;
        }

        item_p = repeated_p->head_p;

        for (i = 0; i < repeated_p->length; i++) {
            repeated_p->items_pp[i] = item_p;
            item_p = item_p->next_p;
        }
    }
}

void pbtools_decoder_init_slice(struct pbtools_decoder_t *self_p,
                                struct pbtools_decoder_t *parent_p,
                                int size)
{
    self_p->buf_p = &parent_p->buf_p[parent_p->pos];

    if (size <= (parent_p->size - parent_p->pos)) {
        self_p->size = size;
        self_p->pos = 0;
    } else {
        self_p->size = -PBTOOLS_OUT_OF_DATA;
        self_p->pos = -PBTOOLS_OUT_OF_DATA;
    }

    self_p->heap_p = parent_p->heap_p;
}

void pbtools_decoder_seek(struct pbtools_decoder_t *self_p,
                          int offset)
{
    PRINTF("seek: %d\n", offset);

    if (self_p->pos >= 0) {
        if (offset < 0) {
            pbtools_decoder_abort(self_p, -offset);
        } else if (((unsigned int)self_p->pos + (unsigned int)offset) > INT_MAX) {
            pbtools_decoder_abort(self_p, PBTOOLS_SEEK_OVERFLOW);
        } else {
            self_p->pos += offset;

            if (self_p->pos > self_p->size) {
                pbtools_decoder_abort(self_p, PBTOOLS_OUT_OF_DATA);
            }
        }
    }
}

void pbtools_decoder_skip_field(struct pbtools_decoder_t *self_p,
                                int wire_type)
{
    PRINTF("pbtools_decoder_skip_field(): %d\n", wire_type);
    uint64_t value;

    switch (wire_type) {

    case PBTOOLS_WIRE_TYPE_VARINT:
        (void)pbtools_decoder_read_varint(self_p);
        break;

    case PBTOOLS_WIRE_TYPE_FIXED_64:
        (void)pbtools_decoder_read_fixed64(self_p, PBTOOLS_WIRE_TYPE_FIXED_64);
        break;

    case PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED:
        value = pbtools_decoder_read_varint(self_p);

        if (value > INT_MAX) {
            pbtools_decoder_abort(self_p, PBTOOLS_LENGTH_DELIMITED_OVERFLOW);
        } else {
            pbtools_decoder_seek(self_p, (int)(int64_t)value);
        }

        break;

    case PBTOOLS_WIRE_TYPE_FIXED_32:
        (void)pbtools_decoder_read_fixed32(self_p, PBTOOLS_WIRE_TYPE_FIXED_32);
        break;

    default:
        pbtools_decoder_abort(self_p, PBTOOLS_BAD_WIRE_TYPE);
        break;
    }
}

void pbtools_set_string(struct pbtools_bytes_t *self_p,
                        char *string_p)
{
    self_p->buf_p = (uint8_t *)string_p;
    self_p->size = strlen(string_p);
}

char *pbtools_get_string(struct pbtools_bytes_t *self_p)
{
    return ((char *)self_p->buf_p);
}

void pbtools_string_init(struct pbtools_bytes_t *self_p)
{
    self_p->buf_p = (uint8_t *)"";
    self_p->size = 0;
}

void pbtools_bytes_init(struct pbtools_bytes_t *self_p)
{
    self_p->size = 0;
}

void *pbtools_message_new(
    void *workspace_p,
    size_t size,
    size_t message_size,
    pbtools_message_init_t message_init)
{
    void *self_p;
    struct pbtools_heap_t *heap_p;

    heap_p = pbtools_heap_new(workspace_p, size);

    if (heap_p == NULL) {
        return (NULL);
    }

    self_p = pbtools_heap_alloc(heap_p, message_size);

    if (self_p != NULL) {
        message_init(self_p, heap_p, NULL);
    }

    return (self_p);
}

int pbtools_message_encode(
    struct pbtools_message_base_t *self_p,
    uint8_t *encoded_p,
    size_t size,
    pbtools_message_encode_inner_t message_encode_inner)
{
    struct pbtools_encoder_t encoder;

    pbtools_encoder_init(&encoder, encoded_p, size);
    message_encode_inner(&encoder, self_p);

    return (pbtools_encoder_get_result(&encoder));
}

int pbtools_message_decode(
    struct pbtools_message_base_t *self_p,
    const uint8_t *encoded_p,
    size_t size,
    pbtools_message_decode_inner_t message_decode_inner)
{
    struct pbtools_decoder_t decoder;

    pbtools_decoder_init(&decoder, encoded_p, size, self_p->heap_p);
    message_decode_inner(&decoder, self_p);

    return (pbtools_decoder_get_result(&decoder));
}

int pbtools_alloc_repeated(
    struct pbtools_repeated_message_t *repeated_p,
    int length,
    struct pbtools_heap_t *heap_p,
    size_t item_size,
    pbtools_message_init_t message_init)
{
    int i;
    struct pbtools_message_base_t *item_p;
    struct pbtools_message_base_t *next_item_p;

    repeated_p->items_pp = pbtools_heap_alloc(
        heap_p,
        sizeof(item_p) * length);

    if (repeated_p->items_pp != NULL) {
        next_item_p = NULL;

        for (i = length - 1; i >= 0; i--) {
            item_p = pbtools_heap_alloc(heap_p, item_size);

            if (item_p == NULL) {
                return (-1);
            }

            message_init(item_p, heap_p, next_item_p);
            repeated_p->items_pp[i] = item_p;
            next_item_p = item_p;
        }

        repeated_p->length = length;
        repeated_p->head_p = repeated_p->items_pp[0];
        repeated_p->tail_p = repeated_p->items_pp[length - 1];
    }

    return (0);
}

void pbtools_encode_repeated_inner(
    struct pbtools_encoder_t *encoder_p,
    int field_number,
    struct pbtools_repeated_message_t *repeated_p,
    pbtools_message_encode_inner_t message_encode_inner)
{
    int i;
    int pos;

    for (i = repeated_p->length - 1; i >= 0; i--) {
        pos = encoder_p->pos;
        message_encode_inner(encoder_p, repeated_p->items_pp[i]);
        pbtools_encoder_write_length_delimited(encoder_p,
                                               field_number,
                                               pos - encoder_p->pos);
    }
}

void pbtools_decode_repeated_inner(
    struct pbtools_decoder_t *decoder_p,
    int wire_type,
    struct pbtools_repeated_message_t *repeated_p,
    size_t item_size,
    pbtools_message_init_t message_init,
    pbtools_message_decode_inner_t message_decode_inner)
{
    size_t size;
    struct pbtools_decoder_t decoder;
    struct pbtools_message_base_t *item_p;

    if (wire_type != PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED) {
        pbtools_decoder_abort(decoder_p, PBTOOLS_BAD_WIRE_TYPE);

        return;
    }

    item_p = pbtools_decoder_heap_alloc(decoder_p, item_size);

    if (item_p == NULL) {
        return;
    }

    size = pbtools_decoder_read_varint(decoder_p);
    message_init(item_p, decoder_p->heap_p, NULL);
    pbtools_decoder_init_slice(&decoder, decoder_p, size);
    message_decode_inner(&decoder, item_p);
    pbtools_decoder_seek(decoder_p, pbtools_decoder_get_result(&decoder));
    item_p->next_p = NULL;

    if (repeated_p->length == 0) {
        repeated_p->head_p = item_p;
    } else {
        repeated_p->tail_p->next_p = item_p;
    }

    repeated_p->tail_p = item_p;
    repeated_p->length++;
}

void pbtools_finalize_repeated_inner(
    struct pbtools_decoder_t *decoder_p,
    struct pbtools_repeated_message_t *repeated_p)
{
    int i;
    struct pbtools_message_base_t *item_p;

    if (repeated_p->length == 0) {
        return;
    }

    repeated_p->items_pp = pbtools_decoder_heap_alloc(
        decoder_p,
        sizeof(item_p) * repeated_p->length);

    if (repeated_p->items_pp == NULL) {
        return;
    }

    item_p = repeated_p->head_p;

    for (i = 0; i < repeated_p->length; i++) {
        repeated_p->items_pp[i] = item_p;
        item_p = item_p->next_p;
    }
}

void pbtools_encoder_sub_message_encode(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_message_base_t *message_p,
    pbtools_message_encode_inner_t encode_inner)
{
    int pos;

    pos = self_p->pos;
    encode_inner(self_p, message_p);
    pbtools_encoder_write_tagged_varint(self_p,
                                        field_number,
                                        PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED,
                                        pos - self_p->pos);
}

void pbtools_decoder_sub_message_decode(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_message_base_t *message_p,
    pbtools_message_decode_inner_t decode_inner)
{
    size_t size;
    struct pbtools_decoder_t decoder;

    if (wire_type != PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED) {
        pbtools_decoder_abort(self_p, PBTOOLS_BAD_WIRE_TYPE);

        return;
    }

    size = pbtools_decoder_read_varint(self_p);
    pbtools_decoder_init_slice(&decoder, self_p, size);
    decode_inner(&decoder, message_p);
    pbtools_decoder_seek(self_p, pbtools_decoder_get_result(&decoder));
}
