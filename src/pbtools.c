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
#include "pbtools.h"

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
                         size_t size)
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
                                 size_t size)
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
        self_p->size = -error;
        self_p->pos = -error;
    }
}

void pbtools_encoder_put(struct pbtools_encoder_t *self_p,
                         uint8_t value)
{
    if (self_p->pos >= 0) {
        self_p->buf_p[self_p->pos] = value;
        self_p->pos--;
    } else {
        pbtools_encoder_abort(self_p, PBTOOLS_ENCODE_BUFFER_FULL);
    }
}

void pbtools_encoder_write(struct pbtools_encoder_t *self_p,
                           uint8_t *buf_p,
                           int size)
{
    int i;

    for (i = size - 1; i >= 0; i--) {
        pbtools_encoder_put(self_p, buf_p[i]);
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

    if (value == 0) {
        pbtools_encoder_put(self_p, 0);
    } else {
        pos = 0;

        while (value > 0) {
            buf[pos++] = (value | 0x80);
            value >>= 7;
        }

        buf[pos - 1] &= 0x7f;
        pbtools_encoder_write(self_p, &buf[0], pos);
    }
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
    pbtools_encoder_write_tag(self_p, field_number, 2);
}

void pbtools_encoder_write_int32(struct pbtools_encoder_t *self_p,
                                 int field_number,
                                 int32_t value)
{
    pbtools_encoder_write_tagged_varint(self_p,
                                        field_number,
                                        0,
                                        (uint64_t)(int64_t)value);
}

void pbtools_encoder_write_repeated_int32(
    struct pbtools_encoder_t *self_p,
    int field_number,
    struct pbtools_repeated_int32_t *repeated_p)
{
    int i;
    int pos;

    if (repeated_p->length == 0) {
        return;
    }

    pos = self_p->pos;

    for (i = repeated_p->length - 1; i >= 0; i--) {
        pbtools_encoder_write_varint(self_p,
                                     repeated_p->items_pp[i]->value);
    }

    pbtools_encoder_write_tagged_varint(self_p,
                                        field_number,
                                        2,
                                        pos - self_p->pos);
}

void pbtools_encoder_write_int64(struct pbtools_encoder_t *self_p,
                                 int field_number,
                                 int64_t value)
{
    pbtools_encoder_write_tagged_varint(self_p, field_number, 0, (uint64_t)value);
}

void pbtools_encoder_write_sint32(struct pbtools_encoder_t *self_p,
                                  int field_number,
                                  int32_t value)
{
    if (value < 0) {
        pbtools_encoder_write_tagged_varint(self_p,
                                            field_number,
                                            0,
                                            ~((uint64_t)value << 1));
    } else {
        pbtools_encoder_write_tagged_varint(self_p,
                                            field_number,
                                            0,
                                            (uint64_t)value << 1);
    }
}

void pbtools_encoder_write_sint64(struct pbtools_encoder_t *self_p,
                                  int field_number,
                                  int64_t value)
{
    if (value < 0) {
        pbtools_encoder_write_tagged_varint(self_p,
                                            field_number,
                                            0,
                                            ~((uint64_t)value << 1));
    } else {
        pbtools_encoder_write_tagged_varint(self_p,
                                            field_number,
                                            0,
                                            (uint64_t)value << 1);
    }
}

void pbtools_encoder_write_uint32(struct pbtools_encoder_t *self_p,
                                  int field_number,
                                  uint32_t value)
{
    pbtools_encoder_write_tagged_varint(self_p,
                                        field_number,
                                        0,
                                        (uint64_t)value);
}

void pbtools_encoder_write_uint64(struct pbtools_encoder_t *self_p,
                                  int field_number,
                                  uint64_t value)
{
    pbtools_encoder_write_tagged_varint(self_p, field_number, 0, value);
}

void pbtools_encoder_write_fixed32(struct pbtools_encoder_t *self_p,
                                   int field_number,
                                   uint32_t value)
{
    uint8_t buf[4];

    if (value > 0) {
        buf[0] = (value & 0xff);
        buf[1] = ((value >> 8) & 0xff);
        buf[2] = ((value >> 16) & 0xff);
        buf[3] = ((value >> 24) & 0xff);
        pbtools_encoder_write(self_p, &buf[0], 4);
        pbtools_encoder_write_tag(self_p, field_number, 5);
    }
}

void pbtools_encoder_write_fixed64(struct pbtools_encoder_t *self_p,
                                   int field_number,
                                   uint64_t value)
{
    uint8_t buf[8];

    if (value > 0) {
        buf[0] = (value & 0xff);
        buf[1] = ((value >> 8) & 0xff);
        buf[2] = ((value >> 16) & 0xff);
        buf[3] = ((value >> 24) & 0xff);
        buf[4] = ((value >> 32) & 0xff);
        buf[5] = ((value >> 40) & 0xff);
        buf[6] = ((value >> 48) & 0xff);
        buf[7] = ((value >> 56) & 0xff);
        pbtools_encoder_write(self_p, &buf[0], 8);
        pbtools_encoder_write_tag(self_p, field_number, 1);
    }
}

void pbtools_encoder_write_sfixed32(struct pbtools_encoder_t *self_p,
                                    int field_number,
                                    int32_t value)
{
    uint8_t buf[4];

    if (value != 0) {
        buf[0] = (value & 0xff);
        buf[1] = ((value >> 8) & 0xff);
        buf[2] = ((value >> 16) & 0xff);
        buf[3] = ((value >> 24) & 0xff);
        pbtools_encoder_write(self_p, &buf[0], 4);
        pbtools_encoder_write_tag(self_p, field_number, 5);
    }
}

void pbtools_encoder_write_sfixed64(struct pbtools_encoder_t *self_p,
                                    int field_number,
                                    int64_t value)
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
        pbtools_encoder_write_tag(self_p, field_number, 1);
    }
}

