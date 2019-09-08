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

/**
 * This file was generated by pbtools.
 */

#include "enum.h"

static void enum_message_init(
    struct enum_message_t *self_p,
    struct pbtools_heap_t *heap_p,
    struct enum_message_t *next_p);

static void enum_message_encode_inner(
    struct pbtools_encoder_t *encoder_p,
    struct enum_message_t *self_p);

static void enum_message_decode_inner(
    struct pbtools_decoder_t *decoder_p,
    struct enum_message_t *self_p);

static void enum_message2_init(
    struct enum_message2_t *self_p,
    struct pbtools_heap_t *heap_p,
    struct enum_message2_t *next_p);

static void enum_message2_encode_inner(
    struct pbtools_encoder_t *encoder_p,
    struct enum_message2_t *self_p);

static void enum_message2_decode_inner(
    struct pbtools_decoder_t *decoder_p,
    struct enum_message2_t *self_p);

static void enum_limits_init(
    struct enum_limits_t *self_p,
    struct pbtools_heap_t *heap_p,
    struct enum_limits_t *next_p);

static void enum_limits_encode_inner(
    struct pbtools_encoder_t *encoder_p,
    struct enum_limits_t *self_p);

static void enum_limits_decode_inner(
    struct pbtools_decoder_t *decoder_p,
    struct enum_limits_t *self_p);

static void enum_message_init(
    struct enum_message_t *self_p,
    struct pbtools_heap_t *heap_p,
    struct enum_message_t *next_p)
{
    self_p->base.heap_p = heap_p;
    self_p->base.next_p = &next_p->base;
    self_p->value = 0;
}

static void enum_message_encode_inner(
    struct pbtools_encoder_t *encoder_p,
    struct enum_message_t *self_p)
{
    pbtools_encoder_write_enum(encoder_p, 1, self_p->value);
}

static void enum_message_decode_inner(
    struct pbtools_decoder_t *decoder_p,
    struct enum_message_t *self_p)
{
    int wire_type;

    while (pbtools_decoder_available(decoder_p)) {
        switch (pbtools_decoder_read_tag(decoder_p, &wire_type)) {

        case 1:
            self_p->value = pbtools_decoder_read_enum(decoder_p, wire_type);
            break;

        default:
            pbtools_decoder_skip_field(decoder_p, wire_type);
            break;
        }
    }
}

struct enum_message_t *
enum_message_new(
    void *workspace_p,
    size_t size)
{
    return (pbtools_message_new(
        workspace_p,
        size,
        sizeof(struct enum_message_t),
        (pbtools_message_init_t)enum_message_init));
}

int enum_message_encode(
    struct enum_message_t *self_p,
    uint8_t *encoded_p,
    size_t size)
{
    return (pbtools_message_encode(
        &self_p->base,
        encoded_p,
        size,
        (pbtools_message_encode_inner_t)enum_message_encode_inner));
}

int enum_message_decode(
    struct enum_message_t *self_p,
    const uint8_t *encoded_p,
    size_t size)
{
    return (pbtools_message_decode(
        &self_p->base,
        encoded_p,
        size,
        (pbtools_message_decode_inner_t)enum_message_decode_inner));
}

static void enum_message2_init(
    struct enum_message2_t *self_p,
    struct pbtools_heap_t *heap_p,
    struct enum_message2_t *next_p)
{
    self_p->base.heap_p = heap_p;
    self_p->base.next_p = &next_p->base;
    self_p->outer = 0;
    self_p->inner = 0;
}

static void enum_message2_encode_inner(
    struct pbtools_encoder_t *encoder_p,
    struct enum_message2_t *self_p)
{
    pbtools_encoder_write_enum(encoder_p, 2, self_p->inner);
    pbtools_encoder_write_enum(encoder_p, 1, self_p->outer);
}

static void enum_message2_decode_inner(
    struct pbtools_decoder_t *decoder_p,
    struct enum_message2_t *self_p)
{
    int wire_type;

    while (pbtools_decoder_available(decoder_p)) {
        switch (pbtools_decoder_read_tag(decoder_p, &wire_type)) {

        case 1:
            self_p->outer = pbtools_decoder_read_enum(decoder_p, wire_type);
            break;

        case 2:
            self_p->inner = pbtools_decoder_read_enum(decoder_p, wire_type);
            break;

        default:
            pbtools_decoder_skip_field(decoder_p, wire_type);
            break;
        }
    }
}

struct enum_message2_t *
enum_message2_new(
    void *workspace_p,
    size_t size)
{
    return (pbtools_message_new(
        workspace_p,
        size,
        sizeof(struct enum_message2_t),
        (pbtools_message_init_t)enum_message2_init));
}

int enum_message2_encode(
    struct enum_message2_t *self_p,
    uint8_t *encoded_p,
    size_t size)
{
    return (pbtools_message_encode(
        &self_p->base,
        encoded_p,
        size,
        (pbtools_message_encode_inner_t)enum_message2_encode_inner));
}

int enum_message2_decode(
    struct enum_message2_t *self_p,
    const uint8_t *encoded_p,
    size_t size)
{
    return (pbtools_message_decode(
        &self_p->base,
        encoded_p,
        size,
        (pbtools_message_decode_inner_t)enum_message2_decode_inner));
}

static void enum_limits_init(
    struct enum_limits_t *self_p,
    struct pbtools_heap_t *heap_p,
    struct enum_limits_t *next_p)
{
    self_p->base.heap_p = heap_p;
    self_p->base.next_p = &next_p->base;
    self_p->value = 0;
}

static void enum_limits_encode_inner(
    struct pbtools_encoder_t *encoder_p,
    struct enum_limits_t *self_p)
{
    pbtools_encoder_write_enum(encoder_p, 1, self_p->value);
}

static void enum_limits_decode_inner(
    struct pbtools_decoder_t *decoder_p,
    struct enum_limits_t *self_p)
{
    int wire_type;

    while (pbtools_decoder_available(decoder_p)) {
        switch (pbtools_decoder_read_tag(decoder_p, &wire_type)) {

        case 1:
            self_p->value = pbtools_decoder_read_enum(decoder_p, wire_type);
            break;

        default:
            pbtools_decoder_skip_field(decoder_p, wire_type);
            break;
        }
    }
}

struct enum_limits_t *
enum_limits_new(
    void *workspace_p,
    size_t size)
{
    return (pbtools_message_new(
        workspace_p,
        size,
        sizeof(struct enum_limits_t),
        (pbtools_message_init_t)enum_limits_init));
}

int enum_limits_encode(
    struct enum_limits_t *self_p,
    uint8_t *encoded_p,
    size_t size)
{
    return (pbtools_message_encode(
        &self_p->base,
        encoded_p,
        size,
        (pbtools_message_encode_inner_t)enum_limits_encode_inner));
}

int enum_limits_decode(
    struct enum_limits_t *self_p,
    const uint8_t *encoded_p,
    size_t size)
{
    return (pbtools_message_decode(
        &self_p->base,
        encoded_p,
        size,
        (pbtools_message_decode_inner_t)enum_limits_decode_inner));
}
