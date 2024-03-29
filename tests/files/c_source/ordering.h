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

#ifndef ORDERING_H
#define ORDERING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pbtools.h"

/**
 * Enum ordering.Fam.
 */
enum ordering_fam_e {
    ordering_a_e = 0
};

/**
 * Enum ordering.Bar.Fum.
 */
enum ordering_bar_fum_e {
    ordering_bar_a_e = 0
};

/**
 * Message ordering.Bar.Fie.
 */
struct ordering_bar_fie_repeated_t {
    int length;
    struct ordering_bar_fie_t *items_p;
};

struct ordering_bar_fie_t {
    struct pbtools_message_base_t base;
    bool v1;
    enum ordering_fam_e v2;
};

/**
 * Message ordering.Bar.Gom.
 */
struct ordering_bar_gom_repeated_t {
    int length;
    struct ordering_bar_gom_t *items_p;
};

struct ordering_bar_gom_t {
    struct pbtools_message_base_t base;
    struct ordering_bar_fie_t *v1_p;
};

/**
 * Message ordering.Bar.
 */
struct ordering_bar_repeated_t {
    int length;
    struct ordering_bar_t *items_p;
};

struct ordering_bar_t {
    struct pbtools_message_base_t base;
    bool value;
    struct ordering_bar_fie_t *fie_p;
    enum ordering_bar_fum_e fum;
    struct ordering_bar_gom_t *gom_p;
};

/**
 * Message ordering.Foo.
 */
struct ordering_foo_repeated_t {
    int length;
    struct ordering_foo_t *items_p;
};

struct ordering_foo_t {
    struct pbtools_message_base_t base;
    struct ordering_bar_t *bar_p;
    enum ordering_fam_e fam;
};

int ordering_bar_fie_alloc(
    struct ordering_bar_t *self_p);

int ordering_bar_gom_alloc(
    struct ordering_bar_t *self_p);

int ordering_bar_gom_v1_alloc(
    struct ordering_bar_gom_t *self_p);

/**
 * Encoding and decoding of ordering.Bar.
 */
struct ordering_bar_t *
ordering_bar_new(
    void *workspace_p,
    size_t size);

int ordering_bar_encode(
    struct ordering_bar_t *self_p,
    uint8_t *encoded_p,
    size_t size);

int ordering_bar_decode(
    struct ordering_bar_t *self_p,
    const uint8_t *encoded_p,
    size_t size);

int ordering_foo_bar_alloc(
    struct ordering_foo_t *self_p);

/**
 * Encoding and decoding of ordering.Foo.
 */
struct ordering_foo_t *
ordering_foo_new(
    void *workspace_p,
    size_t size);

int ordering_foo_encode(
    struct ordering_foo_t *self_p,
    uint8_t *encoded_p,
    size_t size);

int ordering_foo_decode(
    struct ordering_foo_t *self_p,
    const uint8_t *encoded_p,
    size_t size);

/* Internal functions. Do not use! */

void ordering_bar_init(
    struct ordering_bar_t *self_p,
    struct pbtools_heap_t *heap_p);

void ordering_bar_encode_inner(
    struct pbtools_encoder_t *encoder_p,
    struct ordering_bar_t *self_p);

void ordering_bar_decode_inner(
    struct pbtools_decoder_t *decoder_p,
    struct ordering_bar_t *self_p);

void ordering_bar_encode_repeated_inner(
    struct pbtools_encoder_t *encoder_p,
    int field_number,
    struct ordering_bar_repeated_t *repeated_p);

void ordering_bar_decode_repeated_inner(
    struct pbtools_decoder_t *decoder_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct ordering_bar_repeated_t *repeated_p);

void ordering_bar_fie_init(
    struct ordering_bar_fie_t *self_p,
    struct pbtools_heap_t *heap_p);

void ordering_bar_fie_encode_inner(
    struct pbtools_encoder_t *encoder_p,
    struct ordering_bar_fie_t *self_p);

void ordering_bar_fie_decode_inner(
    struct pbtools_decoder_t *decoder_p,
    struct ordering_bar_fie_t *self_p);

void ordering_bar_fie_encode_repeated_inner(
    struct pbtools_encoder_t *encoder_p,
    int field_number,
    struct ordering_bar_fie_repeated_t *repeated_p);

void ordering_bar_fie_decode_repeated_inner(
    struct pbtools_decoder_t *decoder_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct ordering_bar_fie_repeated_t *repeated_p);

void ordering_bar_gom_init(
    struct ordering_bar_gom_t *self_p,
    struct pbtools_heap_t *heap_p);

void ordering_bar_gom_encode_inner(
    struct pbtools_encoder_t *encoder_p,
    struct ordering_bar_gom_t *self_p);

void ordering_bar_gom_decode_inner(
    struct pbtools_decoder_t *decoder_p,
    struct ordering_bar_gom_t *self_p);

void ordering_bar_gom_encode_repeated_inner(
    struct pbtools_encoder_t *encoder_p,
    int field_number,
    struct ordering_bar_gom_repeated_t *repeated_p);

void ordering_bar_gom_decode_repeated_inner(
    struct pbtools_decoder_t *decoder_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct ordering_bar_gom_repeated_t *repeated_p);

void ordering_foo_init(
    struct ordering_foo_t *self_p,
    struct pbtools_heap_t *heap_p);

void ordering_foo_encode_inner(
    struct pbtools_encoder_t *encoder_p,
    struct ordering_foo_t *self_p);

void ordering_foo_decode_inner(
    struct pbtools_decoder_t *decoder_p,
    struct ordering_foo_t *self_p);

void ordering_foo_encode_repeated_inner(
    struct pbtools_encoder_t *encoder_p,
    int field_number,
    struct ordering_foo_repeated_t *repeated_p);

void ordering_foo_decode_repeated_inner(
    struct pbtools_decoder_t *decoder_p,
    struct pbtools_repeated_info_t *repeated_info_p,
    struct ordering_foo_repeated_t *repeated_p);

#ifdef __cplusplus
}
#endif

#endif