void pbtools_encoder_write_float(struct pbtools_encoder_t *self_p,
                                 int field_number,
                                 float value)
{
    uint32_t data;

    if (value == 0) {
        return;
    }

    memcpy(&data, &value, sizeof(data));
    pbtools_encoder_put(self_p, data >> 24);
    pbtools_encoder_put(self_p, data >> 16);
    pbtools_encoder_put(self_p, data >> 8);
    pbtools_encoder_put(self_p, data >> 0);
    pbtools_encoder_write_tag(self_p, field_number, 5);
}

void pbtools_encoder_write_double(struct pbtools_encoder_t *self_p,
                                  int field_number,
                                  double value)
{
    uint64_t data;

    if (value == 0) {
        return;
    }

    memcpy(&data, &value, sizeof(data));
    pbtools_encoder_put(self_p, data >> 56);
    pbtools_encoder_put(self_p, data >> 48);
    pbtools_encoder_put(self_p, data >> 40);
    pbtools_encoder_put(self_p, data >> 32);
    pbtools_encoder_put(self_p, data >> 24);
    pbtools_encoder_put(self_p, data >> 16);
    pbtools_encoder_put(self_p, data >> 8);
    pbtools_encoder_put(self_p, data >> 0);
    pbtools_encoder_write_tag(self_p, field_number, 1);
}

void pbtools_encoder_write_bool(struct pbtools_encoder_t *self_p,
                                int field_number,
                                bool value)
{
    if (value) {
        pbtools_encoder_put(self_p, 1);
        pbtools_encoder_write_tag(self_p, field_number, 0);
    }
}

void pbtools_encoder_write_enum(struct pbtools_encoder_t *self_p,
                                int field_number,
                                int value)
{
    pbtools_encoder_write_tagged_varint(self_p, field_number, 0, value);
}

void pbtools_encoder_write_string(struct pbtools_encoder_t *self_p,
                                  int field_number,
                                  struct pbtools_bytes_t *string_p)
{
    if (self_p->size > 0) {
        pbtools_encoder_write(self_p, string_p->buf_p, string_p->size);
        pbtools_encoder_write_tagged_varint(self_p,
                                            field_number,
                                            2,
                                            string_p->size);
    }
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
        pbtools_encoder_write_bytes(self_p,
                                    field_number,
                                    repeated_p->items_pp[i]);
    }
}

