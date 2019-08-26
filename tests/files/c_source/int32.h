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
 * This file was generated by pbtools version 0.1.0 Mon Aug 26 06:39:14 2019.
 */

#ifndef INT32_H
#define INT32_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define INT32_BAD_WIRE_TYPE                                   1
#define INT32_OUT_OF_DATA                                     2
#define INT32_OUT_OF_MEMORY                                   3
#define INT32_ENCODE_BUFFER_FULL                              4

struct int32_heap_t {
    char *buf_p;
    int size;
    int pos;
};

/**
 * Message Message in package int32.
 */
struct int32_message_t {
    struct int32_heap_t *heap_p;
    int32_t value;
};

/**
 * Message Message2 in package int32.
 */
struct int32_message2_t {
    struct int32_heap_t *heap_p;
    int32_t value;
};

/**
 * Create a new message Message in given workspace.
 *
 * @param[in] workspace_p Message workspace.
 * @param[in] size Workspace size.
 *
 * @return Initialized address book, or NULL on failure.
 */
struct int32_message_t *int32_message_new(
    void *workspace_p,
    size_t size);

/**
 * Encode message Message defined in package int32.
 *
 * @param[in] self_p Message to encode.
 * @param[out] encoded_p Buffer to encode the message into.
 * @param[in] size Encoded buffer size.
 *
 * @return Encoded data length or negative error code.
 */
int int32_message_encode(
    struct int32_message_t *self_p,
    uint8_t *encoded_p,
    size_t size);

/**
 * Decode message Message defined in package int32.
 *
 * @param[in,out] self_p Initialized message to decode into.
 * @param[in] encoded_p Buffer to decode.
 * @param[in] size Size of the encoded message.
 *
 * @return Number of bytes decoded or negative error code.
 */
int int32_message_decode(
    struct int32_message_t *self_p,
    const uint8_t *encoded_p,
    size_t size);

/**
 * Create a new message Message2 in given workspace.
 *
 * @param[in] workspace_p Message workspace.
 * @param[in] size Workspace size.
 *
 * @return Initialized address book, or NULL on failure.
 */
struct int32_message2_t *int32_message2_new(
    void *workspace_p,
    size_t size);

/**
 * Encode message Message2 defined in package int32.
 *
 * @param[in] self_p Message to encode.
 * @param[out] encoded_p Buffer to encode the message into.
 * @param[in] size Encoded buffer size.
 *
 * @return Encoded data length or negative error code.
 */
int int32_message2_encode(
    struct int32_message2_t *self_p,
    uint8_t *encoded_p,
    size_t size);

/**
 * Decode message Message2 defined in package int32.
 *
 * @param[in,out] self_p Initialized message to decode into.
 * @param[in] encoded_p Buffer to decode.
 * @param[in] size Size of the encoded message.
 *
 * @return Number of bytes decoded or negative error code.
 */
int int32_message2_decode(
    struct int32_message2_t *self_p,
    const uint8_t *encoded_p,
    size_t size);

#endif
