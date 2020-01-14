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
#include <stdalign.h>
#include "pbtools.h"

typedef void (*repeated_item_read_t)(struct pbtools_decoder_t *self_p,
                                     void *items_p,
                                     int index);

typedef void (*repeated_item_write_t)(struct pbtools_encoder_t *self_p,
                                      int index,
                                      void *items_p);

typedef int (*decoder_read_repeated_t)(struct pbtools_decoder_t *self_p,
                                       int wire_type,
                                       void *items_p,
                                       int index);

static void *decoder_heap_alloc(struct pbtools_decoder_t *self_p,
                                size_t size,
                                size_t alignemnt);

static uint64_t decoder_read_length_delimited(struct pbtools_decoder_t *self_p,
                                              int wire_type);

static void decoder_init_slice(struct pbtools_decoder_t *self_p,
                               struct pbtools_decoder_t *parent_p,
                               int size);

static void decoder_seek(struct pbtools_decoder_t *self_p,
                         int offset);

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
                        size_t size,
                        size_t alignment)
{
    uintptr_t addr;
    int left;
    size_t rest;
    size_t pad;

    left = (self_p->size - self_p->pos);

    if ((int)size <= left) {
        addr = (uintptr_t)&self_p->buf_p[self_p->pos];
        rest = (addr & (alignment - 1));

        if (rest == 0) {
            self_p->pos += (int)size;
        } else {
            pad = (alignment - rest);
            size += pad;

            if ((int)size <= left) {
                addr += pad;
                self_p->pos += (int)size;
            } else {
                addr = (uintptr_t)NULL;
            }
        }
    } else {
        addr = (uintptr_t)NULL;
    }

    return ((void *)addr);
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

void encoder_write_length_delimited(struct pbtools_encoder_t *self_p,
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

uint32_t sint32_encode(int32_t value)
{
    uint32_t data;

    if (value < 0) {
        data = ~((uint32_t)value << 1);
    } else {
        data = ((uint32_t)value << 1);
    }

    return (data);
}

int32_t sint32_decode(uint64_t value)
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

static void encoder_write_32_bit_value(struct pbtools_encoder_t *self_p,
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
        encoder_write_32_bit_value(self_p, value);
        encoder_write_tag(self_p,
                          field_number,
                          PBTOOLS_WIRE_TYPE_32_BIT);
    }
}

static void encoder_write_64_bit_value(struct pbtools_encoder_t *self_p,
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
        encoder_write_64_bit_value(self_p, value);
        encoder_write_tag(self_p,
                          field_number,
                          PBTOOLS_WIRE_TYPE_64_BIT);
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

static void encoder_write_repeated(struct pbtools_encoder_t *self_p,
                                   int field_number,
                                   struct pbtools_repeated_message_t *repeated_p,
                                   repeated_item_write_t item_write)
{
    int i;
    int pos;

    if (repeated_p->length == 0) {
        return;
    }

    pos = self_p->pos;

    for (i = repeated_p->length - 1; i >= 0; i--) {
        item_write(self_p, i, repeated_p->items_p);
    }

    encoder_write_tagged_varint(self_p,
                                field_number,
                                PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED,
                                (uint64_t)(pos - self_p->pos));
}

static void write_repeated_int32(struct pbtools_encoder_t *self_p,
                                 int index,
                                 int32_t *items_p)
{
    encoder_write_varint(self_p, (uint64_t)(int64_t)items_p[index]);
}

static void write_repeated_int64(struct pbtools_encoder_t *self_p,
                                  int index,
                                  int64_t *items_p)
{
    encoder_write_varint(self_p, (uint64_t)items_p[index]);
}

static void write_repeated_sint32(struct pbtools_encoder_t *self_p,
                                  int index,
                                  int32_t *items_p)
{
    encoder_write_varint(self_p, sint32_encode(items_p[index]));
}

static void write_repeated_sint64(struct pbtools_encoder_t *self_p,
                                  int index,
                                  int64_t *items_p)
{
    encoder_write_varint(self_p, sint64_encode(items_p[index]));
}

static void write_repeated_uint32(struct pbtools_encoder_t *self_p,
                                  int index,
                                  uint32_t *items_p)
{
    encoder_write_varint(self_p, (uint64_t)items_p[index]);
}

static void write_repeated_uint64(struct pbtools_encoder_t *self_p,
                                  int index,
                                  uint64_t *items_p)
{
    encoder_write_varint(self_p, items_p[index]);
}

static void write_repeated_32bit(struct pbtools_encoder_t *self_p,
                                 int index,
                                 uint32_t *items_p)
{
    encoder_write_32_bit_value(self_p, items_p[index]);
}

static void write_repeated_64bit(struct pbtools_encoder_t *self_p,
                                 int index,
                                 uint64_t *items_p)
{
    encoder_write_64_bit_value(self_p, items_p[index]);
}

static void write_repeated_float(struct pbtools_encoder_t *self_p,
                                 int index,
                                 float *items_p)
{
    uint32_t data;

    memcpy(&data, &items_p[index], sizeof(data));
    encoder_write_32_bit_value(self_p, data);
}

static void write_repeated_double(struct pbtools_encoder_t *self_p,
                                  int index,
                                  double *items_p)
{
    uint64_t data;

    memcpy(&data, &items_p[index], sizeof(data));
    encoder_write_64_bit_value(self_p, data);
}

static void write_repeated_bool(struct pbtools_encoder_t *self_p,
                                int index,
                                bool *items_p)
{
    if (items_p[index]) {
        encoder_write_varint(self_p, 1);
    } else {
        encoder_write_varint(self_p, 0);
    }
}

void pbtools_encoder_write_repeated_int32(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_int32_t *repeated_p)
{
    encoder_write_repeated(
        self_p,
        field_number,
        (struct pbtools_repeated_message_t *)repeated_p,
        (repeated_item_write_t)write_repeated_int32);
}

void pbtools_encoder_write_repeated_int64(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_int64_t *repeated_p)
{
    encoder_write_repeated(
        self_p,
        field_number,
        (struct pbtools_repeated_message_t *)repeated_p,
        (repeated_item_write_t)write_repeated_int64);
}

void pbtools_encoder_write_repeated_sint32(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_int32_t *repeated_p)
{
    encoder_write_repeated(
        self_p,
        field_number,
        (struct pbtools_repeated_message_t *)repeated_p,
        (repeated_item_write_t)write_repeated_sint32);
}

void pbtools_encoder_write_repeated_sint64(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_int64_t *repeated_p)
{
    encoder_write_repeated(
        self_p,
        field_number,
        (struct pbtools_repeated_message_t *)repeated_p,
        (repeated_item_write_t)write_repeated_sint64);
}

void pbtools_encoder_write_repeated_uint32(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_uint32_t *repeated_p)
{
    encoder_write_repeated(
        self_p,
        field_number,
        (struct pbtools_repeated_message_t *)repeated_p,
        (repeated_item_write_t)write_repeated_uint32);
}

void pbtools_encoder_write_repeated_uint64(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_uint64_t *repeated_p)
{
    encoder_write_repeated(
        self_p,
        field_number,
        (struct pbtools_repeated_message_t *)repeated_p,
        (repeated_item_write_t)write_repeated_uint64);
}

void pbtools_encoder_write_repeated_fixed32(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_uint32_t *repeated_p)
{
    encoder_write_repeated(self_p,
                           field_number,
                           (struct pbtools_repeated_message_t *)repeated_p,
                           (repeated_item_write_t)write_repeated_32bit);
}

void pbtools_encoder_write_repeated_fixed64(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_uint64_t *repeated_p)
{
    encoder_write_repeated(self_p,
                           field_number,
                           (struct pbtools_repeated_message_t *)repeated_p,
                           (repeated_item_write_t)write_repeated_64bit);
}

void pbtools_encoder_write_repeated_sfixed32(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_int32_t *repeated_p)
{
    encoder_write_repeated(self_p,
                           field_number,
                           (struct pbtools_repeated_message_t *)repeated_p,
                           (repeated_item_write_t)write_repeated_32bit);
}

void pbtools_encoder_write_repeated_sfixed64(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_int64_t *repeated_p)
{
    encoder_write_repeated(self_p,
                           field_number,
                           (struct pbtools_repeated_message_t *)repeated_p,
                           (repeated_item_write_t)write_repeated_64bit);
}

#if PBTOOLS_CONFIG_FLOAT == 1

void pbtools_encoder_write_repeated_float(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_float_t *repeated_p)
{
    encoder_write_repeated(self_p,
                           field_number,
                           (struct pbtools_repeated_message_t *)repeated_p,
                           (repeated_item_write_t)write_repeated_float);
}

void pbtools_encoder_write_repeated_double(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_double_t *repeated_p)
{
    encoder_write_repeated(self_p,
                           field_number,
                           (struct pbtools_repeated_message_t *)repeated_p,
                           (repeated_item_write_t)write_repeated_double);
}

#endif

void pbtools_encoder_write_repeated_bool(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_bool_t *repeated_p)
{
    encoder_write_repeated(self_p,
                           field_number,
                           (struct pbtools_repeated_message_t *)repeated_p,
                           (repeated_item_write_t)write_repeated_bool);
}

void pbtools_encoder_write_repeated_string(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_string_t *repeated_p)
{
    int i;
    size_t length;

    for (i = repeated_p->length - 1; i >= 0; i--) {
        length = strlen(repeated_p->items_pp[i]);
        encoder_write(self_p,
                      (uint8_t *)repeated_p->items_pp[i],
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
                      repeated_p->items_p[i].buf_p,
                      (int)repeated_p->items_p[i].size);
        encoder_write_length_delimited(self_p,
                                       field_number,
                                       repeated_p->items_p[i].size);
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
                                size_t size,
                                size_t alignment)
{
    void *buf_p;

    buf_p = heap_alloc(self_p->heap_p, size, alignment);

    if (buf_p == NULL) {
        decoder_abort(self_p, PBTOOLS_OUT_OF_MEMORY);
    }

    return (buf_p);
}

static int decoder_left(struct pbtools_decoder_t *self_p)
{
    return (self_p->size - self_p->pos);
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

static uint32_t decoder_read_32_bit_value(struct pbtools_decoder_t *self_p)
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
    if (wire_type != PBTOOLS_WIRE_TYPE_32_BIT) {
        decoder_abort(self_p, PBTOOLS_BAD_WIRE_TYPE);

        return (0);
    }

    return (decoder_read_32_bit_value(self_p));
}

static uint64_t decoder_read_64_bit_value(struct pbtools_decoder_t *self_p)
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
    if (wire_type != PBTOOLS_WIRE_TYPE_64_BIT) {
        decoder_abort(self_p, PBTOOLS_BAD_WIRE_TYPE);

        return (0);
    }

    return (decoder_read_64_bit_value(self_p));
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
    *value_pp = decoder_heap_alloc(self_p,
                                   size + 1,
                                   alignof(**value_pp));

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
    bytes_p->buf_p = decoder_heap_alloc(self_p,
                                        bytes_p->size,
                                        alignof(*bytes_p->buf_p));

    if (bytes_p->buf_p == NULL) {
        return;
    }

    decoder_read(self_p, bytes_p->buf_p, bytes_p->size);
}

static void repeated_info_decode_varint(struct pbtools_repeated_info_t *self_p,
                                        struct pbtools_decoder_t *decoder_p,
                                        int wire_type)
{
    int size;
    struct pbtools_decoder_t decoder;

    /* Accept both packed and not packed. */
    switch (wire_type) {

    case PBTOOLS_WIRE_TYPE_VARINT:
        (void)decoder_read_varint(decoder_p);
        self_p->length++;
        break;

    case PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED:
        size = (int)decoder_read_length_delimited(decoder_p, wire_type);
        decoder_init_slice(&decoder, decoder_p, size);

        while (pbtools_decoder_available(&decoder)) {
            (void)decoder_read_varint(&decoder);
            self_p->length++;
        }

        decoder_seek(decoder_p, decoder_get_result(&decoder));
        break;

    default:
        decoder_abort(decoder_p, PBTOOLS_BAD_WIRE_TYPE);
        break;
    }
}

static void repeated_info_decode_32bit(struct pbtools_repeated_info_t *self_p,
                                       struct pbtools_decoder_t *decoder_p,
                                       int wire_type)
{
    int size;

    /* Accept both packed and not packed. */
    switch (wire_type) {

    case PBTOOLS_WIRE_TYPE_32_BIT:
        decoder_seek(decoder_p, 4);
        self_p->length++;
        break;

    case PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED:
        size = (int)decoder_read_length_delimited(decoder_p, wire_type);
        self_p->length += (size / 4);
        decoder_seek(decoder_p, size);
        break;

    default:
        decoder_abort(decoder_p, PBTOOLS_BAD_WIRE_TYPE);
        break;
    }
}

static void repeated_info_decode_64bit(struct pbtools_repeated_info_t *self_p,
                                       struct pbtools_decoder_t *decoder_p,
                                       int wire_type)
{
    int size;

    /* Accept both packed and not packed. */
    switch (wire_type) {

    case PBTOOLS_WIRE_TYPE_64_BIT:
        decoder_seek(decoder_p, 8);
        self_p->length++;
        break;

    case PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED:
        size = (int)decoder_read_length_delimited(decoder_p, wire_type);
        self_p->length += (size / 8);
        decoder_seek(decoder_p, size);
        break;

    default:
        decoder_abort(decoder_p, PBTOOLS_BAD_WIRE_TYPE);
        break;
    }
}

static void repeated_info_decode_length_delimited(
    struct pbtools_repeated_info_t *self_p,
    struct pbtools_decoder_t *decoder_p,
    int wire_type)
{
    int size;

    size = (int)decoder_read_length_delimited(decoder_p, wire_type);
    decoder_seek(decoder_p, size);
    self_p->length++;
}

static int decoder_read_repeated(struct pbtools_decoder_t *self_p,
                                 int wire_type,
                                 void *items_p,
                                 repeated_item_read_t item_read,
                                 int item_wire_type)
{
    int size;
    struct pbtools_decoder_t decoder;
    int pos;

    pos = 0;

    /* Accept both packed and not packed. */
    if (wire_type == item_wire_type) {
        item_read(self_p, items_p, pos);
        pos++;
    } else if (wire_type == PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED) {
        size = (int)decoder_read_length_delimited(self_p, wire_type);
        decoder_init_slice(&decoder, self_p, size);

        while (pbtools_decoder_available(&decoder)) {
            item_read(&decoder, items_p, pos);
            pos++;
        }

        decoder_seek(self_p, decoder_get_result(&decoder));
    } else {
        decoder_abort(self_p, PBTOOLS_BAD_WIRE_TYPE);
    }

    return (pos);
}

static int decoder_read_repeated_varint(struct pbtools_decoder_t *self_p,
                                        int wire_type,
                                        void *items_p,
                                        repeated_item_read_t item_read)
{
    return (decoder_read_repeated(self_p,
                                  wire_type,
                                  items_p,
                                  item_read,
                                  PBTOOLS_WIRE_TYPE_VARINT));
}

static int decoder_read_repeated_32_bit(struct pbtools_decoder_t *self_p,
                                        int wire_type,
                                        void *items_p,
                                        repeated_item_read_t item_read)
{
    return (decoder_read_repeated(self_p,
                                  wire_type,
                                  items_p,
                                  item_read,
                                  PBTOOLS_WIRE_TYPE_32_BIT));
}

static int decoder_read_repeated_64_bit(struct pbtools_decoder_t *self_p,
                                        int wire_type,
                                        void *items_p,
                                        repeated_item_read_t item_read)
{
    return (decoder_read_repeated(self_p,
                                  wire_type,
                                  items_p,
                                  item_read,
                                  PBTOOLS_WIRE_TYPE_64_BIT));
}

static int calloc_repeated(struct pbtools_message_base_t *self_p,
                           size_t size,
                           void **items_pp)
{
    *items_pp = heap_alloc(self_p->heap_p, size, alignof(uint64_t));

    if (*items_pp == NULL) {
        return (-1);
    }

    memset(*items_pp, 0, size);

    return (0);
}

static int alloc_repeated_int32(struct pbtools_message_base_t *self_p,
                                int length,
                                struct pbtools_repeated_int32_t *repeated_p)
{
    repeated_p->length = length;

    return (calloc_repeated(self_p,
                            sizeof(*repeated_p->items_p) * (size_t)length,
                            (void **)&repeated_p->items_p));
}

static int alloc_repeated_uint32(struct pbtools_message_base_t *self_p,
                                 int length,
                                 struct pbtools_repeated_uint32_t *repeated_p)
{
    repeated_p->length = length;

    return (calloc_repeated(self_p,
                            sizeof(*repeated_p->items_p) * (size_t)length,
                            (void **)&repeated_p->items_p));
}

static int alloc_repeated_int64(struct pbtools_message_base_t *self_p,
                                int length,
                                struct pbtools_repeated_int64_t *repeated_p)
{
    repeated_p->length = length;

    return (calloc_repeated(self_p,
                            sizeof(*repeated_p->items_p) * (size_t)length,
                            (void **)&repeated_p->items_p));
}

static int alloc_repeated_uint64(struct pbtools_message_base_t *self_p,
                                 int length,
                                 struct pbtools_repeated_uint64_t *repeated_p)
{
    repeated_p->length = length;

    return (calloc_repeated(self_p,
                            sizeof(*repeated_p->items_p) * (size_t)length,
                            (void **)&repeated_p->items_p));
}

static bool are_more_items_to_decode(struct pbtools_repeated_info_t *repeated_info_p,
                                     int pos)
{
    return (pbtools_decoder_available(&repeated_info_p->decoder)
            && (pos < repeated_info_p->length));
}

int pbtools_alloc_repeated_int32(struct pbtools_message_base_t *self_p,
                                 int length,
                                 struct pbtools_repeated_int32_t *repeated_p)
{
    return (alloc_repeated_int32(self_p, length, repeated_p));
}

void pbtools_repeated_info_decode_int32(struct pbtools_repeated_info_t *self_p,
                                        struct pbtools_decoder_t *decoder_p,
                                        int wire_type)
{
    repeated_info_decode_varint(self_p, decoder_p, wire_type);
}

static void decoder_decode_repeated(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_message_t *repeated_p,
    size_t item_size,
    decoder_read_repeated_t read_repeated)
{
    int res;
    int wire_type;
    int tag;
    int pos;

    repeated_p->items_p = decoder_heap_alloc(
        self_p,
        item_size * (size_t)repeated_info_p->length,
        alignof(uint64_t));

    if (repeated_p->items_p == NULL) {
        return;
    }

    repeated_p->length = repeated_info_p->length;
    pos = 0;

    while (are_more_items_to_decode(repeated_info_p, pos)) {
        tag = pbtools_decoder_read_tag(&repeated_info_p->decoder, &wire_type);

        if (tag == repeated_info_p->tag) {
            pos += read_repeated(&repeated_info_p->decoder,
                                 wire_type,
                                 repeated_p->items_p,
                                 pos);
        } else {
            pbtools_decoder_skip_field(&repeated_info_p->decoder, wire_type);
        }
    }

    res = decoder_get_result(&repeated_info_p->decoder);

    if (res < 0) {
        decoder_abort(self_p, -res);
    }
}

static void read_repeated_int32_item(struct pbtools_decoder_t *self_p,
                                     int32_t *items_p,
                                     int index)
{
    items_p[index] = (int32_t)(int64_t)decoder_read_varint(self_p);
}

static void read_repeated_int64_item(struct pbtools_decoder_t *self_p,
                                     int64_t *items_p,
                                     int index)
{
    items_p[index] = (int64_t)decoder_read_varint(self_p);
}

static void read_repeated_sint32_item(struct pbtools_decoder_t *self_p,
                                      int32_t *items_p,
                                      int index)
{
    items_p[index] = sint32_decode(decoder_read_varint(self_p));
}


static void read_repeated_sint64_item(struct pbtools_decoder_t *self_p,
                                      int64_t *items_p,
                                      int index)
{
    items_p[index] = sint64_decode(decoder_read_varint(self_p));
}

static void read_repeated_uint32_item(struct pbtools_decoder_t *self_p,
                                      uint32_t *items_p,
                                      int index)
{
    items_p[index] = (uint32_t)decoder_read_varint(self_p);
}

static void read_repeated_uint64_item(struct pbtools_decoder_t *self_p,
                                      uint64_t *items_p,
                                      int index)
{
    items_p[index] = decoder_read_varint(self_p);
}

static void read_repeated_fixed32_item(struct pbtools_decoder_t *self_p,
                                       uint32_t *items_p,
                                       int index)
{
    items_p[index] = decoder_read_32_bit_value(self_p);
}

static void read_repeated_fixed64_item(struct pbtools_decoder_t *self_p,
                                       uint64_t *items_p,
                                       int index)
{
    items_p[index] = decoder_read_64_bit_value(self_p);
}

static void read_repeated_sfixed32_item(struct pbtools_decoder_t *self_p,
                                        int32_t *items_p,
                                        int index)
{
    items_p[index] = (int32_t)decoder_read_32_bit_value(self_p);
}

static void read_repeated_sfixed64_item(struct pbtools_decoder_t *self_p,
                                        int64_t *items_p,
                                        int index)
{
    items_p[index] = (int64_t)decoder_read_64_bit_value(self_p);
}

static void read_repeated_bool_item(struct pbtools_decoder_t *self_p,
                                    bool *items_p,
                                    int index)
{
    items_p[index] = (bool)decoder_read_varint(self_p);
}

static void read_repeated_float_item(struct pbtools_decoder_t *self_p,
                                     float *items_p,
                                     int index)
{
    uint32_t data;

    data = (uint32_t)decoder_read_32_bit_value(self_p);
    memcpy(&items_p[index], &data, sizeof(data));
}

static void read_repeated_double_item(struct pbtools_decoder_t *self_p,
                                      double *items_p,
                                      int index)
{
    uint64_t data;

    data = (uint64_t)decoder_read_64_bit_value(self_p);
    memcpy(&items_p[index], &data, sizeof(data));
}

static int read_repeated_int32(struct pbtools_decoder_t *self_p,
                               int wire_type,
                               int32_t *items_p,
                               int index)
{
    return (decoder_read_repeated_varint(
                self_p,
                wire_type,
                &items_p[index],
                (repeated_item_read_t)read_repeated_int32_item));
}

static int read_repeated_int64(struct pbtools_decoder_t *self_p,
                               int wire_type,
                               int64_t *items_p,
                               int index)
{
    return (decoder_read_repeated_varint(
                self_p,
                wire_type,
                &items_p[index],
                (repeated_item_read_t)read_repeated_int64_item));
}

static int read_repeated_uint32(struct pbtools_decoder_t *self_p,
                                int wire_type,
                                uint32_t *items_p,
                                int index)
{
    return (decoder_read_repeated_varint(
                self_p,
                wire_type,
                &items_p[index],
                (repeated_item_read_t)read_repeated_uint32_item));
}

static int read_repeated_uint64(struct pbtools_decoder_t *self_p,
                                int wire_type,
                                uint64_t *items_p,
                                int index)
{
    return (decoder_read_repeated_varint(
                self_p,
                wire_type,
                &items_p[index],
                (repeated_item_read_t)read_repeated_uint64_item));
}

static int read_repeated_sint32(struct pbtools_decoder_t *self_p,
                                int wire_type,
                                int32_t *items_p,
                                int index)
{
    return (decoder_read_repeated_varint(
                self_p,
                wire_type,
                &items_p[index],
                (repeated_item_read_t)read_repeated_sint32_item));
}

static int read_repeated_sint64(struct pbtools_decoder_t *self_p,
                                int wire_type,
                                int64_t *items_p,
                                int index)
{
    return (decoder_read_repeated_varint(
                self_p,
                wire_type,
                &items_p[index],
                (repeated_item_read_t)read_repeated_sint64_item));
}

static int read_repeated_fixed32(struct pbtools_decoder_t *self_p,
                                 int wire_type,
                                 uint32_t *items_p,
                                 int index)
{
    return (decoder_read_repeated_32_bit(
                self_p,
                wire_type,
                &items_p[index],
                (repeated_item_read_t)read_repeated_fixed32_item));
}

static int read_repeated_fixed64(struct pbtools_decoder_t *self_p,
                                 int wire_type,
                                 uint64_t *items_p,
                                 int index)
{
    return (decoder_read_repeated_64_bit(
                self_p,
                wire_type,
                &items_p[index],
                (repeated_item_read_t)read_repeated_fixed64_item));
}

static int read_repeated_sfixed32(struct pbtools_decoder_t *self_p,
                                  int wire_type,
                                  int32_t *items_p,
                                  int index)
{
    return (decoder_read_repeated_32_bit(
                self_p,
                wire_type,
                &items_p[index],
                (repeated_item_read_t)read_repeated_sfixed32_item));
}

static int read_repeated_sfixed64(struct pbtools_decoder_t *self_p,
                                  int wire_type,
                                  int64_t *items_p,
                                  int index)
{
    return (decoder_read_repeated_64_bit(
                self_p,
                wire_type,
                &items_p[index],
                (repeated_item_read_t)read_repeated_sfixed64_item));
}

static int read_repeated_float(struct pbtools_decoder_t *self_p,
                               int wire_type,
                               float *items_p,
                               int index)
{
    return (decoder_read_repeated_32_bit(
                self_p,
                wire_type,
                &items_p[index],
                (repeated_item_read_t)read_repeated_float_item));
}

static int read_repeated_double(struct pbtools_decoder_t *self_p,
                                int wire_type,
                                double *items_p,
                                int index)
{
    return (decoder_read_repeated_64_bit(
                self_p,
                wire_type,
                &items_p[index],
                (repeated_item_read_t)read_repeated_double_item));
}

static int read_repeated_bool(struct pbtools_decoder_t *self_p,
                              int wire_type,
                              bool *items_p,
                              int index)
{
    return (decoder_read_repeated_varint(
                self_p,
                wire_type,
                &items_p[index],
                (repeated_item_read_t)read_repeated_bool_item));
}

static int read_repeated_string(struct pbtools_decoder_t *self_p,
                                int wire_type,
                                char **items_pp,
                                int index)
{
    pbtools_decoder_read_string(self_p, wire_type, &items_pp[index]);

    return (1);
}

static int read_repeated_bytes(struct pbtools_decoder_t *self_p,
                               int wire_type,
                               struct pbtools_bytes_t *items_p,
                               int index)
{
    pbtools_decoder_read_bytes(self_p, wire_type, &items_p[index]);

    return (1);
}

void pbtools_decoder_decode_repeated_int32(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_int32_t *repeated_p)
{
    decoder_decode_repeated(self_p,
                            repeated_info_p,
                            (struct pbtools_repeated_message_t *)repeated_p,
                            sizeof(int32_t),
                            (decoder_read_repeated_t)read_repeated_int32);
}

int pbtools_alloc_repeated_int64(struct pbtools_message_base_t *self_p,
                                 int length,
                                 struct pbtools_repeated_int64_t *repeated_p)
{
    return (alloc_repeated_int64(self_p, length, repeated_p));
}

void pbtools_repeated_info_decode_int64(struct pbtools_repeated_info_t *self_p,
                                        struct pbtools_decoder_t *decoder_p,
                                        int wire_type)
{
    repeated_info_decode_varint(self_p, decoder_p, wire_type);
}

void pbtools_decoder_decode_repeated_int64(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_int64_t *repeated_p)
{
    decoder_decode_repeated(self_p,
                            repeated_info_p,
                            (struct pbtools_repeated_message_t *)repeated_p,
                            sizeof(int64_t),
                            (decoder_read_repeated_t)read_repeated_int64);
}

int pbtools_alloc_repeated_uint32(struct pbtools_message_base_t *self_p,
                                  int length,
                                  struct pbtools_repeated_uint32_t *repeated_p)
{
    return (alloc_repeated_uint32(self_p, length, repeated_p));
}

void pbtools_repeated_info_decode_uint32(struct pbtools_repeated_info_t *self_p,
                                         struct pbtools_decoder_t *decoder_p,
                                         int wire_type)
{
    repeated_info_decode_varint(self_p, decoder_p, wire_type);
}

void pbtools_decoder_decode_repeated_uint32(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_uint32_t *repeated_p)
{
    decoder_decode_repeated(self_p,
                            repeated_info_p,
                            (struct pbtools_repeated_message_t *)repeated_p,
                            sizeof(uint32_t),
                            (decoder_read_repeated_t)read_repeated_uint32);
}

int pbtools_alloc_repeated_uint64(struct pbtools_message_base_t *self_p,
                                  int length,
                                  struct pbtools_repeated_uint64_t *repeated_p)
{
    return (alloc_repeated_uint64(self_p, length, repeated_p));
}

void pbtools_repeated_info_decode_uint64(struct pbtools_repeated_info_t *self_p,
                                         struct pbtools_decoder_t *decoder_p,
                                         int wire_type)
{
    repeated_info_decode_varint(self_p, decoder_p, wire_type);
}

void pbtools_decoder_decode_repeated_uint64(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_uint64_t *repeated_p)
{
    decoder_decode_repeated(self_p,
                            repeated_info_p,
                            (struct pbtools_repeated_message_t *)repeated_p,
                            sizeof(uint64_t),
                            (decoder_read_repeated_t)read_repeated_uint64);
}

int pbtools_alloc_repeated_sint32(struct pbtools_message_base_t *self_p,
                                  int length,
                                  struct pbtools_repeated_int32_t *repeated_p)
{
    return (alloc_repeated_int32(self_p, length, repeated_p));
}

void pbtools_repeated_info_decode_sint32(struct pbtools_repeated_info_t *self_p,
                                         struct pbtools_decoder_t *decoder_p,
                                         int wire_type)
{
    repeated_info_decode_varint(self_p, decoder_p, wire_type);
}

void pbtools_decoder_decode_repeated_sint32(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_int32_t *repeated_p)
{
    decoder_decode_repeated(self_p,
                            repeated_info_p,
                            (struct pbtools_repeated_message_t *)repeated_p,
                            sizeof(int32_t),
                            (decoder_read_repeated_t)read_repeated_sint32);
}

int pbtools_alloc_repeated_sint64(struct pbtools_message_base_t *self_p,
                                  int length,
                                  struct pbtools_repeated_int64_t *repeated_p)
{
    return (alloc_repeated_int64(self_p, length, repeated_p));
}

void pbtools_repeated_info_decode_sint64(struct pbtools_repeated_info_t *self_p,
                                         struct pbtools_decoder_t *decoder_p,
                                         int wire_type)
{
    repeated_info_decode_varint(self_p, decoder_p, wire_type);
}

void pbtools_decoder_decode_repeated_sint64(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_int64_t *repeated_p)
{
    decoder_decode_repeated(self_p,
                            repeated_info_p,
                            (struct pbtools_repeated_message_t *)repeated_p,
                            sizeof(int64_t),
                            (decoder_read_repeated_t)read_repeated_sint64);
}

int pbtools_alloc_repeated_fixed32(struct pbtools_message_base_t *self_p,
                                   int length,
                                   struct pbtools_repeated_uint32_t *repeated_p)
{
    return (alloc_repeated_uint32(self_p, length, repeated_p));
}

void pbtools_repeated_info_decode_fixed32(struct pbtools_repeated_info_t *self_p,
                                          struct pbtools_decoder_t *decoder_p,
                                          int wire_type)
{
    repeated_info_decode_32bit(self_p, decoder_p, wire_type);
}

void pbtools_decoder_decode_repeated_fixed32(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_uint32_t *repeated_p)
{
    decoder_decode_repeated(self_p,
                            repeated_info_p,
                            (struct pbtools_repeated_message_t *)repeated_p,
                            sizeof(uint32_t),
                            (decoder_read_repeated_t)read_repeated_fixed32);
}

int pbtools_alloc_repeated_fixed64(struct pbtools_message_base_t *self_p,
                                   int length,
                                   struct pbtools_repeated_uint64_t *repeated_p)
{
    return (alloc_repeated_uint64(self_p, length, repeated_p));
}

void pbtools_repeated_info_decode_fixed64(struct pbtools_repeated_info_t *self_p,
                                          struct pbtools_decoder_t *decoder_p,
                                          int wire_type)
{
    repeated_info_decode_64bit(self_p, decoder_p, wire_type);
}

void pbtools_decoder_decode_repeated_fixed64(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_uint64_t *repeated_p)
{
    decoder_decode_repeated(self_p,
                            repeated_info_p,
                            (struct pbtools_repeated_message_t *)repeated_p,
                            sizeof(uint64_t),
                            (decoder_read_repeated_t)read_repeated_fixed64);
}

int pbtools_alloc_repeated_sfixed32(struct pbtools_message_base_t *self_p,
                                    int length,
                                    struct pbtools_repeated_int32_t *repeated_p)
{
    return (alloc_repeated_int32(self_p, length, repeated_p));
}

void pbtools_repeated_info_decode_sfixed32(struct pbtools_repeated_info_t *self_p,
                                           struct pbtools_decoder_t *decoder_p,
                                           int wire_type)
{
    repeated_info_decode_32bit(self_p, decoder_p, wire_type);
}

void pbtools_decoder_decode_repeated_sfixed32(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_int32_t *repeated_p)
{
    decoder_decode_repeated(self_p,
                            repeated_info_p,
                            (struct pbtools_repeated_message_t *)repeated_p,
                            sizeof(int32_t),
                            (decoder_read_repeated_t)read_repeated_sfixed32);
}

int pbtools_alloc_repeated_sfixed64(struct pbtools_message_base_t *self_p,
                                    int length,
                                    struct pbtools_repeated_int64_t *repeated_p)
{
    return (alloc_repeated_int64(self_p, length, repeated_p));
}

void pbtools_repeated_info_decode_sfixed64(struct pbtools_repeated_info_t *self_p,
                                           struct pbtools_decoder_t *decoder_p,
                                           int wire_type)
{
    repeated_info_decode_64bit(self_p, decoder_p, wire_type);
}

void pbtools_decoder_decode_repeated_sfixed64(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_int64_t *repeated_p)
{
    decoder_decode_repeated(self_p,
                            repeated_info_p,
                            (struct pbtools_repeated_message_t *)repeated_p,
                            sizeof(int64_t),
                            (decoder_read_repeated_t)read_repeated_sfixed64);
}

#if PBTOOLS_CONFIG_FLOAT == 1

int pbtools_alloc_repeated_float(struct pbtools_message_base_t *self_p,
                                 int length,
                                 struct pbtools_repeated_float_t *repeated_p)
{
    repeated_p->length = length;

    return (calloc_repeated(self_p,
                            sizeof(*repeated_p->items_p) * (size_t)length,
                            (void **)&repeated_p->items_p));
}

void pbtools_repeated_info_decode_float(struct pbtools_repeated_info_t *self_p,
                                        struct pbtools_decoder_t *decoder_p,
                                        int wire_type)
{
    repeated_info_decode_32bit(self_p, decoder_p, wire_type);
}

void pbtools_decoder_decode_repeated_float(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_float_t *repeated_p)
{
    decoder_decode_repeated(self_p,
                            repeated_info_p,
                            (struct pbtools_repeated_message_t *)repeated_p,
                            sizeof(float),
                            (decoder_read_repeated_t)read_repeated_float);
}

int pbtools_alloc_repeated_double(struct pbtools_message_base_t *self_p,
                                  int length,
                                  struct pbtools_repeated_double_t *repeated_p)
{
    repeated_p->length = length;

    return (calloc_repeated(self_p,
                            sizeof(*repeated_p->items_p) * (size_t)length,
                            (void **)&repeated_p->items_p));
}

void pbtools_repeated_info_decode_double(struct pbtools_repeated_info_t *self_p,
                                         struct pbtools_decoder_t *decoder_p,
                                         int wire_type)
{
    repeated_info_decode_64bit(self_p, decoder_p, wire_type);
}

void pbtools_decoder_decode_repeated_double(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_double_t *repeated_p)
{
    decoder_decode_repeated(self_p,
                            repeated_info_p,
                            (struct pbtools_repeated_message_t *)repeated_p,
                            sizeof(double),
                            (decoder_read_repeated_t)read_repeated_double);
}

#endif

int pbtools_alloc_repeated_bool(struct pbtools_message_base_t *self_p,
                                int length,
                                struct pbtools_repeated_bool_t *repeated_p)
{
    repeated_p->length = length;

    return (calloc_repeated(self_p,
                            sizeof(*repeated_p->items_p) * (size_t)length,
                            (void **)&repeated_p->items_p));
}

void pbtools_repeated_info_decode_bool(struct pbtools_repeated_info_t *self_p,
                                       struct pbtools_decoder_t *decoder_p,
                                       int wire_type)
{
    repeated_info_decode_varint(self_p, decoder_p, wire_type);
}

void pbtools_decoder_decode_repeated_bool(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_bool_t *repeated_p)
{
    decoder_decode_repeated(self_p,
                            repeated_info_p,
                            (struct pbtools_repeated_message_t *)repeated_p,
                            sizeof(bool),
                            (decoder_read_repeated_t)read_repeated_bool);
}

int pbtools_alloc_repeated_string(struct pbtools_message_base_t *self_p,
                                  int length,
                                  struct pbtools_repeated_string_t *repeated_p)
{
    int pos;

    repeated_p->items_pp = heap_alloc(self_p->heap_p,
                                      sizeof(*repeated_p->items_pp) * (size_t)length,
                                      alignof(*repeated_p->items_pp));

    if (repeated_p->items_pp == NULL) {
        return (-1);
    }

    for (pos = 0; pos < length; pos++) {
        repeated_p->items_pp[pos] = "";
    }

    repeated_p->length = length;

    return (0);
}

void pbtools_repeated_info_decode_string(struct pbtools_repeated_info_t *self_p,
                                         struct pbtools_decoder_t *decoder_p,
                                         int wire_type)
{
    repeated_info_decode_length_delimited(self_p, decoder_p, wire_type);
}

void pbtools_decoder_decode_repeated_string(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_string_t *repeated_p)
{
    decoder_decode_repeated(self_p,
                            repeated_info_p,
                            (struct pbtools_repeated_message_t *)repeated_p,
                            sizeof(char *),
                            (decoder_read_repeated_t)read_repeated_string);
}

int pbtools_alloc_repeated_bytes(struct pbtools_message_base_t *self_p,
                                 int length,
                                 struct pbtools_repeated_bytes_t *repeated_p)
{
    repeated_p->length = length;

    return (calloc_repeated(self_p,
                            sizeof(*repeated_p->items_p) * (size_t)length,
                            (void **)&repeated_p->items_p));
}

void pbtools_repeated_info_decode_bytes(struct pbtools_repeated_info_t *self_p,
                                        struct pbtools_decoder_t *decoder_p,
                                        int wire_type)
{
    repeated_info_decode_length_delimited(self_p, decoder_p, wire_type);
}

void pbtools_decoder_decode_repeated_bytes(
    struct pbtools_decoder_t *self_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_repeated_bytes_t *repeated_p)
{
    decoder_decode_repeated(self_p,
                            repeated_info_p,
                            (struct pbtools_repeated_message_t *)repeated_p,
                            sizeof(*repeated_p->items_p),
                            (decoder_read_repeated_t)read_repeated_bytes);
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
    if (self_p->pos < 0) {
        return;
    }

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

void pbtools_decoder_skip_field(struct pbtools_decoder_t *self_p,
                                int wire_type)
{
    uint64_t value;

    switch (wire_type) {

    case PBTOOLS_WIRE_TYPE_VARINT:
        (void)decoder_read_varint(self_p);
        break;

    case PBTOOLS_WIRE_TYPE_64_BIT:
        (void)pbtools_decoder_read_fixed64(self_p, PBTOOLS_WIRE_TYPE_64_BIT);
        break;

    case PBTOOLS_WIRE_TYPE_LENGTH_DELIMITED:
        value = decoder_read_length_delimited(self_p, wire_type);
        decoder_seek(self_p, (int)(int64_t)value);
        break;

    case PBTOOLS_WIRE_TYPE_32_BIT:
        (void)pbtools_decoder_read_fixed32(self_p, PBTOOLS_WIRE_TYPE_32_BIT);
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

    self_p = heap_alloc(heap_p,
                        message_size,
                        alignof(struct pbtools_message_base_t));

    if (self_p != NULL) {
        message_init(self_p, heap_p);
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
    char *item_p;

    repeated_p->items_p = heap_alloc(heap_p,
                                     item_size * (size_t)length,
                                     alignof(*repeated_p->items_p));

    if (repeated_p->items_p == NULL) {
        return (-1);
    }

    item_p = repeated_p->items_p;

    for (i = 0; i < length; i++) {
        message_init(item_p, heap_p);
        item_p += item_size;
    }

    repeated_p->length = length;

    return (0);
}

void pbtools_encode_repeated_inner(
    struct pbtools_encoder_t *encoder_p,
    int field_number,
    struct pbtools_repeated_message_t *repeated_p,
    size_t item_size,
    pbtools_message_encode_inner_t message_encode_inner)
{
    int i;
    int pos;
    char *item_p;

    item_p = repeated_p->items_p;
    item_p += ((size_t)repeated_p->length * item_size);

    for (i = 0; i < repeated_p->length; i++) {
        item_p -= item_size;
        pos = encoder_p->pos;
        message_encode_inner(encoder_p, item_p);
        encoder_write_length_delimited(encoder_p,
                                       field_number,
                                       (uint64_t)(pos - encoder_p->pos));
    }
}

void pbtools_decode_repeated_inner(
    struct pbtools_repeated_info_t *repeated_info_p,
    struct pbtools_decoder_t *decoder_p,
    struct pbtools_repeated_message_t *repeated_p,
    size_t item_size,
    pbtools_message_init_t message_init,
    pbtools_message_decode_inner_t message_decode_inner)
{
    int res;
    int wire_type;
    int tag;
    int length;
    int size;
    char *item_p;
    struct pbtools_decoder_t decoder;

    res = pbtools_alloc_repeated(repeated_p,
                                 repeated_info_p->length,
                                 repeated_info_p->decoder.heap_p,
                                 item_size,
                                 message_init);

    if (res != 0) {
        return;
    }

    length = 0;
    item_p = repeated_p->items_p;

    while (are_more_items_to_decode(repeated_info_p, length)) {
        tag = pbtools_decoder_read_tag(&repeated_info_p->decoder, &wire_type);

        if (tag == repeated_info_p->tag) {
            size = (int)decoder_read_length_delimited(&repeated_info_p->decoder,
                                                      wire_type);
            decoder_init_slice(&decoder, &repeated_info_p->decoder, size);
            message_decode_inner(&decoder, item_p);
            decoder_seek(&repeated_info_p->decoder, decoder_get_result(&decoder));
            item_p += item_size;
            length++;
        } else {
            pbtools_decoder_skip_field(&repeated_info_p->decoder, wire_type);
        }
    }

    res = decoder_get_result(&repeated_info_p->decoder);

    if (res < 0) {
        decoder_abort(decoder_p, -res);
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

void pbtools_repeated_info_init(struct pbtools_repeated_info_t *self_p,
                                int tag,
                                struct pbtools_decoder_t *decoder_p)
{
    decoder_init_slice(&self_p->decoder, decoder_p, decoder_left(decoder_p));
    self_p->length = 0;
    self_p->tag = tag;
}

void pbtools_repeated_info_decode(struct pbtools_repeated_info_t *self_p,
                                  struct pbtools_decoder_t *decoder_p,
                                  int wire_type)
{
    int size;

    size = (int)decoder_read_length_delimited(decoder_p, wire_type);
    decoder_seek(decoder_p, size);
    self_p->length++;
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