void pbtools_encoder_write_bytes(struct pbtools_encoder_t *self_p,
                                 int field_number,
                                 struct pbtools_bytes_t *value_p)
{
    if (value_p->size > 0) {
        pbtools_encoder_write(self_p, value_p->buf_p, value_p->size);
        pbtools_encoder_write_tagged_varint(self_p, field_number, 2, value_p->size);
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
    int res;

    if (self_p->pos == self_p->size) {
        res = self_p->pos;
    } else {
        res = -1;
    }

    return (res);
}

void pbtools_decoder_abort(struct pbtools_decoder_t *self_p,
                           int error)
{
    if (self_p->size >= 0) {
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
                          int size)
{
    int i;

    for (i = 0; i < size; i++) {
        buf_p[i] = pbtools_decoder_get(self_p);
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

    PRINTF("v: %llu\n", (unsigned long long)value);

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
    return (pbtools_decoder_read_varint_check_wire_type(self_p, wire_type, 0));
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
    return (pbtools_decoder_read_varint_check_wire_type_varint(self_p,
                                                               wire_type));
}

int pbtools_alloc_repeated_int32(struct pbtools_repeated_int32_t *repeated_p,
                                 struct pbtools_heap_t *heap_p,
                                 int length)
{
    int res;
    int i;
    struct pbtools_int32_t *items_p;

    res = -1;
    repeated_p->items_pp = pbtools_heap_alloc(
        heap_p,
        sizeof(items_p) * length);

    if (repeated_p->items_pp != NULL) {
        items_p = pbtools_heap_alloc(heap_p, sizeof(*items_p) * length);

        if (items_p != NULL) {
            for (i = 0; i < length; i++) {
                items_p[i].value = 0;
                items_p[i].next_p = &items_p[i + 1];
                repeated_p->items_pp[i] = &items_p[i];
            }

            items_p[length - 1].next_p = NULL;
            repeated_p->length = length;
            repeated_p->head_p = &items_p[0];
            repeated_p->tail_p = &items_p[length - 1];
            res = 0;
        }
    }

    return (res);
}

void pbtools_decoder_read_repeated_int32(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_int32_t *repeated_p)
{
    size_t size;
    int pos;
    struct pbtools_int32_t *item_p;

    size = pbtools_decoder_read_varint_check_wire_type(self_p, wire_type, 2);
    pos = self_p->pos;

    while (self_p->pos < pos + size) {
        item_p = pbtools_decoder_heap_alloc(self_p, sizeof(*item_p));

        if (item_p == NULL) {
            return;
        }

        item_p->value = pbtools_decoder_read_varint(self_p);
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

int64_t pbtools_decoder_read_int64(struct pbtools_decoder_t *self_p,
                                   int wire_type)
{
    return (pbtools_decoder_read_varint_check_wire_type_varint(self_p,
                                                               wire_type));
}

int32_t pbtools_decoder_read_sint32(struct pbtools_decoder_t *self_p,
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
    return (pbtools_decoder_read_varint_check_wire_type_varint(self_p,
                                                               wire_type));
}

uint64_t pbtools_decoder_read_uint64(struct pbtools_decoder_t *self_p,
                                     int wire_type)
{
    return (pbtools_decoder_read_varint_check_wire_type_varint(self_p,
                                                               wire_type));
}

uint32_t pbtools_decoder_read_fixed32(struct pbtools_decoder_t *self_p,
                                      int wire_type)
{
    uint32_t value;

    value = pbtools_decoder_get(self_p);
    value |= (pbtools_decoder_get(self_p) << 8);
    value |= (pbtools_decoder_get(self_p) << 16);
    value |= (pbtools_decoder_get(self_p) << 24);

    return (value);
}

uint64_t pbtools_decoder_read_fixed64(struct pbtools_decoder_t *self_p,
                                      int wire_type)
{
    uint64_t value;

    value = pbtools_decoder_get(self_p);
    value |= ((uint64_t)pbtools_decoder_get(self_p) << 8);
    value |= ((uint64_t)pbtools_decoder_get(self_p) << 16);
    value |= ((uint64_t)pbtools_decoder_get(self_p) << 24);
    value |= ((uint64_t)pbtools_decoder_get(self_p) << 32);
    value |= ((uint64_t)pbtools_decoder_get(self_p) << 40);
    value |= ((uint64_t)pbtools_decoder_get(self_p) << 48);
    value |= ((uint64_t)pbtools_decoder_get(self_p) << 56);

    return (value);
}

int32_t pbtools_decoder_read_sfixed32(struct pbtools_decoder_t *self_p,
                                      int wire_type)
{
    uint32_t value;

    value = pbtools_decoder_get(self_p);
    value |= (pbtools_decoder_get(self_p) << 8);
    value |= (pbtools_decoder_get(self_p) << 16);
    value |= (pbtools_decoder_get(self_p) << 24);

    return (value);
}

int64_t pbtools_decoder_read_sfixed64(struct pbtools_decoder_t *self_p,
                                      int wire_type)
{
    uint64_t value;

    value = pbtools_decoder_get(self_p);
    value |= ((uint64_t)pbtools_decoder_get(self_p) << 8);
    value |= ((uint64_t)pbtools_decoder_get(self_p) << 16);
    value |= ((uint64_t)pbtools_decoder_get(self_p) << 24);
    value |= ((uint64_t)pbtools_decoder_get(self_p) << 32);
    value |= ((uint64_t)pbtools_decoder_get(self_p) << 40);
    value |= ((uint64_t)pbtools_decoder_get(self_p) << 48);
    value |= ((uint64_t)pbtools_decoder_get(self_p) << 56);

    return (value);
}

float pbtools_decoder_read_float(struct pbtools_decoder_t *self_p,
                                 int wire_type)
{
    uint32_t data;
    float value;

    if (wire_type != 5) {
        pbtools_decoder_abort(self_p, PBTOOLS_BAD_WIRE_TYPE);

        return (0.0);
    }

    data = pbtools_decoder_get(self_p);
    data |= (pbtools_decoder_get(self_p) << 8);
    data |= (pbtools_decoder_get(self_p) << 16);
    data |= (pbtools_decoder_get(self_p) << 24);
    memcpy(&value, &data, sizeof(value));

    return (value);
}

double pbtools_decoder_read_double(struct pbtools_decoder_t *self_p,
                                   int wire_type)
{
    uint64_t data;
    double value;

    if (wire_type != 1) {
        pbtools_decoder_abort(self_p, PBTOOLS_BAD_WIRE_TYPE);

        return (0.0);
    }

    data = pbtools_decoder_get(self_p);
    data |= ((uint64_t)pbtools_decoder_get(self_p) << 8);
    data |= ((uint64_t)pbtools_decoder_get(self_p) << 16);
    data |= ((uint64_t)pbtools_decoder_get(self_p) << 24);
    data |= ((uint64_t)pbtools_decoder_get(self_p) << 32);
    data |= ((uint64_t)pbtools_decoder_get(self_p) << 40);
    data |= ((uint64_t)pbtools_decoder_get(self_p) << 48);
    data |= ((uint64_t)pbtools_decoder_get(self_p) << 56);
    memcpy(&value, &data, sizeof(value));

    return (value);
}

bool pbtools_decoder_read_bool(struct pbtools_decoder_t *self_p,
                               int wire_type)
{
    if (wire_type != 0) {
        pbtools_decoder_abort(self_p, PBTOOLS_BAD_WIRE_TYPE);

        return (false);
    }

    return (pbtools_decoder_get(self_p) == 1);
}

int pbtools_decoder_read_enum(struct pbtools_decoder_t *self_p,
                              int wire_type)
{
    return (pbtools_decoder_read_varint_check_wire_type_varint(self_p,
                                                               wire_type));
}

void pbtools_decoder_read_string(struct pbtools_decoder_t *self_p,
                                 int wire_type,
                                 struct pbtools_bytes_t *string_p)
{
    if (wire_type != 2) {
        pbtools_decoder_abort(self_p, PBTOOLS_BAD_WIRE_TYPE);

        return;
    }

    string_p->size = pbtools_decoder_read_varint(self_p);
    string_p->buf_p = pbtools_decoder_heap_alloc(self_p, string_p->size + 1);

    if (string_p->buf_p == NULL) {
        return;
    }

    pbtools_decoder_read(self_p, string_p->buf_p, string_p->size);
    string_p->buf_p[string_p->size] = '\0';
}

int pbtools_alloc_repeated_string(struct pbtools_repeated_string_t *repeated_p,
                                  struct pbtools_heap_t *heap_p,
                                  int length)
{
    int res;
    int i;
    struct pbtools_bytes_t *items_p;

    res = -1;
    repeated_p->items_pp = pbtools_heap_alloc(
        heap_p,
        sizeof(items_p) * length);

    if (repeated_p->items_pp != NULL) {
        items_p = pbtools_heap_alloc(heap_p, sizeof(*items_p) * length);

        if (items_p != NULL) {
            for (i = 0; i < length; i++) {
                items_p[i].size = 0;
                items_p[i].next_p = &items_p[i + 1];
                repeated_p->items_pp[i] = &items_p[i];
            }

            items_p[length - 1].next_p = NULL;
            repeated_p->length = length;
            repeated_p->head_p = &items_p[0];
            repeated_p->tail_p = &items_p[length - 1];
            res = 0;
        }
    }

    return (res);
}

void pbtools_decoder_read_repeated_string(
    struct pbtools_decoder_t *self_p,
    int wire_type,
    struct pbtools_repeated_string_t *repeated_p)
{
    struct pbtools_bytes_t *item_p;

    item_p = pbtools_decoder_heap_alloc(self_p, sizeof(*item_p));

    if (item_p == NULL) {
        return;
    }

    pbtools_decoder_read_string(self_p, wire_type, item_p);

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

void pbtools_decoder_read_bytes(struct pbtools_decoder_t *self_p,
                                int wire_type,
                                struct pbtools_bytes_t *bytes_p)
{
    if (wire_type != 2) {
        pbtools_decoder_abort(self_p, PBTOOLS_BAD_WIRE_TYPE);

        return;
    }

    bytes_p->size = pbtools_decoder_read_varint(self_p);
    bytes_p->buf_p = pbtools_decoder_heap_alloc(self_p, bytes_p->size);

    if (bytes_p->buf_p == NULL) {
        return;
    }

    pbtools_decoder_read(self_p, bytes_p->buf_p, bytes_p->size);
}

int pbtools_alloc_repeated_bytes(struct pbtools_repeated_bytes_t *repeated_p,
                                 struct pbtools_heap_t *heap_p,
                                 int length)
{
    int res;
    int i;
    struct pbtools_bytes_t *items_p;

    res = -1;
    repeated_p->items_pp = pbtools_heap_alloc(
        heap_p,
        sizeof(items_p) * length);

    if (repeated_p->items_pp != NULL) {
        items_p = pbtools_heap_alloc(heap_p, sizeof(*items_p) * length);

        if (items_p != NULL) {
            for (i = 0; i < length; i++) {
                items_p[i].size = 0;
                items_p[i].next_p = &items_p[i + 1];
                repeated_p->items_pp[i] = &items_p[i];
            }

            items_p[length - 1].next_p = NULL;
            repeated_p->length = length;
            repeated_p->head_p = &items_p[0];
            repeated_p->tail_p = &items_p[length - 1];
            res = 0;
        }
    }

    return (res);
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

    if (size <= parent_p->size - parent_p->pos) {
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
    if (self_p->pos >= 0) {
        if (((unsigned int)self_p->pos + (unsigned int)offset) > INT_MAX) {
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

    case 0:
        (void)pbtools_decoder_read_varint(self_p);
        break;

    case 1:
        (void)pbtools_decoder_read_fixed64(self_p, 1);
        break;

    case 2:
        value = pbtools_decoder_read_varint(self_p);

        if (value > INT_MAX) {
            pbtools_decoder_abort(self_p, PBTOOLS_LENGTH_DELIMITED_OVERFLOW);
        } else {
            pbtools_decoder_seek(self_p, (int)(int64_t)value);
        }
        break;

    case 5:
        (void)pbtools_decoder_read_fixed32(self_p, 5);
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
