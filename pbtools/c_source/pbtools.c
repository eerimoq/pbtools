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

#include <limits.h>
#include "pbtools.h"

#define ALLOC_REPEATED_SCALAR_VALUE_TYPE(type)                          \
    alloc_repeated_scalar_value_type(                                   \
        self_p,                                                         \
        length,                                                         \
        (struct pbtools_repeated_scalar_value_type_t *)repeated_p,      \
        sizeof(struct pbtools_ ## type ## _t),                          \
        (scalar_value_type_init_t)pbtools_ ## type ## _init)

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

typedef void (*repeated_scalar_value_type_write_t)(
    struct pbtools_encoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p);

typedef void (*repeated_scalar_value_type_read_t)(
    struct pbtools_decoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p);

typedef void (*scalar_value_type_init_t)(void *self_p);

static void *decoder_heap_alloc(struct pbtools_decoder_t *self_p,
                                size_t size);

static uint64_t decoder_read_length_delimited(struct pbtools_decoder_t *self_p,
                                              int wire_type);

static struct pbtools_heap_t *heap_new(void *buf_p,
                                       size_t size)
{
    struct pbtools_heap_t *heap_p;

    if (size >= sizeof(*heap_p)) {
        heap_p = (struct pbtools_heap_t *)buf_p;
        heap_p->buf_p = buf_p;
        heap_p->size = (int)size;
        heap_p->pos = sizeof(*heap_p);
    } else {
        heap_p = NULL;
    }

    return (heap_p);
}

static void *heap_alloc(struct pbtools_heap_t *self_p,
                        size_t size)
{
    void *buf_p;
    int left;

    left = (self_p->size - self_p->pos);

    if ((int)size <= left) {
        buf_p = &self_p->buf_p[self_p->pos];
        self_p->pos += (int)size;
    } else {
        buf_p = NULL;
    }

    return (buf_p);
}

static void encoder_init(struct pbtools_encoder_t *self_p,
                         uint8_t *buf_p,
                         size_t  size)
{
    self_p->buf_p = buf_p;
    self_p->size = (int)size;
    self_p->pos = ((int)size - 1);
}

static int encoder_get_result(struct pbtools_encoder_t *self_p)
{
    int length;

    if (self_p->pos >= 0) {
        length = (self_p->size - self_p->pos - 1);
        memmove(self_p->buf_p,
                &self_p->buf_p[self_p->pos + 1],
                (size_t)length);
    } else {
        length = self_p->pos;
    }

    return (length);
}

static void encoder_abort(struct pbtools_encoder_t *self_p,
                          int error)
{
    if (self_p->size >= 0) {
        self_p->size = -error;
        self_p->pos = -error;
    }
}

static void encoder_write(struct pbtools_encoder_t *self_p,
                          uint8_t *buf_p,
                          int size)
{
    if (self_p->pos >= size) {
        self_p->pos -= size;
        memcpy(&self_p->buf_p[self_p->pos + 1], buf_p, (size_t)size);
    } else {
        encoder_abort(self_p, PBTOOLS_ENCODE_BUFFER_FULL);
    }
}

static void encoder_write_varint(struct pbtools_encoder_t *self_p,
                                 uint64_t value)
{
    uint8_t buf[10];
    int pos;

    pos = 0;

    do {
        buf[pos++] = (uint8_t)(value | 0x80);
        value >>= 7;
    } while (value > 0);

    buf[pos - 1] &= 0x7f;
    encoder_write(self_p, &buf[0], pos);
}

static void encoder_write_tag(struct pbtools_encoder_t *self_p,
                              int field_number,
                              int wire_type)
{
    encoder_write_varint(
        self_p,
        (uint64_t)((uint32_t)field_number << 3) | (uint32_t)wire_type);
}

static void encoder_write_tagged_varint(struct pbtools_encoder_t *self_p,
                                        int field_number,
                                        int wire_type,
                                        uint64_t value)
{
    if (value > 0) {
        encoder_write_varint(self_p, value);
        encoder_write_tag(self_p, field_number, wire_type);
    }
}

static void encoder_write_length_delimited(struct pbtools_encoder_t *self_p,
                                           int field_number,
                                           uint64_t value)
{
    encoder_write_varint(self_p, value);
    encoder_write_tag(self_p,
                      field_number,
                      PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED);
}

void pbtools_encoder_write_int32(struct pbtools_encoder_t *self_p,
                                 int field_number,
                                 int32_t value)
{
    encoder_write_tagged_varint(self_p,
                                field_number,
                                PBTOOLS_WIRE_TYPE_VARINT,
                                (uint64_t)(int64_t)value);
}

void pbtools_encoder_write_int64(struct pbtools_encoder_t *self_p,
                                 int field_number,
                                 int64_t value)
{
    encoder_write_tagged_varint(self_p,
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

static uint32_t sint32_encode(int32_t value)
{
    uint32_t data;

    if (value < 0) {
        data = ~((uint32_t)value << 1);
    } else {
        data = ((uint32_t)value << 1);
    }

    return (data);
}

static int32_t sint32_decode(uint64_t value)
{
    if (value & 0x1) {
        value >>= 1;
        value = ~value;
    } else {
        value >>= 1;
    }

    return ((int32_t)value);
}

static uint64_t sint64_encode(int64_t value)
{
    uint64_t data;

    if (value < 0) {
        data = ~((uint64_t)value << 1);
    } else {
        data = ((uint64_t)value << 1);
    }

    return (data);
}

static int64_t sint64_decode(uint64_t value)
{
    if (value & 0x1) {
        value >>= 1;
        value = ~value;
    } else {
        value >>= 1;
    }

    return ((int64_t)value);
}

void pbtools_encoder_write_sint64(struct pbtools_encoder_t *self_p,
                                  int field_number,
                                  int64_t value)
{
    pbtools_encoder_write_uint64(self_p, field_number, sint64_encode(value));
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
    encoder_write_tagged_varint(self_p,
                                field_number,
                                PBTOOLS_WIRE_TYPE_VARINT,
                                value);
}

static void encoder_write_fixed32_value(struct pbtools_encoder_t *self_p,
                                        uint32_t value)
{
    uint8_t buf[4];

    buf[0] = (uint8_t)(value & 0xff);
    buf[1] = (uint8_t)((value >> 8) & 0xff);
    buf[2] = (uint8_t)((value >> 16) & 0xff);
    buf[3] = (uint8_t)((value >> 24) & 0xff);
    encoder_write(self_p, &buf[0], 4);
}

void pbtools_encoder_write_fixed32(struct pbtools_encoder_t *self_p,
                                   int field_number,
                                   uint32_t value)
{
    if (value != 0) {
        encoder_write_fixed32_value(self_p, value);
        encoder_write_tag(self_p,
                          field_number,
                          PBTOOLS_WIRE_TYPE_FIXED_32);
    }
}

static void encoder_write_fixed64_value(struct pbtools_encoder_t *self_p,
                                        uint64_t value)
{
    uint8_t buf[8];

    buf[0] = (uint8_t)(value & 0xff);
    buf[1] = (uint8_t)((value >> 8) & 0xff);
    buf[2] = (uint8_t)((value >> 16) & 0xff);
    buf[3] = (uint8_t)((value >> 24) & 0xff);
    buf[4] = (uint8_t)((value >> 32) & 0xff);
    buf[5] = (uint8_t)((value >> 40) & 0xff);
    buf[6] = (uint8_t)((value >> 48) & 0xff);
    buf[7] = (uint8_t)((value >> 56) & 0xff);
    encoder_write(self_p, &buf[0], 8);
}

void pbtools_encoder_write_fixed64(struct pbtools_encoder_t *self_p,
                                   int field_number,
                                   uint64_t value)
{
    if (value != 0) {
        encoder_write_fixed64_value(self_p, value);
        encoder_write_tag(self_p,
                          field_number,
                          PBTOOLS_WIRE_TYPE_FIXED_64);
    }
}

void pbtools_encoder_write_sfixed32(struct pbtools_encoder_t *self_p,
                                    int field_number,
                                    int32_t value)
{
    pbtools_encoder_write_fixed32(self_p, field_number, (uint32_t)value);
}

void pbtools_encoder_write_sfixed64(struct pbtools_encoder_t *self_p,
                                    int field_number,
                                    int64_t value)
{
    pbtools_encoder_write_fixed64(self_p, field_number, (uint64_t)value);
}

#if PBTOOLS_CONFIG_FLOAT == 1

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

#endif

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
        encoder_write(self_p, (uint8_t *)value_p, (int)length);
        encoder_write_tagged_varint(self_p,
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
        encoder_write(self_p, value_p->buf_p, (int)value_p->size);
        encoder_write_tagged_varint(self_p,
                                    field_number,
                                    PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED,
                                    value_p->size);
    }
}

static int alloc_repeated_scalar_value_type(
    struct pbtools_message_base_t *self_p,
    int length,
    struct pbtools_repeated_scalar_value_type_t *repeated_p,
    size_t item_size,
    scalar_value_type_init_t item_init)
{
    int i;
    char *items_p;

    repeated_p->items_pp = heap_alloc(self_p->heap_p,
                                      sizeof(items_p) * (size_t)length);

    if (repeated_p->items_pp == NULL) {
        return (-PBTOOLS_OUT_OF_MEMORY);
    }

    items_p = heap_alloc(self_p->heap_p, item_size * (size_t)length);

    if (items_p == NULL) {
        return (-PBTOOLS_OUT_OF_MEMORY);
    }

    for (i = 0; i < length; i++) {
        item_init(items_p);
        repeated_p->items_pp[i] = (struct pbtools_scalar_value_type_base_t *)items_p;
        items_p += item_size;
    }

    repeated_p->items_pp[length - 1]->next_p = NULL;
    repeated_p->length = length;
    repeated_p->head_p = repeated_p->items_pp[0];
    repeated_p->tail_p = repeated_p->items_pp[length - 1];

    return (0);
}

static void write_repeated_scalar_value_type(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_scalar_value_type_t *repeated_p,
    repeated_scalar_value_type_write_t member_write)
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

    encoder_write_tagged_varint(self_p,
                                field_number,
                                PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED,
                                (uint64_t)(pos - self_p->pos));
}

static void read_repeated_scalar_value_type(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_scalar_value_type_t *repeated_p,
    size_t item_size,
    repeated_scalar_value_type_read_t item_read)
{
    size_t size;
    int pos;
    struct pbtools_scalar_value_type_base_t *item_p;

    /* Accept both packed and not packed. */
    switch (wire_type) {

    case PBTOOLS_WIRE_TYPE_VARINT:
    case PBTOOLS_WIRE_TYPE_FIXED_32:
    case PBTOOLS_WIRE_TYPE_FIXED_64:
        size = 1;
        break;

    default:
        size = (size_t)decoder_read_length_delimited(self_p, wire_type);
        break;
    }

    pos = self_p->pos;

    while ((size_t)self_p->pos < ((size_t)pos + size)) {
        item_p = decoder_heap_alloc(self_p, item_size);

        if (item_p == NULL) {
            return;
        }

        item_read(self_p, item_p);
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

static void write_repeated_int32_item(
    struct pbtools_encoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    encoder_write_varint(
        self_p,
        (uint64_t)((struct pbtools_int32_t *)item_p)->value);
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
    encoder_write_varint(
        self_p,
        (uint64_t)((struct pbtools_int64_t *)item_p)->value);
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
    encoder_write_varint(
        self_p,
        sint32_encode(((struct pbtools_int32_t *)item_p)->value));
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
    encoder_write_varint(
        self_p,
        sint64_encode(((struct pbtools_int64_t *)item_p)->value));
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
    encoder_write_varint(self_p,
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
    encoder_write_varint(self_p,
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
    encoder_write_fixed32_value(self_p,
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
    encoder_write_fixed64_value(self_p,
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
    encoder_write_fixed32_value(
        self_p,
        (uint32_t)((struct pbtools_int32_t *)item_p)->value);
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
    encoder_write_fixed64_value(
        self_p,
        (uint64_t)((struct pbtools_int64_t *)item_p)->value);
}

void pbtools_encoder_write_repeated_sfixed64(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_int64_t *repeated_p)
{
    WRITE_REPEATED_SCALAR_VALUE_TYPE(sfixed64);
}

#if PBTOOLS_CONFIG_FLOAT == 1

static void write_repeated_float_item(
    struct pbtools_encoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    uint32_t data;

    memcpy(&data, &(((struct pbtools_float_t *)item_p)->value), sizeof(data));
    encoder_write_fixed32_value(self_p, data);
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
    encoder_write_fixed64_value(self_p, data);
}

void pbtools_encoder_write_repeated_double(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_double_t *repeated_p)
{
    WRITE_REPEATED_SCALAR_VALUE_TYPE(double);
}

#endif

static void write_repeated_bool_item(
    struct pbtools_encoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    if (((struct pbtools_bool_t *)item_p)->value) {
        encoder_write_varint(self_p, 1);
    } else {
        encoder_write_varint(self_p, 0);
    }
}

void pbtools_encoder_write_repeated_bool(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_bool_t *repeated_p)
{
    WRITE_REPEATED_SCALAR_VALUE_TYPE(bool);
}

static void pbtools_int32_init(struct pbtools_int32_t *self_p)
{
    self_p->value = 0;
}

static void pbtools_int64_init(struct pbtools_int64_t *self_p)
{
    self_p->value = 0;
}

static void pbtools_uint32_init(struct pbtools_uint32_t *self_p)
{
    self_p->value = 0;
}

static void pbtools_uint64_init(struct pbtools_uint64_t *self_p)
{
    self_p->value = 0;
}

#if PBTOOLS_CONFIG_FLOAT == 1

static void pbtools_float_init(struct pbtools_float_t *self_p)
{
    self_p->value = 0;
}

static void pbtools_double_init(struct pbtools_double_t *self_p)
{
    self_p->value = 0;
}

#endif

static void pbtools_bool_init(struct pbtools_bool_t *self_p)
{
    self_p->value = false;
}

static void pbtools_string_init(struct pbtools_string_t *self_p)
{
    self_p->value_p = "";
}

void pbtools_encoder_write_repeated_string(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_string_t *repeated_p)
{
    int i;
    size_t length;

    for (i = repeated_p->length - 1; i >= 0; i--) {
        length = strlen(repeated_p->items_pp[i]->value_p);
        encoder_write(self_p,
                      (uint8_t *)repeated_p->items_pp[i]->value_p,
                      (int)length);
        encoder_write_length_delimited(self_p, field_number, length);
    }
}

void pbtools_encoder_write_repeated_bytes(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_bytes_t *repeated_p)
{
    int i;

    for (i = repeated_p->length - 1; i >= 0; i--) {
        encoder_write(self_p,
                      repeated_p->items_pp[i]->buf_p,
                      (int)repeated_p->items_pp[i]->size);
        encoder_write_length_delimited(self_p,
                                       field_number,
                                       repeated_p->items_pp[i]->size);
    }
}

static void decoder_init(struct pbtools_decoder_t *self_p,
                         const uint8_t *buf_p,
                         size_t size,
                         struct pbtools_heap_t *heap_p)
{
    self_p->buf_p = buf_p;
    self_p->size = (int)size;
    self_p->pos = 0;
    self_p->heap_p = heap_p;
}

static int decoder_get_result(struct pbtools_decoder_t *self_p)
{
    return (self_p->pos);
}

static void decoder_abort(struct pbtools_decoder_t *self_p,
                          int error)
{
    if (self_p->pos >= 0) {
        self_p->size = -error;
        self_p->pos = -error;
    }
}

static void *decoder_heap_alloc(struct pbtools_decoder_t *self_p,
                                size_t size)
{
    void *buf_p;

    buf_p = heap_alloc(self_p->heap_p, size);

    if (buf_p == NULL) {
        decoder_abort(self_p, PBTOOLS_OUT_OF_MEMORY);
    }

    return (buf_p);
}

bool pbtools_decoder_available(struct pbtools_decoder_t *self_p)
{
    return (self_p->pos < self_p->size);
}

static uint8_t decoder_get(struct pbtools_decoder_t *self_p)
{
    uint8_t value;

    if (pbtools_decoder_available(self_p)) {
        value = self_p->buf_p[self_p->pos];
        self_p->pos++;
    } else {
        decoder_abort(self_p, PBTOOLS_OUT_OF_DATA);
        value = 0;
    }

    return (value);
}

static void decoder_read(struct pbtools_decoder_t *self_p,
                         uint8_t *buf_p,
                         size_t size)
{
    if ((self_p->size - self_p->pos) >= (int)size) {
        memcpy(buf_p, &self_p->buf_p[self_p->pos], size);
        self_p->pos += (int)size;
    } else {
        memset(buf_p, 0, size);
        decoder_abort(self_p, PBTOOLS_OUT_OF_DATA);
    }
}

static uint64_t decoder_read_varint(struct pbtools_decoder_t *self_p)
{
    uint64_t value;
    uint8_t byte;
    int offset;

    value = 0;
    offset = 0;

    do {
        byte = decoder_get(self_p);
        value |= (((uint64_t)byte & 0x7f) << offset);
        offset += 7;
    } while ((offset < 64) && (byte & 0x80));

    if (byte & 0x80) {
        decoder_abort(self_p, PBTOOLS_VARINT_OVERFLOW);
        value = 0;
    }

    return (value);
}

static uint64_t read_varint_check_wire_type(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    int expected_wire_type)
{
    if (wire_type != expected_wire_type) {
        decoder_abort(self_p, PBTOOLS_BAD_WIRE_TYPE);

        return (0);
    }

    return (decoder_read_varint(self_p));
}

static uint64_t read_varint_check_wire_type_varint(
    struct pbtools_decoder_t *self_p,
    int wire_type)
{
    return (read_varint_check_wire_type(
                self_p,
                wire_type,
                PBTOOLS_WIRE_TYPE_VARINT));
}

static uint64_t decoder_read_length_delimited(struct pbtools_decoder_t *self_p,
                                              int wire_type)
{
    uint64_t length;

    length = read_varint_check_wire_type(
        self_p,
        wire_type,
        PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED);

    if (length >= INT_MAX) {
        decoder_abort(self_p, PBTOOLS_LENGTH_DELIMITED_OVERFLOW);
        length = 0;
    }

    return (length);
}

int pbtools_decoder_read_tag(struct pbtools_decoder_t *self_p,
                             int *wire_type_p)
{
    uint32_t value;
    int field_number;

    value = (uint32_t)decoder_read_varint(self_p);
    field_number = (int)(value >> 3);
    *wire_type_p = (value & 0x7);

    if (field_number == 0) {
        decoder_abort(self_p, PBTOOLS_BAD_FIELD_NUMBER);
    }

    return (field_number);
}

int32_t pbtools_decoder_read_int32(struct pbtools_decoder_t *self_p,
                                   int wire_type)
{
    return ((int32_t)pbtools_decoder_read_int64(self_p, wire_type));
}

int64_t pbtools_decoder_read_int64(struct pbtools_decoder_t *self_p,
                                   int wire_type)
{
    return ((int64_t)read_varint_check_wire_type_varint(
                self_p,
                wire_type));
}

int32_t pbtools_decoder_read_sint32(struct pbtools_decoder_t *self_p,
                                    int wire_type)
{
    return ((int32_t)pbtools_decoder_read_sint64(self_p, wire_type));
}

int64_t pbtools_decoder_read_sint64(struct pbtools_decoder_t *self_p,
                                    int wire_type)
{
    return (sint64_decode(read_varint_check_wire_type_varint(self_p,
                                                             wire_type)));
}

uint32_t pbtools_decoder_read_uint32(struct pbtools_decoder_t *self_p,
                                     int wire_type)
{
    return ((uint32_t)pbtools_decoder_read_uint64(self_p, wire_type));
}

uint64_t pbtools_decoder_read_uint64(struct pbtools_decoder_t *self_p,
                                     int wire_type)
{
    return (read_varint_check_wire_type_varint(self_p, wire_type));
}

static uint32_t decoder_read_fixed32_value(struct pbtools_decoder_t *self_p)
{
    uint32_t value;
    uint8_t buf[4];

    decoder_read(self_p, &buf[0], sizeof(buf));
    value = ((uint32_t)buf[0] << 0);
    value |= ((uint32_t)buf[1] << 8);
    value |= ((uint32_t)buf[2] << 16);
    value |= ((uint32_t)buf[3] << 24);

    return (value);
}

uint32_t pbtools_decoder_read_fixed32(struct pbtools_decoder_t *self_p,
                                      int wire_type)
{
    if (wire_type != PBTOOLS_WIRE_TYPE_FIXED_32) {
        decoder_abort(self_p, PBTOOLS_BAD_WIRE_TYPE);

        return (0);
    }

    return (decoder_read_fixed32_value(self_p));
}

static uint64_t decoder_read_fixed64_value(struct pbtools_decoder_t *self_p)
{
    uint64_t value;
    uint8_t buf[8];

    decoder_read(self_p, &buf[0], sizeof(buf));
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
    if (wire_type != PBTOOLS_WIRE_TYPE_FIXED_64) {
        decoder_abort(self_p, PBTOOLS_BAD_WIRE_TYPE);

        return (0);
    }

    return (decoder_read_fixed64_value(self_p));
}

int32_t pbtools_decoder_read_sfixed32(struct pbtools_decoder_t *self_p,
                                      int wire_type)
{
    return ((int32_t)pbtools_decoder_read_fixed32(self_p, wire_type));
}

int64_t pbtools_decoder_read_sfixed64(struct pbtools_decoder_t *self_p,
                                      int wire_type)
{
    return ((int64_t)pbtools_decoder_read_fixed64(self_p, wire_type));
}

#if PBTOOLS_CONFIG_FLOAT == 1

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

#endif

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

    size = decoder_read_length_delimited(self_p, wire_type);
    *value_pp = decoder_heap_alloc(self_p, size + 1);

    if (*value_pp == NULL) {
        return;
    }

    decoder_read(self_p, (uint8_t *)*value_pp, size);
    (*value_pp)[size] = '\0';
}

void pbtools_decoder_read_bytes(struct pbtools_decoder_t *self_p,
                                int wire_type,
                                struct pbtools_bytes_t *bytes_p)
{
    uint64_t size;

    size = decoder_read_length_delimited(self_p, wire_type);
    bytes_p->size = size;
    bytes_p->buf_p = decoder_heap_alloc(self_p, bytes_p->size);

    if (bytes_p->buf_p == NULL) {
        return;
    }

    decoder_read(self_p, bytes_p->buf_p, bytes_p->size);
}

int pbtools_alloc_repeated_int32(struct pbtools_message_base_t *self_p,
                                 int length,
                                 struct pbtools_repeated_int32_t *repeated_p)
{
    return (ALLOC_REPEATED_SCALAR_VALUE_TYPE(int32));
}

static void decoder_finalize_repeated_scalar_value_type(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_scalar_value_type_t *repeated_p)
{
    struct pbtools_scalar_value_type_base_t *item_p;
    int i;

    if (repeated_p->length > 0) {
        repeated_p->items_pp = decoder_heap_alloc(
            self_p,
            sizeof(item_p) * (size_t)repeated_p->length);

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

static void read_repeated_int32_item(
    struct pbtools_decoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    struct pbtools_int32_t *int32_item_p;

    int32_item_p = (struct pbtools_int32_t *)item_p;
    int32_item_p->value = (int32_t)decoder_read_varint(self_p);
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
    decoder_finalize_repeated_scalar_value_type(
        self_p,
        (struct pbtools_repeated_scalar_value_type_t *)repeated_p);
}

int pbtools_alloc_repeated_int64(struct pbtools_message_base_t *self_p,
                                 int length,
                                 struct pbtools_repeated_int64_t *repeated_p)
{
    return (ALLOC_REPEATED_SCALAR_VALUE_TYPE(int64));
}

static void read_repeated_int64_item(
    struct pbtools_decoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    struct pbtools_int64_t *int64_item_p;

    int64_item_p = (struct pbtools_int64_t *)item_p;
    int64_item_p->value = (int64_t)decoder_read_varint(self_p);
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
    decoder_finalize_repeated_scalar_value_type(
        self_p,
        (struct pbtools_repeated_scalar_value_type_t *)repeated_p);
}

int pbtools_alloc_repeated_uint32(struct pbtools_message_base_t *self_p,
                                  int length,
                                  struct pbtools_repeated_uint32_t *repeated_p)
{
    return (ALLOC_REPEATED_SCALAR_VALUE_TYPE(uint32));
}

static void read_repeated_uint32_item(
    struct pbtools_decoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    struct pbtools_uint32_t *uint32_item_p;

    uint32_item_p = (struct pbtools_uint32_t *)item_p;
    uint32_item_p->value = (uint32_t)decoder_read_varint(self_p);
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
    decoder_finalize_repeated_scalar_value_type(
        self_p,
        (struct pbtools_repeated_scalar_value_type_t *)repeated_p);
}

int pbtools_alloc_repeated_uint64(struct pbtools_message_base_t *self_p,
                                  int length,
                                  struct pbtools_repeated_uint64_t *repeated_p)
{
    return (ALLOC_REPEATED_SCALAR_VALUE_TYPE(uint64));
}

static void read_repeated_uint64_item(
    struct pbtools_decoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    ((struct pbtools_uint64_t *)item_p)->value = decoder_read_varint(self_p);
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
    decoder_finalize_repeated_scalar_value_type(
        self_p,
        (struct pbtools_repeated_scalar_value_type_t *)repeated_p);
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
    ((struct pbtools_int32_t *)item_p)->value = sint32_decode(
        decoder_read_varint(self_p));
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
    ((struct pbtools_int64_t *)item_p)->value = sint64_decode(
        decoder_read_varint(self_p));
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
        decoder_read_fixed32_value(self_p);
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
        decoder_read_fixed64_value(self_p);
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
    struct pbtools_int32_t *int32_item_p;

    int32_item_p = (struct pbtools_int32_t *)item_p;
    int32_item_p->value = (int32_t)decoder_read_fixed32_value(self_p);
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
    struct pbtools_int64_t *int64_item_p;

    int64_item_p = (struct pbtools_int64_t *)item_p;
    int64_item_p->value = (int64_t)decoder_read_fixed64_value(self_p);
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

#if PBTOOLS_CONFIG_FLOAT == 1

int pbtools_alloc_repeated_float(struct pbtools_message_base_t *self_p,
                                 int length,
                                 struct pbtools_repeated_float_t *repeated_p)
{
    return (ALLOC_REPEATED_SCALAR_VALUE_TYPE(float));
}

static void read_repeated_float_item(
    struct pbtools_decoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    uint32_t data;

    data = decoder_read_fixed32_value(self_p);
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
    decoder_finalize_repeated_scalar_value_type(
        self_p,
        (struct pbtools_repeated_scalar_value_type_t *)repeated_p);
}

int pbtools_alloc_repeated_double(struct pbtools_message_base_t *self_p,
                                  int length,
                                  struct pbtools_repeated_double_t *repeated_p)
{
    return (ALLOC_REPEATED_SCALAR_VALUE_TYPE(double));
}

static void read_repeated_double_item(
    struct pbtools_decoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    uint64_t data;

    data = decoder_read_fixed64_value(self_p);
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
    decoder_finalize_repeated_scalar_value_type(
        self_p,
        (struct pbtools_repeated_scalar_value_type_t *)repeated_p);
}

#endif

int pbtools_alloc_repeated_bool(struct pbtools_message_base_t *self_p,
                                int length,
                                struct pbtools_repeated_bool_t *repeated_p)
{
    return (ALLOC_REPEATED_SCALAR_VALUE_TYPE(bool));
}

static void read_repeated_bool_item(
    struct pbtools_decoder_t *self_p,
    struct pbtools_scalar_value_type_base_t *item_p)
{
    ((struct pbtools_bool_t *)item_p)->value = (
        decoder_read_varint(self_p) != 0);
}

void pbtools_decoder_read_repeated_bool(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_bool_t *repeated_p)
{
    READ_REPEATED_SCALAR_VALUE_TYPE(bool, bool);
}

void pbtools_decoder_finalize_repeated_bool(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_bool_t *repeated_p)
{
    decoder_finalize_repeated_scalar_value_type(
        self_p,
        (struct pbtools_repeated_scalar_value_type_t *)repeated_p);
}

int pbtools_alloc_repeated_string(struct pbtools_message_base_t *self_p,
                                  int length,
                                  struct pbtools_repeated_string_t *repeated_p)
{
    return (ALLOC_REPEATED_SCALAR_VALUE_TYPE(string));
}

void pbtools_decoder_read_repeated_string(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_string_t *repeated_p)
{
    struct pbtools_string_t *item_p;

    item_p = decoder_heap_alloc(self_p, sizeof(*item_p));

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
    decoder_finalize_repeated_scalar_value_type(
        self_p,
        (struct pbtools_repeated_scalar_value_type_t *)repeated_p);
}

int pbtools_alloc_repeated_bytes(struct pbtools_message_base_t *self_p,
                                 int length,
                                 struct pbtools_repeated_bytes_t *repeated_p)
{
    return (ALLOC_REPEATED_SCALAR_VALUE_TYPE(bytes));
}

void pbtools_decoder_read_repeated_bytes(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_bytes_t *repeated_p)
{
    struct pbtools_bytes_t *item_p;

    item_p = decoder_heap_alloc(self_p, sizeof(*item_p));

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
    decoder_finalize_repeated_scalar_value_type(
        self_p,
        (struct pbtools_repeated_scalar_value_type_t *)repeated_p);
}

static void decoder_init_slice(struct pbtools_decoder_t *self_p,
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

static void decoder_seek(struct pbtools_decoder_t *self_p,
                         int offset)
{
    if (self_p->pos >= 0) {
        if (offset < 0) {
            decoder_abort(self_p, -offset);
        } else if (((unsigned int)self_p->pos + (unsigned int)offset) > INT_MAX) {
            decoder_abort(self_p, PBTOOLS_SEEK_OVERFLOW);
        } else {
            self_p->pos += offset;

            if (self_p->pos > self_p->size) {
                decoder_abort(self_p, PBTOOLS_OUT_OF_DATA);
            }
        }
    }
}

void pbtools_decoder_skip_field(struct pbtools_decoder_t *self_p,
                                int wire_type)
{
    uint64_t value;

    switch (wire_type) {

    case PBTOOLS_WIRE_TYPE_VARINT:
        (void)decoder_read_varint(self_p);
        break;

    case PBTOOLS_WIRE_TYPE_FIXED_64:
        (void)pbtools_decoder_read_fixed64(self_p, PBTOOLS_WIRE_TYPE_FIXED_64);
        break;

    case PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED:
        value = decoder_read_length_delimited(self_p, wire_type);
        decoder_seek(self_p, (int)(int64_t)value);
        break;

    case PBTOOLS_WIRE_TYPE_FIXED_32:
        (void)pbtools_decoder_read_fixed32(self_p, PBTOOLS_WIRE_TYPE_FIXED_32);
        break;

    default:
        decoder_abort(self_p, PBTOOLS_BAD_WIRE_TYPE);
        break;
    }
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

    heap_p = heap_new(workspace_p, size);

    if (heap_p == NULL) {
        return (NULL);
    }

    self_p = heap_alloc(heap_p, message_size);

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

    encoder_init(&encoder, encoded_p, size);
    message_encode_inner(&encoder, self_p);

    return (encoder_get_result(&encoder));
}

int pbtools_message_decode(
    struct pbtools_message_base_t *self_p,
    const uint8_t *encoded_p,
    size_t size,
    pbtools_message_decode_inner_t message_decode_inner)
{
    struct pbtools_decoder_t decoder;

    decoder_init(&decoder, encoded_p, size, self_p->heap_p);
    message_decode_inner(&decoder, self_p);

    return (decoder_get_result(&decoder));
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

    repeated_p->items_pp = heap_alloc(
        heap_p,
        sizeof(item_p) * (size_t)length);

    if (repeated_p->items_pp != NULL) {
        next_item_p = NULL;

        for (i = length - 1; i >= 0; i--) {
            item_p = heap_alloc(heap_p, item_size);

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
        encoder_write_length_delimited(encoder_p,
                                       field_number,
                                       (uint64_t)(pos - encoder_p->pos));
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
    int size;
    struct pbtools_decoder_t decoder;
    struct pbtools_message_base_t *item_p;

    item_p = decoder_heap_alloc(decoder_p, item_size);

    if (item_p == NULL) {
        return;
    }

    size = (int)decoder_read_length_delimited(decoder_p, wire_type);
    message_init(item_p, decoder_p->heap_p, NULL);
    decoder_init_slice(&decoder, decoder_p, size);
    message_decode_inner(&decoder, item_p);
    decoder_seek(decoder_p, decoder_get_result(&decoder));
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

    repeated_p->items_pp = decoder_heap_alloc(
        decoder_p,
        sizeof(item_p) * (size_t)repeated_p->length);

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
    encoder_write_tagged_varint(self_p,
                                field_number,
                                PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED,
                                (uint64_t)(pos - self_p->pos));
}

void pbtools_decoder_sub_message_decode(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_message_base_t *message_p,
    pbtools_message_decode_inner_t decode_inner)
{
    int size;
    struct pbtools_decoder_t decoder;

    size = (int)decoder_read_length_delimited(self_p, wire_type);
    decoder_init_slice(&decoder, self_p, size);
    decode_inner(&decoder, message_p);
    decoder_seek(self_p, decoder_get_result(&decoder));
}

const char *pbtools_error_code_to_string(int code)
{
    const char *string_p;

    if (code < 0) {
        code *= -1;
    }

    switch (code) {

    case 0:
        string_p = "Ok";
        break;

    case PBTOOLS_BAD_WIRE_TYPE:
        string_p = "Bad wire type";
        break;

    case PBTOOLS_OUT_OF_DATA:
        string_p = "Out of data";
        break;

    case PBTOOLS_OUT_OF_MEMORY:
        string_p = "Out of memory";
        break;

    case PBTOOLS_ENCODE_BUFFER_FULL:
        string_p = "Encode buffer full";
        break;

    case PBTOOLS_BAD_FIELD_NUMBER:
        string_p = "Bad field number";
        break;

    case PBTOOLS_VARINT_OVERFLOW:
        string_p = "Varint overflow";
        break;

    case PBTOOLS_SEEK_OVERFLOW:
        string_p = "Seek overflow";
        break;

    case PBTOOLS_LENGTH_DELIMITED_OVERFLOW:
        string_p = "Length delimited overflow";
        break;

    default:
        string_p = "Unknown error";
        break;
    }

    return (string_p);
}
