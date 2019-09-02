#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "narwhal.h"
#include "files/c_source/int32.h"
#include "files/c_source/int64.h"
#include "files/c_source/sint32.h"
#include "files/c_source/sint64.h"
#include "files/c_source/uint32.h"
#include "files/c_source/uint64.h"
#include "files/c_source/fixed32.h"
#include "files/c_source/fixed64.h"
#include "files/c_source/sfixed32.h"
#include "files/c_source/sfixed64.h"
#include "files/c_source/float.h"
#include "files/c_source/double.h"
#include "files/c_source/bool.h"
#include "files/c_source/string.h"
#include "files/c_source/bytes.h"
#include "files/c_source/enum.h"
#include "files/c_source/address_book.h"
#include "files/c_source/tags.h"
#include "files/c_source/oneof.h"
#include "files/c_source/repeated.h"

#define membersof(a) (sizeof(a) / sizeof((a)[0]))

TEST(int32)
{
    int i;
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct int32_message_t *message_p;
    struct {
        int32_t decoded;
        int size;
        const char *encoded_p;
    } datas[] = {
        { -0x80000000, 11, "\x08\x80\x80\x80\x80\xf8\xff\xff\xff\xff\x01" },
        { -0x2,        11, "\x08\xfe\xff\xff\xff\xff\xff\xff\xff\xff\x01" },
        { -0x1,        11, "\x08\xff\xff\xff\xff\xff\xff\xff\xff\xff\x01" },
        { 0x0,         0, "" },
        { 0x1,         2, "\x08\x01" },
        { 0x7f,        2, "\x08\x7f" },
        { 0x80,        3, "\x08\x80\x01" },
        { 0x3fff,      3, "\x08\xff\x7f" },
        { 0x4000,      4, "\x08\x80\x80\x01" },
        { 0x1fffff,    4, "\x08\xff\xff\x7f" },
        { 0x200000,    5, "\x08\x80\x80\x80\x01" },
        { 0xfffffff,   5, "\x08\xff\xff\xff\x7f" },
        { 0x10000000,  6, "\x08\x80\x80\x80\x80\x01" },
        { 0x7fffffff,  6, "\x08\xff\xff\xff\xff\x07" }
    };

    /* Default. */
    message_p = int32_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = int32_message_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 0);

    for (i = 0; i < membersof(datas); i++) {
        printf("int32: %d\n", datas[i].decoded);

        message_p = int32_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        message_p->value = datas[i].decoded;
        size = int32_message_encode(message_p, &encoded[0], sizeof(encoded));
        ASSERT_EQ(size, datas[i].size);
        ASSERT_MEMORY(&encoded[0], datas[i].encoded_p, size);

        message_p = int32_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        size = int32_message_decode(message_p, &encoded[0], size);
        ASSERT_EQ(size, datas[i].size);
        ASSERT_EQ(message_p->value, datas[i].decoded);
    }
}

TEST(int32_decode)
{
    int i;
    int size;
    uint8_t workspace[512];
    struct int32_message_t *message_p;
    struct {
        int32_t decoded;
        int size;
        const char *encoded_p;
    } datas[] = {
        // Python decoder behaviour.
        { 0x0,        11, "\x08\x80\x80\x80\x80\xf0\xff\xff\xff\xff\x01" },
        { -0x1,        6, "\x08\xff\xff\xff\xff\x0f" },
        { -0x1,       11, "\x08\xff\xff\xff\xff\xff\xff\xff\xff\xff\x03" }
    };

    for (i = 0; i < membersof(datas); i++) {
        printf("int32: %d\n", datas[i].decoded);

        message_p = int32_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        size = int32_message_decode(message_p,
                                    (uint8_t *)datas[i].encoded_p,
                                    datas[i].size);
        ASSERT_EQ(size, datas[i].size);
        ASSERT_EQ(message_p->value, datas[i].decoded);
    }
}

TEST(int32_message_2)
{
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct int32_message2_t *message_p;
    uint8_t *encoded_p = (uint8_t *)"\x80\x01\x01";

    message_p = int32_message2_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    message_p->value = 1;
    size = int32_message2_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 3);
    ASSERT_MEMORY(&encoded[0], encoded_p, size);

    message_p = int32_message2_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = int32_message2_decode(message_p, &encoded[0], size);
    ASSERT_EQ(size, 3);
    ASSERT_EQ(message_p->value, 1);
}

TEST(int32_decode_out_of_data)
{
    int size;
    uint8_t workspace[512];
    struct int32_message_t *message_p;

    message_p = int32_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = int32_message_decode(message_p, (uint8_t *)"\x08", 1);
    ASSERT_EQ(size, -PBTOOLS_OUT_OF_DATA);

    message_p = int32_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = int32_message_decode(message_p, (uint8_t *)"\x08\x80", 2);
    ASSERT_EQ(size, -PBTOOLS_OUT_OF_DATA);
}

TEST(int32_decode_unknown_field_number)
{
    int size;
    uint8_t workspace[512];
    struct int32_message_t *message_p;

    message_p = int32_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = int32_message_decode(message_p, (uint8_t *)"\x10\x01", 2);
    ASSERT_EQ(size, 2);
    ASSERT_EQ(message_p->value, 0);

    message_p = int32_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = int32_message_decode(message_p,
                                (uint8_t *)"\x10\x01\x08\x44",
                                4);
    ASSERT_EQ(size, 4);
    ASSERT_EQ(message_p->value, 68);
}

TEST(int32_decode_duplicated_field_number)
{
    int size;
    uint8_t workspace[512];
    struct int32_message_t *message_p;

    message_p = int32_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = int32_message_decode(message_p,
                                (uint8_t *)"\x08\x01\x08\x07",
                                4);
    ASSERT_EQ(size, 4);
    ASSERT_EQ(message_p->value, 7);
}

TEST(int32_decode_oveflow)
{
    int i;
    int size;
    uint8_t workspace[512];
    struct int32_message_t *message_p;
    struct {
        int size;
        const char *encoded_p;
    } datas[] = {
        { 11, "\x08\xff\xff\xff\xff\xff\xff\xff\xff\xff\x80\x00" }
    };

    for (i = 0; i < membersof(datas); i++) {
        printf("int32: %d\n", i);

        message_p = int32_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        size = int32_message_decode(message_p,
                                    (uint8_t *)datas[i].encoded_p,
                                    datas[i].size);
        ASSERT_EQ(size, -PBTOOLS_VARINT_OVERFLOW);
    }
}

TEST(int32_decode_seek_out_of_data)
{
    int size;
    uint8_t workspace[512];
    struct int32_message_t *message_p;

    /* Ok. */
    message_p = int32_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = int32_message_decode(message_p,
                                (uint8_t *)"\x72\x03\x11\x22\x33",
                                5);
    ASSERT_EQ(size, 5);
    ASSERT_EQ(message_p->value, 0);

    /* Not ok. */
    message_p = int32_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = int32_message_decode(message_p,
                                (uint8_t *)"\x72\x04\x11\x22\x33",
                                5);
    ASSERT_EQ(size, -PBTOOLS_OUT_OF_DATA);
}

TEST(int64)
{
    int i;
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct int64_message_t *message_p;
    struct {
        int64_t decoded;
        int size;
        const char *encoded_p;
    } datas[] = {
        { -0x8000000000000000, 11, "\x08\x80\x80\x80\x80\x80\x80\x80\x80\x80\x01" },
        { -0x1,                11, "\x08\xff\xff\xff\xff\xff\xff\xff\xff\xff\x01" },
        { 0x0,                  0, "" },
        { 0x1,                  2, "\x08\x01" },
        { 0x7f,                 2, "\x08\x7f" },
        { 0x80,                 3, "\x08\x80\x01" },
        { 0x3fff,               3, "\x08\xff\x7f" },
        { 0x4000,               4, "\x08\x80\x80\x01" },
        { 0x1fffff,             4, "\x08\xff\xff\x7f" },
        { 0x200000,             5, "\x08\x80\x80\x80\x01" },
        { 0xfffffff,            5, "\x08\xff\xff\xff\x7f" },
        { 0x10000000,           6, "\x08\x80\x80\x80\x80\x01" },
        { 0x7ffffffff,          6, "\x08\xff\xff\xff\xff\x7f" },
        { 0x800000000,          7, "\x08\x80\x80\x80\x80\x80\x01" },
        { 0x3ffffffffff,        7, "\x08\xff\xff\xff\xff\xff\x7f" },
        { 0x40000000000,        8, "\x08\x80\x80\x80\x80\x80\x80\x01" },
        { 0x1ffffffffffff,      8, "\x08\xff\xff\xff\xff\xff\xff\x7f" },
        { 0x2000000000000,      9, "\x08\x80\x80\x80\x80\x80\x80\x80\x01" },
        { 0xffffffffffffff,     9, "\x08\xff\xff\xff\xff\xff\xff\xff\x7f" },
        { 0x100000000000000,   10, "\x08\x80\x80\x80\x80\x80\x80\x80\x80\x01" },
        { 0x7fffffffffffffff,  10, "\x08\xff\xff\xff\xff\xff\xff\xff\xff\x7f" }
    };

    /* Default. */
    message_p = int64_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = int64_message_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 0);

    for (i = 0; i < membersof(datas); i++) {
        printf("int64: %lld\n", (long long)datas[i].decoded);

        message_p = int64_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        message_p->value = datas[i].decoded;
        size = int64_message_encode(message_p, &encoded[0], sizeof(encoded));
        ASSERT_EQ(size, datas[i].size);
        ASSERT_MEMORY(&encoded[0], datas[i].encoded_p, size);

        message_p = int64_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        size = int64_message_decode(message_p, &encoded[0], size);
        ASSERT_EQ(size, datas[i].size);
        ASSERT_EQ(message_p->value, datas[i].decoded);
    }
}

TEST(int64_decode)
{
    int i;
    int size;
    uint8_t workspace[512];
    struct int64_message_t *message_p;
    struct {
        int64_t decoded;
        int size;
        const char *encoded_p;
    } datas[] = {
        // Python decoder behaviour.
        { -0x1,       11, "\x08\xff\xff\xff\xff\xff\xff\xff\xff\xff\x7f" }
    };

    for (i = 0; i < membersof(datas); i++) {
        printf("int64: %lld\n", (long long)datas[i].decoded);

        message_p = int64_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        size = int64_message_decode(message_p,
                                    (uint8_t *)datas[i].encoded_p,
                                    datas[i].size);
        ASSERT_EQ(size, datas[i].size);
        ASSERT_EQ(message_p->value, datas[i].decoded);
    }
}

TEST(sint32)
{
    int i;
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct sint32_message_t *message_p;
    struct {
        int32_t decoded;
        int size;
        const char *encoded_p;
    } datas[] = {
        { -0x80000000, 6, "\x08\xff\xff\xff\xff\x0f" },
        { -0x8000001,  6, "\x08\x81\x80\x80\x80\x01" },
        { -0x8000000,  5, "\x08\xff\xff\xff\x7f" },
        { -0x100001,   5, "\x08\x81\x80\x80\x01" },
        { -0x100000,   4, "\x08\xff\xff\x7f" },
        { -0x2001,     4, "\x08\x81\x80\x01" },
        { -0x2000,     3, "\x08\xff\x7f" },
        { -0x41,       3, "\x08\x81\x01" },
        { -0x40,       2, "\x08\x7f" },
        { -0x1,        2, "\x08\x01" },
        { 0x0,         0, "" },
        { 0x1,         2, "\x08\x02" },
        { 0x7f,        3, "\x08\xfe\x01" },
        { 0x80,        3, "\x08\x80\x02" },
        { 0x3fff,      4, "\x08\xfe\xff\x01" },
        { 0x4000,      4, "\x08\x80\x80\x02" },
        { 0x1fffff,    5, "\x08\xfe\xff\xff\x01" },
        { 0x200000,    5, "\x08\x80\x80\x80\x02" },
        { 0xfffffff,   6, "\x08\xfe\xff\xff\xff\x01" },
        { 0x10000000,  6, "\x08\x80\x80\x80\x80\x02" },
        { 0x7fffffff,  6, "\x08\xfe\xff\xff\xff\x0f" }
    };

    /* Default. */
    message_p = sint32_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = sint32_message_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 0);

    for (i = 0; i < membersof(datas); i++) {
        printf("sint32: %d\n", datas[i].decoded);

        message_p = sint32_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        message_p->value = datas[i].decoded;
        size = sint32_message_encode(message_p, &encoded[0], sizeof(encoded));
        ASSERT_EQ(size, datas[i].size);
        ASSERT_MEMORY(&encoded[0], datas[i].encoded_p, size);

        message_p = sint32_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        size = sint32_message_decode(message_p, &encoded[0], size);
        ASSERT_EQ(size, datas[i].size);
        ASSERT_EQ(message_p->value, datas[i].decoded);
    }
}

TEST(sint64)
{
    int i;
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct sint64_message_t *message_p;
    struct {
        int64_t decoded;
        int size;
        const char *encoded_p;
    } datas[] = {
        { -0x8000000000000000, 11, "\x08\xff\xff\xff\xff\xff\xff\xff\xff\xff\x01" },
        { -0x4000000000000001, 11, "\x08\x81\x80\x80\x80\x80\x80\x80\x80\x80\x01" },
        { -0x4000000000000000, 10, "\x08\xff\xff\xff\xff\xff\xff\xff\xff\x7f" },
        { -0x80000000000001,   10, "\x08\x81\x80\x80\x80\x80\x80\x80\x80\x01" },
        { -0x80000000000000,    9, "\x08\xff\xff\xff\xff\xff\xff\xff\x7f" },
        { -0x1000000000001,     9, "\x08\x81\x80\x80\x80\x80\x80\x80\x01" },
        { -0x1000000000000,     8, "\x08\xff\xff\xff\xff\xff\xff\x7f" },
        { -0x20000000001,       8, "\x08\x81\x80\x80\x80\x80\x80\x01" },
        { -0x20000000000,       7, "\x08\xff\xff\xff\xff\xff\x7f" },
        { -0x400000001,         7, "\x08\x81\x80\x80\x80\x80\x01" },
        { -0x400000000,         6, "\x08\xff\xff\xff\xff\x7f" },
        { -0x8000001,           6, "\x08\x81\x80\x80\x80\x01" },
        { -0x8000000,           5, "\x08\xff\xff\xff\x7f" },
        { -0x100001,            5, "\x08\x81\x80\x80\x01" },
        { -0x100000,            4, "\x08\xff\xff\x7f" },
        { -0x2001,              4, "\x08\x81\x80\x01" },
        { -0x2000,              3, "\x08\xff\x7f" },
        { -0x41,                3, "\x08\x81\x01" },
        { -0x40,                2, "\x08\x7f" },
        { -0x1,                 2, "\x08\x01" },
        { 0x0,                  0, "" },
        { 0x1,                  2, "\x08\x02" },
        { 0x3f,                 2, "\x08\x7e" },
        { 0x40,                 3, "\x08\x80\x01" },
        { 0x1fff,               3, "\x08\xfe\x7f" },
        { 0x2000,               4, "\x08\x80\x80\x01" },
        { 0xfffff,              4, "\x08\xfe\xff\x7f" },
        { 0x100000,             5, "\x08\x80\x80\x80\x01" },
        { 0x7ffffff,            5, "\x08\xfe\xff\xff\x7f" },
        { 0x8000000,            6, "\x08\x80\x80\x80\x80\x01" },
        { 0x3ffffffff,          6, "\x08\xfe\xff\xff\xff\x7f" },
        { 0x400000000,          7, "\x08\x80\x80\x80\x80\x80\x01" },
        { 0x1ffffffffff,        7, "\x08\xfe\xff\xff\xff\xff\x7f" },
        { 0x20000000000,        8, "\x08\x80\x80\x80\x80\x80\x80\x01" },
        { 0xffffffffffff,       8, "\x08\xfe\xff\xff\xff\xff\xff\x7f" },
        { 0x1000000000000,      9, "\x08\x80\x80\x80\x80\x80\x80\x80\x01" },
        { 0x7fffffffffffff,     9, "\x08\xfe\xff\xff\xff\xff\xff\xff\x7f" },
        { 0x80000000000000,    10, "\x08\x80\x80\x80\x80\x80\x80\x80\x80\x01" },
        { 0x3fffffffffffffff,  10, "\x08\xfe\xff\xff\xff\xff\xff\xff\xff\x7f" },
        { 0x4000000000000000,  11, "\x08\x80\x80\x80\x80\x80\x80\x80\x80\x80\x01" },
        { 0x7fffffffffffffff,  11, "\x08\xfe\xff\xff\xff\xff\xff\xff\xff\xff\x01" },
    };

    /* Default. */
    message_p = sint64_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = sint64_message_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 0);

    for (i = 0; i < membersof(datas); i++) {
        printf("sint64: %lld\n", (long long)datas[i].decoded);

        message_p = sint64_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        message_p->value = datas[i].decoded;
        size = sint64_message_encode(message_p, &encoded[0], sizeof(encoded));
        ASSERT_EQ(size, datas[i].size);
        ASSERT_MEMORY(&encoded[0], datas[i].encoded_p, size);

        message_p = sint64_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        size = sint64_message_decode(message_p, &encoded[0], size);
        ASSERT_EQ(size, datas[i].size);
        ASSERT_EQ(message_p->value, datas[i].decoded);
    }
}

TEST(uint32)
{
    int i;
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct uint32_message_t *message_p;
    struct {
        uint32_t decoded;
        int size;
        const char *encoded_p;
    } datas[] = {
        { 0x0,        0, "" },
        { 0x1,        2, "\x08\x01" },
        { 0x7f,       2, "\x08\x7f" },
        { 0x80,       3, "\x08\x80\x01" },
        { 0x3fff,     3, "\x08\xff\x7f" },
        { 0x4000,     4, "\x08\x80\x80\x01" },
        { 0x1fffff,   4, "\x08\xff\xff\x7f" },
        { 0x200000,   5, "\x08\x80\x80\x80\x01" },
        { 0xfffffff,  5, "\x08\xff\xff\xff\x7f" },
        { 0x10000000, 6, "\x08\x80\x80\x80\x80\x01" },
        { 0xffffffff, 6, "\x08\xff\xff\xff\xff\x0f" }
    };

    /* Default. */
    message_p = uint32_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = uint32_message_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 0);

    for (i = 0; i < membersof(datas); i++) {
        printf("uint32: %u\n", datas[i].decoded);

        message_p = uint32_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        message_p->value = datas[i].decoded;
        size = uint32_message_encode(message_p, &encoded[0], sizeof(encoded));
        ASSERT_EQ(size, datas[i].size);
        ASSERT_MEMORY(&encoded[0], datas[i].encoded_p, size);

        message_p = uint32_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        size = uint32_message_decode(message_p, &encoded[0], size);
        ASSERT_EQ(size, datas[i].size);
        ASSERT_EQ(message_p->value, datas[i].decoded);
    }
}

TEST(uint64)
{
    int i;
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct uint64_message_t *message_p;
    struct {
        uint64_t decoded;
        int size;
        const char *encoded_p;
    } datas[] = {
        { 0x0,                 0, "" },
        { 0x1,                 2, "\x08\x01" },
        { 0x7f,                2, "\x08\x7f" },
        { 0x80,                3, "\x08\x80\x01" },
        { 0x3fff,              3, "\x08\xff\x7f" },
        { 0x4000,              4, "\x08\x80\x80\x01" },
        { 0x1fffff,            4, "\x08\xff\xff\x7f" },
        { 0x200000,            5, "\x08\x80\x80\x80\x01" },
        { 0xfffffff,           5, "\x08\xff\xff\xff\x7f" },
        { 0x10000000,          6, "\x08\x80\x80\x80\x80\x01" },
        { 0x7ffffffff,         6, "\x08\xff\xff\xff\xff\x7f" },
        { 0x800000000,         7, "\x08\x80\x80\x80\x80\x80\x01" },
        { 0x3ffffffffff,       7, "\x08\xff\xff\xff\xff\xff\x7f" },
        { 0x40000000000,       8, "\x08\x80\x80\x80\x80\x80\x80\x01" },
        { 0x1ffffffffffff,     8, "\x08\xff\xff\xff\xff\xff\xff\x7f" },
        { 0x2000000000000,     9, "\x08\x80\x80\x80\x80\x80\x80\x80\x01" },
        { 0xffffffffffffff,    9, "\x08\xff\xff\xff\xff\xff\xff\xff\x7f" },
        { 0x100000000000000,  10, "\x08\x80\x80\x80\x80\x80\x80\x80\x80\x01" },
        { 0xffffffffffffffff, 11, "\x08\xff\xff\xff\xff\xff\xff\xff\xff\xff\x01" }
    };

    /* Default. */
    message_p = uint64_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = uint64_message_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 0);

    for (i = 0; i < membersof(datas); i++) {
        printf("uint64: %llu\n", (unsigned long long)datas[i].decoded);

        message_p = uint64_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        message_p->value = datas[i].decoded;
        size = uint64_message_encode(message_p, &encoded[0], sizeof(encoded));
        ASSERT_EQ(size, datas[i].size);
        ASSERT_MEMORY(&encoded[0], datas[i].encoded_p, size);

        message_p = uint64_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        size = uint64_message_decode(message_p, &encoded[0], size);
        ASSERT_EQ(size, datas[i].size);
        ASSERT_EQ(message_p->value, datas[i].decoded);
    }
}

TEST(fixed32)
{
    int i;
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct fixed32_message_t *message_p;
    struct {
        uint32_t decoded;
        int size;
        const char *encoded_p;
    } datas[] = {
        { 0x0,        0, "" },
        { 0x1,        5, "\x0d\x01\x00\x00\x00" },
        { 0xffffffff, 5, "\x0d\xff\xff\xff\xff" }
    };

    /* Default. */
    message_p = fixed32_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = fixed32_message_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 0);

    for (i = 0; i < membersof(datas); i++) {
        printf("fixed32: %u\n", datas[i].decoded);

        message_p = fixed32_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        message_p->value = datas[i].decoded;
        size = fixed32_message_encode(message_p, &encoded[0], sizeof(encoded));
        ASSERT_EQ(size, datas[i].size);
        ASSERT_MEMORY(&encoded[0], datas[i].encoded_p, size);

        message_p = fixed32_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        size = fixed32_message_decode(message_p, &encoded[0], size);
        ASSERT_EQ(size, datas[i].size);
        ASSERT_EQ(message_p->value, datas[i].decoded);
    }
}

TEST(fixed64)
{
    int i;
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct fixed64_message_t *message_p;
    struct {
        uint64_t decoded;
        int size;
        const char *encoded_p;
    } datas[] = {
        { 0x0,                0, "" },
        { 0x1,                9, "\x09\x01\x00\x00\x00\x00\x00\x00\x00" },
        { 0xffffffffffffffff, 9, "\x09\xff\xff\xff\xff\xff\xff\xff\xff" }
    };

    /* Default. */
    message_p = fixed64_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = fixed64_message_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 0);

    for (i = 0; i < membersof(datas); i++) {
        printf("fixed64: %llu\n", (unsigned long long)datas[i].decoded);

        message_p = fixed64_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        message_p->value = datas[i].decoded;
        size = fixed64_message_encode(message_p, &encoded[0], sizeof(encoded));
        ASSERT_EQ(size, datas[i].size);
        ASSERT_MEMORY(&encoded[0], datas[i].encoded_p, size);

        message_p = fixed64_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        size = fixed64_message_decode(message_p, &encoded[0], size);
        ASSERT_EQ(size, datas[i].size);
        ASSERT_EQ(message_p->value, datas[i].decoded);
    }
}

TEST(sfixed32)
{
    int i;
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct sfixed32_message_t *message_p;
    struct {
        int32_t decoded;
        int size;
        const char *encoded_p;
    } datas[] = {
        { -0x80000000, 5, "\x0d\x00\x00\x00\x80" },
        { -0x1,        5, "\x0d\xff\xff\xff\xff" },
        { 0x0,         0, "" },
        { 0x1,         5, "\x0d\x01\x00\x00\x00" },
        { 0x7fffffff,  5, "\x0d\xff\xff\xff\x7f" }
    };

    /* Default. */
    message_p = sfixed32_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = sfixed32_message_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 0);

    for (i = 0; i < membersof(datas); i++) {
        printf("sfixed32: %d\n", datas[i].decoded);

        message_p = sfixed32_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        message_p->value = datas[i].decoded;
        size = sfixed32_message_encode(message_p, &encoded[0], sizeof(encoded));
        ASSERT_EQ(size, datas[i].size);
        ASSERT_MEMORY(&encoded[0], datas[i].encoded_p, size);

        message_p = sfixed32_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        size = sfixed32_message_decode(message_p, &encoded[0], size);
        ASSERT_EQ(size, datas[i].size);
        ASSERT_EQ(message_p->value, datas[i].decoded);
    }
}

TEST(sfixed64)
{
    int i;
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct sfixed64_message_t *message_p;
    struct {
        int64_t decoded;
        int size;
        const char *encoded_p;
    } datas[] = {
        { -0x8000000000000000, 9, "\x09\x00\x00\x00\x00\x00\x00\x00\x80" },
        { -0x1,                9, "\x09\xff\xff\xff\xff\xff\xff\xff\xff" },
        { 0x0,                 0, "" },
        { 0x1,                 9, "\x09\x01\x00\x00\x00\x00\x00\x00\x00" },
        { 0x7fffffffffffffff,  9, "\x09\xff\xff\xff\xff\xff\xff\xff\x7f" }
    };

    /* Default. */
    message_p = sfixed64_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = sfixed64_message_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 0);

    for (i = 0; i < membersof(datas); i++) {
        printf("sfixed64: %lld\n", (long long)datas[i].decoded);

        message_p = sfixed64_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        message_p->value = datas[i].decoded;
        size = sfixed64_message_encode(message_p, &encoded[0], sizeof(encoded));
        ASSERT_EQ(size, datas[i].size);
        ASSERT_MEMORY(&encoded[0], datas[i].encoded_p, size);

        message_p = sfixed64_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        size = sfixed64_message_decode(message_p, &encoded[0], size);
        ASSERT_EQ(size, datas[i].size);
        ASSERT_EQ(message_p->value, datas[i].decoded);
    }
}

TEST(float_)
{
    int i;
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct float_message_t *message_p;
    struct {
        float decoded;
        int size;
        const char *encoded_p;
    } datas[] = {
        { -500.0, 5, "\x0d\x00\x00\xfa\xc3" },
        { -1.0,   5, "\x0d\x00\x00\x80\xbf" },
        { 0.0,    0, "" },
        { 1.0,    5, "\x0d\x00\x00\x80\x3f" },
        { 500.0,  5, "\x0d\x00\x00\xfa\x43" }
    };

    /* Default. */
    message_p = float_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = float_message_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 0);

    for (i = 0; i < membersof(datas); i++) {
        printf("float: %f\n", datas[i].decoded);

        message_p = float_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        message_p->value = datas[i].decoded;
        size = float_message_encode(message_p, &encoded[0], sizeof(encoded));
        ASSERT_EQ(size, datas[i].size);
        ASSERT_MEMORY(&encoded[0], datas[i].encoded_p, size);

        message_p = float_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        size = float_message_decode(message_p, &encoded[0], size);
        ASSERT_EQ(size, datas[i].size);
        ASSERT_EQ(message_p->value, datas[i].decoded);
    }
}

TEST(double_)
{
    int i;
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct double_message_t *message_p;
    struct {
        double decoded;
        double size;
        const char *encoded_p;
    } datas[] = {
        { -500.0, 9, "\x09\x00\x00\x00\x00\x00\x40\x7f\xc0" },
        { -1.0,   9, "\x09\x00\x00\x00\x00\x00\x00\xf0\xbf" },
        { 0.0,    0, "" },
        { 1.0,    9, "\x09\x00\x00\x00\x00\x00\x00\xf0\x3f" },
        { 500.0,  9, "\x09\x00\x00\x00\x00\x00\x40\x7f\x40" }
    };

    /* Default. */
    message_p = double_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = double_message_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 0);

    for (i = 0; i < membersof(datas); i++) {
        printf("double: %f\n", datas[i].decoded);

        message_p = double_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        message_p->value = datas[i].decoded;
        size = double_message_encode(message_p, &encoded[0], sizeof(encoded));
        ASSERT_EQ(size, datas[i].size);
        ASSERT_MEMORY(&encoded[0], datas[i].encoded_p, size);

        message_p = double_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        size = double_message_decode(message_p, &encoded[0], size);
        ASSERT_EQ(size, datas[i].size);
        ASSERT_EQ(message_p->value, datas[i].decoded);
    }
}

TEST(bool_)
{
    int i;
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct bool_message_t *message_p;
    struct {
        bool decoded;
        int size;
        const char *encoded_p;
    } datas[] = {
        { true, 2, "\x08\x01" },
        { false, 0, "" }
    };

    /* Default. */
    message_p = bool_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = bool_message_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 0);

    for (i = 0; i < membersof(datas); i++) {
        printf("bool: %d\n", datas[i].decoded);

        message_p = bool_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        message_p->value = datas[i].decoded;
        size = bool_message_encode(message_p, &encoded[0], sizeof(encoded));
        ASSERT_EQ(size, datas[i].size);
        ASSERT_MEMORY(&encoded[0], datas[i].encoded_p, size);

        message_p = bool_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        size = bool_message_decode(message_p, &encoded[0], size);
        ASSERT_EQ(size, datas[i].size);
        ASSERT_EQ(message_p->value, datas[i].decoded);
    }
}

TEST(string)
{
    int i;
    uint8_t encoded[256];
    int size;
    uint8_t workspace[512];
    struct string_message_t *message_p;
    struct {
        int decoded_size;
        char *decoded_p;
        int size;
        const char *encoded_p;
    } datas[] = {
        { 0, "",  0, "" },
        { 1, "1", 3, "\x0a\x01\x31" },
        { 150,
          "123456789012345678901234567890123456789012345678901234567890"
          "123456789012345678901234567890123456789012345678901234567890"
          "123456789012345678901234567890",
          153,
          "\x0a\x96\x01\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32"
          "\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x36\x37"
          "\x38\x39\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32"
          "\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x36\x37"
          "\x38\x39\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32"
          "\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x36\x37"
          "\x38\x39\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32"
          "\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x36\x37"
          "\x38\x39\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32"
          "\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x36\x37"
          "\x38\x39\x30" }
    };

    /* Default. */
    message_p = string_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = string_message_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 0);

    for (i = 0; i < membersof(datas); i++) {
        printf("string: '%s'\n", datas[i].decoded_p);

        message_p = string_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        pbtools_set_string(&message_p->value, datas[i].decoded_p);
        size = string_message_encode(message_p, &encoded[0], sizeof(encoded));
        ASSERT_EQ(size, datas[i].size);
        ASSERT_MEMORY(&encoded[0], datas[i].encoded_p, size);

        message_p = string_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        size = string_message_decode(message_p, &encoded[0], size);
        ASSERT_EQ(size, datas[i].size);
        ASSERT_EQ(message_p->value.size, datas[i].decoded_size);
        ASSERT_MEMORY(message_p->value.buf_p,
                      datas[i].decoded_p,
                      datas[i].decoded_size + 1);
    }
}

TEST(string_encode_buffer_full)
{
    uint8_t encoded[150];
    int size;
    uint8_t workspace[512];
    struct string_message_t *message_p;

    message_p = string_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    pbtools_set_string(
        &message_p->value,
        "123456789012345678901234567890123456789012345678901234567890"
        "123456789012345678901234567890123456789012345678901234567890"
        "123456789012345678901234567890");
    message_p->value.size = 150;
    size = string_message_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, -PBTOOLS_ENCODE_BUFFER_FULL);
}

TEST(string_decode_out_of_data)
{
    int size;
    uint8_t workspace[512];
    struct string_message_t *message_p;

    message_p = string_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = string_message_decode(message_p, (uint8_t *)"\x0a\x01", 2);
    ASSERT_EQ(size, -PBTOOLS_OUT_OF_DATA);

    message_p = string_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = string_message_decode(message_p,
                                 (uint8_t *)"\x0a\x96\x01\x31",
                                 4);
    ASSERT_EQ(size, -PBTOOLS_OUT_OF_DATA);
}

TEST(string_embedded_zero_termination)
{
    uint8_t encoded[256];
    int size;
    uint8_t workspace[512];
    struct string_message_t *message_p;

    message_p = string_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    message_p->value.buf_p = (uint8_t *)"\x31\x00\x32";
    message_p->value.size = 3;
    size = string_message_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 5);
    ASSERT_MEMORY(&encoded[0], "\x0a\x03\x31\x00\x32", size);

    message_p = string_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = string_message_decode(message_p, &encoded[0], size);
    ASSERT_EQ(size, 5);
    ASSERT_EQ(message_p->value.size, 3);
    ASSERT_MEMORY(message_p->value.buf_p, "\x31\x00\x32\x00", 4);
}

TEST(bytes)
{
    int i;
    uint8_t encoded[256];
    int size;
    uint8_t workspace[512];
    struct bytes_message_t *message_p;
    struct {
        uint8_t *decoded_p;
        int decoded_size;
        int size;
        const char *encoded_p;
    } datas[] = {
        { (uint8_t *)"" ,  0, 0, ""},
        { (uint8_t *)"1" , 1, 3, "\x0a\x01\x31"},
        { (uint8_t *)
          "123456789012345678901234567890123456789012345678901234567890"
          "123456789012345678901234567890123456789012345678901234567890"
          "123456789012345678901234567890",
          150,
          153,
          "\x0a\x96\x01\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32"
          "\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x36\x37"
          "\x38\x39\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32"
          "\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x36\x37"
          "\x38\x39\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32"
          "\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x36\x37"
          "\x38\x39\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32"
          "\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x36\x37"
          "\x38\x39\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32"
          "\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x36\x37"
          "\x38\x39\x30" }
    };

    /* Default. */
    message_p = bytes_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = bytes_message_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 0);

    for (i = 0; i < membersof(datas); i++) {
        printf("bytes size: %d\n", datas[i].size);

        message_p = bytes_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        message_p->value.buf_p = datas[i].decoded_p;
        message_p->value.size = datas[i].decoded_size;
        size = bytes_message_encode(message_p, &encoded[0], sizeof(encoded));
        ASSERT_EQ(size, datas[i].size);
        ASSERT_MEMORY(&encoded[0], datas[i].encoded_p, size);

        message_p = bytes_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        size = bytes_message_decode(message_p, &encoded[0], size);
        ASSERT_EQ(size, datas[i].size);
        ASSERT_EQ(message_p->value.size, datas[i].decoded_size);
        ASSERT_MEMORY(message_p->value.buf_p,
                      datas[i].decoded_p,
                      datas[i].decoded_size);
    }
}

TEST(enum_)
{
    int i;
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct enum_message_t *message_p;
    struct {
        enum enum_message_enum_e decoded;
        int size;
        const char *encoded_p;
    } datas[] = {
        { enum_message_enum_a_e, 0, "" },
        { enum_message_enum_b_e, 2, "\x08\x01" }
    };

    /* Default. */
    message_p = enum_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = enum_message_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 0);

    for (i = 0; i < membersof(datas); i++) {
        printf("enum: %d\n", datas[i].decoded);

        message_p = enum_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        message_p->value = datas[i].decoded;
        size = enum_message_encode(message_p, &encoded[0], sizeof(encoded));
        ASSERT_EQ(size, datas[i].size);
        ASSERT_MEMORY(&encoded[0], datas[i].encoded_p, size);

        message_p = enum_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        size = enum_message_decode(message_p, &encoded[0], size);
        ASSERT_EQ(size, datas[i].size);
        ASSERT_EQ(message_p->value, datas[i].decoded);
    }
}

TEST(address_book)
{
    uint8_t encoded[128];
    int size;
    uint8_t workspace[1024];
    struct address_book_address_book_t *address_book_p;
    struct address_book_person_t *person_p;
    struct address_book_person_phone_number_t *phone_number_p;

    address_book_p = address_book_address_book_new(&workspace[0],
                                                   sizeof(workspace));
    ASSERT_NE(address_book_p, NULL);

    /* Add one person to the address book. */
    ASSERT_EQ(address_book_address_book_people_alloc(address_book_p, 1), 0);
    person_p = address_book_p->people.items_pp[0];
    pbtools_set_string(&person_p->name, "Kalle Kula");
    person_p->id = 56;
    pbtools_set_string(&person_p->email, "kalle.kula@foobar.com");

    /* Add phone numbers. */
    ASSERT_EQ(address_book_person_phones_alloc(person_p, 2), 0);

    /* Home. */
    phone_number_p = person_p->phones.items_pp[0];
    pbtools_set_string(&phone_number_p->number, "+46701232345");
    phone_number_p->type = address_book_person_phone_type_home_e;

    /* Work. */
    phone_number_p = person_p->phones.items_pp[1];
    pbtools_set_string(&phone_number_p->number, "+46999999999");
    phone_number_p->type = address_book_person_phone_type_work_e;

    /* Encode the message. */
    size = address_book_address_book_encode(address_book_p,
                                            &encoded[0],
                                            sizeof(encoded));
    ASSERT_EQ(size, 75);
    ASSERT_MEMORY(&encoded[0],
                  "\x0a\x49\x0a\x0a\x4b\x61\x6c\x6c\x65\x20"
                  "\x4b\x75\x6c\x61\x10\x38\x1a\x15\x6b\x61"
                  "\x6c\x6c\x65\x2e\x6b\x75\x6c\x61\x40\x66"
                  "\x6f\x6f\x62\x61\x72\x2e\x63\x6f\x6d\x22"
                  "\x10\x0a\x0c\x2b\x34\x36\x37\x30\x31\x32"
                  "\x33\x32\x33\x34\x35\x10\x01\x22\x10\x0a"
                  "\x0c\x2b\x34\x36\x39\x39\x39\x39\x39\x39"
                  "\x39\x39\x39\x10\x02",
                  size);

    /* Decode the message. */
    address_book_p = address_book_address_book_new(&workspace[0],
                                                   sizeof(workspace));
    ASSERT_NE(address_book_p, NULL);
    size = address_book_address_book_decode(address_book_p, &encoded[0], size);
    ASSERT_EQ(size, 75);

    /* Check the decoded person. */
    ASSERT_EQ(address_book_p->people.length, 1);
    person_p = address_book_p->people.items_pp[0];
    ASSERT_SUBSTRING(pbtools_get_string(&person_p->name), "Kalle Kula");
    ASSERT_EQ(person_p->id, 56);
    ASSERT_SUBSTRING(pbtools_get_string(&person_p->email),
                     "kalle.kula@foobar.com");

    /* Check phone numbers. */
    ASSERT_EQ(person_p->phones.length, 2);

    /* Home. */
    phone_number_p = person_p->phones.items_pp[0];
    ASSERT_SUBSTRING(pbtools_get_string(&phone_number_p->number),
                     "+46701232345");
    ASSERT_EQ(phone_number_p->type, address_book_person_phone_type_home_e);

    /* Work. */
    phone_number_p = person_p->phones.items_pp[1];
    ASSERT_SUBSTRING(pbtools_get_string(&phone_number_p->number),
                     "+46999999999");
    ASSERT_EQ(phone_number_p->type, address_book_person_phone_type_work_e);
}

TEST(address_book_default)
{
    uint8_t encoded[75];
    int size;
    uint8_t workspace[512];
    struct address_book_address_book_t *address_book_p;

    address_book_p = address_book_address_book_new(&workspace[0],
                                                   sizeof(workspace));
    ASSERT_NE(address_book_p, NULL);

    /* Encode the message. */
    size = address_book_address_book_encode(address_book_p,
                                            &encoded[0],
                                            sizeof(encoded));
    ASSERT_EQ(size, 0);

    /* Decode the message. */
    address_book_p = address_book_address_book_new(&workspace[0],
                                                   sizeof(workspace));
    ASSERT_NE(address_book_p, NULL);
    size = address_book_address_book_decode(address_book_p, &encoded[0], size);
    ASSERT_EQ(size, 0);
    ASSERT_EQ(address_book_p->people.length, 0);
}

TEST(address_book_default_person)
{
    uint8_t encoded[75];
    int size;
    uint8_t workspace[512];
    struct address_book_address_book_t *address_book_p;
    struct address_book_person_t *person_p;

    address_book_p = address_book_address_book_new(&workspace[0],
                                                   sizeof(workspace));
    ASSERT_NE(address_book_p, NULL);

    /* Add one person to the address book. */
    ASSERT_EQ(address_book_address_book_people_alloc(address_book_p, 1), 0);

    /* Encode the message. */
    size = address_book_address_book_encode(address_book_p,
                                            &encoded[0],
                                            sizeof(encoded));
    ASSERT_EQ(size, 2);
    ASSERT_MEMORY(&encoded[0], "\x0a\x00", size);

    /* Decode the message. */
    address_book_p = address_book_address_book_new(&workspace[0],
                                                   sizeof(workspace));
    ASSERT_NE(address_book_p, NULL);
    size = address_book_address_book_decode(address_book_p, &encoded[0], size);
    ASSERT_EQ(size, 2);
    ASSERT_EQ(address_book_p->people.length, 1);

    /* Check the decoded person. */
    person_p = address_book_p->people.items_pp[0];
    ASSERT_SUBSTRING(pbtools_get_string(&person_p->name), "");
    ASSERT_EQ(person_p->id, 0);
    ASSERT_SUBSTRING(pbtools_get_string(&person_p->email), "");
    ASSERT_EQ(person_p->phones.length, 0);
}

TEST(address_book_person)
{
    uint8_t encoded[75];
    int size;
    uint8_t workspace[512];
    struct address_book_person_t *person_p;

    person_p = address_book_person_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(person_p, NULL);

    pbtools_set_string(&person_p->name, "Kalle Kula");
    person_p->id = 56;
    pbtools_set_string(&person_p->email, "kalle.kula@foobar.com");

    /* Encode the message. */
    size = address_book_person_encode(person_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 37);
    ASSERT_MEMORY(&encoded[0],
                  "\x0a\x0a\x4b\x61\x6c\x6c\x65\x20\x4b\x75"
                  "\x6c\x61\x10\x38\x1a\x15\x6b\x61\x6c\x6c"
                  "\x65\x2e\x6b\x75\x6c\x61\x40\x66\x6f\x6f"
                  "\x62\x61\x72\x2e\x63\x6f\x6d",
                  size);

    /* Decode the message. */
    person_p = address_book_person_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(person_p, NULL);
    size = address_book_person_decode(person_p, &encoded[0], size);
    ASSERT_EQ(size, 37);

    /* Check the decoded person. */
    ASSERT_SUBSTRING(pbtools_get_string(&person_p->name),
                     "Kalle Kula");
    ASSERT_EQ(person_p->id, 56);
    ASSERT_SUBSTRING(pbtools_get_string(&person_p->email),
                     "kalle.kula@foobar.com");
    ASSERT_EQ(person_p->phones.length, 0);
}

TEST(address_book_decode_issue_1)
{
    int size;
    uint8_t workspace[4096];
    struct address_book_address_book_t *address_book_p;

    address_book_p = address_book_address_book_new(&workspace[0],
                                                   sizeof(workspace));
    ASSERT_NE(address_book_p, NULL);
    size = address_book_address_book_decode(address_book_p,
                                            (uint8_t *)"\x00\x0c\x2e\x00",
                                            4);
    ASSERT_EQ(size, -PBTOOLS_BAD_FIELD_NUMBER);
}

TEST(address_book_decode_issue_2)
{
    int size;
    uint8_t workspace[4096];
    struct address_book_address_book_t *address_book_p;

    address_book_p = address_book_address_book_new(&workspace[0],
                                                   sizeof(workspace));
    ASSERT_NE(address_book_p, NULL);
    size = address_book_address_book_decode(address_book_p,
                                            (uint8_t *)"\x0b\x02\x00\x0e\x00",
                                            5);
    ASSERT_EQ(size, -PBTOOLS_BAD_WIRE_TYPE);
}

TEST(address_book_decode_issue_3)
{
    int size;
    uint8_t workspace[4096];
    struct address_book_address_book_t *address_book_p;

    address_book_p = address_book_address_book_new(&workspace[0],
                                                   sizeof(workspace));
    ASSERT_NE(address_book_p, NULL);
    size = address_book_address_book_decode(
        address_book_p,
        (uint8_t *)"\x41\x0a\x02\x08\x00\x02",
        6);
    ASSERT_EQ(size, -PBTOOLS_OUT_OF_DATA);
}

TEST(address_book_decode_issue_4)
{
    int size;
    uint8_t workspace[4096];
    struct address_book_address_book_t *address_book_p;

    address_book_p = address_book_address_book_new(&workspace[0],
                                                   sizeof(workspace));
    ASSERT_NE(address_book_p, NULL);
    size = address_book_address_book_decode(
        address_book_p,
        (uint8_t *)"\x8a\x00\x01\x1e",
        4);
    ASSERT_EQ(size, -PBTOOLS_BAD_WIRE_TYPE);
}

TEST(address_book_decode_issue_5)
{
    int size;
    uint8_t workspace[4096];
    struct address_book_address_book_t *address_book_p;

    address_book_p = address_book_address_book_new(&workspace[0],
                                                   sizeof(workspace));
    ASSERT_NE(address_book_p, NULL);
    size = address_book_address_book_decode(
        address_book_p,
        (uint8_t *)"\x0a\x0a\x00\x8a\x02\x0a\xff\xff\x02\x00\xd2\x00\x00\x00",
        14);
    ASSERT_EQ(size, -PBTOOLS_BAD_FIELD_NUMBER);
}

TEST(address_book_decode_issue_6)
{
    int size;
    uint8_t workspace[4096];
    struct address_book_address_book_t *address_book_p;

    address_book_p = address_book_address_book_new(&workspace[0],
                                                   sizeof(workspace));
    ASSERT_NE(address_book_p, NULL);
    size = address_book_address_book_decode(
        address_book_p,
        (uint8_t *)"\x0a\x07\x4a\xff\xff\xff\xff\x07\xff\x0e",
        10);
    ASSERT_EQ(size, -PBTOOLS_SEEK_OVERFLOW);
}

TEST(address_book_decode_issue_7)
{
    int size;
    uint8_t workspace[1024];
    struct address_book_address_book_t *address_book_p;
    struct address_book_person_t *person_p;

    address_book_p = address_book_address_book_new(&workspace[0],
                                                   sizeof(workspace));
    ASSERT_NE(address_book_p, NULL);
    size = address_book_address_book_decode(
        address_book_p,
        (uint8_t *)"\x0a\x0a\x10\xff\xff\xff\xff\xfd\xff\xfb\x8f\x70",
        12);
    ASSERT_EQ(size, 12);
    ASSERT_EQ(address_book_p->people.length, 1);

    person_p = address_book_p->people.items_pp[0];
    ASSERT_SUBSTRING(pbtools_get_string(&person_p->name), "");
    ASSERT_EQ(person_p->id, -536870913);
    ASSERT_SUBSTRING(pbtools_get_string(&person_p->email), "");
    ASSERT_EQ(person_p->phones.length, 0);
}

TEST(tags_1)
{
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct tags_message1_t *message_p;

    message_p = tags_message1_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    message_p->value = true;
    size = tags_message1_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 2);
    ASSERT_MEMORY(&encoded[0], "\x08\x01", size);

    message_p = tags_message1_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = tags_message1_decode(message_p, &encoded[0], size);
    ASSERT_EQ(size, 2);
    ASSERT_EQ(message_p->value, true);
}

TEST(tags_2)
{
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct tags_message2_t *message_p;

    message_p = tags_message2_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    message_p->value = true;
    size = tags_message2_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 2);
    ASSERT_MEMORY(&encoded[0], "\x78\x01", size);

    message_p = tags_message2_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = tags_message2_decode(message_p, &encoded[0], size);
    ASSERT_EQ(size, 2);
    ASSERT_EQ(message_p->value, true);
}

TEST(tags_3)
{
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct tags_message3_t *message_p;

    message_p = tags_message3_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    message_p->value = true;
    size = tags_message3_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 3);
    ASSERT_MEMORY(&encoded[0], "\x80\x01\x01", size);

    message_p = tags_message3_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = tags_message3_decode(message_p, &encoded[0], size);
    ASSERT_EQ(size, 3);
    ASSERT_EQ(message_p->value, true);
}

TEST(tags_4)
{
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct tags_message4_t *message_p;

    message_p = tags_message4_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    message_p->value = true;
    size = tags_message4_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 3);
    ASSERT_MEMORY(&encoded[0], "\xf8\x7f\x01", size);

    message_p = tags_message4_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = tags_message4_decode(message_p, &encoded[0], size);
    ASSERT_EQ(size, 3);
    ASSERT_EQ(message_p->value, true);
}

TEST(tags_5)
{
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct tags_message5_t *message_p;

    message_p = tags_message5_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    message_p->value = true;
    size = tags_message5_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 4);
    ASSERT_MEMORY(&encoded[0], "\x80\x80\x01\x01", size);

    message_p = tags_message5_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = tags_message5_decode(message_p, &encoded[0], size);
    ASSERT_EQ(size, 4);
    ASSERT_EQ(message_p->value, true);
}

TEST(tags_6)
{
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct tags_message6_t *message_p;

    message_p = tags_message6_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    message_p->value = true;
    size = tags_message6_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 6);
    ASSERT_MEMORY(&encoded[0], "\xf8\xff\xff\xff\x0f\x01", size);

    message_p = tags_message6_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = tags_message6_decode(message_p, &encoded[0], size);
    ASSERT_EQ(size, 6);
    ASSERT_EQ(message_p->value, true);
}

TEST(oneof_v1)
{
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct oneof_message_t *message_p;

    message_p = oneof_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);

    ASSERT_EQ(message_p->value.choice, oneof_message_value_v1_e);
    ASSERT_EQ(message_p->value.value.v1, 0);

    message_p->value.choice = oneof_message_value_v1_e;
    message_p->value.value.v1 = 65;
    size = oneof_message_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 2);
    ASSERT_MEMORY(&encoded[0], "\x08\x41", size);

    message_p = oneof_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = oneof_message_decode(message_p, &encoded[0], size);
    ASSERT_EQ(size, 2);
    ASSERT_EQ(message_p->value.choice, oneof_message_value_v1_e);
    ASSERT_EQ(message_p->value.value.v1, 65);
}

TEST(oneof_v2)
{
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct oneof_message_t *message_p;

    message_p = oneof_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    message_p->value.choice = oneof_message_value_v2_e;
    pbtools_set_string(&message_p->value.value.v2, "Hello!");
    size = oneof_message_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 8);
    ASSERT_MEMORY(&encoded[0], "\x12\x06Hello!", size);

    message_p = oneof_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = oneof_message_decode(message_p, &encoded[0], size);
    ASSERT_EQ(size, 8);
    ASSERT_EQ(message_p->value.choice, oneof_message_value_v2_e);
    ASSERT_SUBSTRING(pbtools_get_string(&message_p->value.value.v2),
                     "Hello!");
}

TEST(repeated_int32s_one_item)
{
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct repeated_message_t *message_p;

    message_p = repeated_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    ASSERT_EQ(message_p->int32s.length, 0);
    ASSERT_EQ(message_p->messages.length, 0);
    ASSERT_EQ(message_p->strings.length, 0);
    ASSERT_EQ(message_p->bytes.length, 0);
    ASSERT_EQ(repeated_message_int32s_alloc(message_p, 1), 0);
    ASSERT_EQ(message_p->int32s.length, 1);
    message_p->int32s.items_pp[0]->value = 7;
    size = repeated_message_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 3);
    ASSERT_MEMORY(&encoded[0], "\x0a\x01\x07", size);

    message_p = repeated_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = repeated_message_decode(message_p, &encoded[0], size);
    ASSERT_EQ(size, 3);
    ASSERT_EQ(message_p->int32s.length, 1);
    ASSERT_EQ(message_p->int32s.items_pp[0]->value, 7);
    ASSERT_EQ(message_p->messages.length, 0);
    ASSERT_EQ(message_p->strings.length, 0);
    ASSERT_EQ(message_p->bytes.length, 0);
}

TEST(repeated_int32s_two_items)
{
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct repeated_message_t *message_p;

    message_p = repeated_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    ASSERT_EQ(repeated_message_int32s_alloc(message_p, 2), 0);
    message_p->int32s.items_pp[0]->value = 1;
    message_p->int32s.items_pp[1]->value = 1000;
    size = repeated_message_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 5);
    ASSERT_MEMORY(&encoded[0], "\x0a\x03\x01\xe8\x07", size);

    message_p = repeated_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = repeated_message_decode(message_p, &encoded[0], size);
    ASSERT_EQ(size, 5);
    ASSERT_EQ(message_p->int32s.length, 2);
    ASSERT_EQ(message_p->int32s.items_pp[0]->value, 1);
    ASSERT_EQ(message_p->int32s.items_pp[1]->value, 1000);
}

TEST(repeated_int32s_decode_segments)
{
    int i;
    int size;
    uint8_t workspace[512];
    struct repeated_message_t *message_p;
    struct {
        int size;
        const char *encoded_p;
    } datas[] = {
        { 5, "\x0a\x03\x01\x02\x03" },
        { 7, "\x0a\x01\x01\x0a\x02\x02\x03" }
    };

    for (i = 0; i < membersof(datas); i++) {
        message_p = repeated_message_new(&workspace[0], sizeof(workspace));
        size = repeated_message_decode(message_p,
                                       (uint8_t *)datas[i].encoded_p,
                                       datas[i].size);
        ASSERT_EQ(size, datas[i].size);
        ASSERT_EQ(message_p->int32s.length, 3);
        ASSERT_EQ(message_p->int32s.items_pp[0]->value, 1);
        ASSERT_EQ(message_p->int32s.items_pp[1]->value, 2);
        ASSERT_EQ(message_p->int32s.items_pp[2]->value, 3);
    }
}

TEST(repeated_int32s_decode_zero_items)
{
    int size;
    uint8_t workspace[512];
    struct repeated_message_t *message_p;

    message_p = repeated_message_new(&workspace[0], sizeof(workspace));
    size = repeated_message_decode(message_p, (uint8_t *)"\x0a\x00", 2);
    ASSERT_EQ(size, 2);
    ASSERT_EQ(message_p->int32s.length, 0);
}

TEST(repeated_int32s_decode_error_out_of_memory)
{
    int size;
    uint8_t workspace[512];
    struct repeated_message_t *message_p;

    message_p = repeated_message_new(&workspace[0], sizeof(workspace));
    size = repeated_message_decode(
        message_p,
        (uint8_t *)
        "\x0a\x64"
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        104);
    ASSERT_EQ(size, -PBTOOLS_OUT_OF_MEMORY);
}

TEST(repeated_messages_decode_zero_items)
{
    int size;
    uint8_t workspace[512];
    struct repeated_message_t *message_p;
    struct repeated_message_t *message_1_p;

    message_p = repeated_message_new(&workspace[0], sizeof(workspace));
    size = repeated_message_decode(message_p, (uint8_t *)"\x12\x00", 2);
    ASSERT_EQ(size, 2);
    ASSERT_EQ(message_p->messages.length, 1);
    message_1_p = message_p->messages.items_pp[0];
    ASSERT_EQ(message_1_p->int32s.length, 0);
    ASSERT_EQ(message_1_p->messages.length, 0);
    ASSERT_EQ(message_1_p->strings.length, 0);
    ASSERT_EQ(message_1_p->bytes.length, 0);
}

TEST(repeated_messages_decode_error_too_big)
{
    int size;
    uint8_t workspace[512];
    struct repeated_message_t *message_p;

    message_p = repeated_message_new(&workspace[0], sizeof(workspace));
    size = repeated_message_decode(message_p, (uint8_t *)"\x12\xff\xff\x00", 4);
    ASSERT_EQ(size, -PBTOOLS_OUT_OF_DATA);
}

TEST(repeated_bytes_two_items)
{
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct repeated_message_t *message_p;

    message_p = repeated_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    ASSERT_EQ(repeated_message_bytes_alloc(message_p, 2), 0);
    message_p->bytes.items_pp[0]->size = 1;
    message_p->bytes.items_pp[0]->buf_p = (uint8_t *)"1";
    message_p->bytes.items_pp[1]->size = 1;
    message_p->bytes.items_pp[1]->buf_p = (uint8_t *)"2";
    size = repeated_message_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 6);
    ASSERT_MEMORY(&encoded[0], "\x22\x01\x31\x22\x01\x32", size);

    message_p = repeated_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = repeated_message_decode(message_p, &encoded[0], size);
    ASSERT_EQ(size, 6);
    ASSERT_EQ(message_p->bytes.length, 2);
    ASSERT_EQ(message_p->bytes.items_pp[0]->size, 1);
    ASSERT_MEMORY(message_p->bytes.items_pp[0]->buf_p, "1", 1);
    ASSERT_EQ(message_p->bytes.items_pp[1]->size, 1);
    ASSERT_MEMORY(message_p->bytes.items_pp[1]->buf_p, "2", 1);
}

TEST(repeated_bytes_decode_element_of_zero_bytes)
{
    int size;
    uint8_t workspace[512];
    struct repeated_message_t *message_p;

    message_p = repeated_message_new(&workspace[0], sizeof(workspace));
    size = repeated_message_decode(message_p,
                                   (uint8_t *)"\x22\x00\x22\x01\x32",
                                   5);
    ASSERT_EQ(size, 5);
    ASSERT_EQ(message_p->bytes.length, 2);
    ASSERT_EQ(message_p->bytes.items_pp[0]->size, 0);
    ASSERT_EQ(message_p->bytes.items_pp[1]->size, 1);
    ASSERT_MEMORY(message_p->bytes.items_pp[1]->buf_p, "2", 1);
}

TEST(repeated_nested)
{
    uint8_t encoded[128];
    int size;
    uint8_t workspace[1024];
    struct repeated_message_t *message_p;
    struct repeated_message_t *message_1_p;
    struct repeated_message_t *message_2_p;

    message_p = repeated_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);

    /* int32s[0..2]. */
    ASSERT_EQ(repeated_message_int32s_alloc(message_p, 3), 0);
    message_p->int32s.items_pp[0]->value = 1;
    message_p->int32s.items_pp[1]->value = 2;
    message_p->int32s.items_pp[2]->value = 3;

    /* messages[0]. */
    ASSERT_EQ(repeated_message_messages_alloc(message_p, 2), 0);
    message_1_p = message_p->messages.items_pp[0];

    /* messages[0].int32s[0]. */
    ASSERT_EQ(repeated_message_int32s_alloc(message_1_p, 1), 0);
    message_1_p->int32s.items_pp[0]->value = 9;

    /* messages[0].messages[0]. */
    ASSERT_EQ(repeated_message_messages_alloc(message_1_p, 1), 0);
    message_2_p = message_1_p->messages.items_pp[0];

    /* messages[0].messages[0].int32s[0..1]. */
    ASSERT_EQ(repeated_message_int32s_alloc(message_2_p, 2), 0);
    message_2_p->int32s.items_pp[0]->value = 5;
    message_2_p->int32s.items_pp[1]->value = 7;

    /* messages[1].int32s[0]. */
    message_1_p = message_p->messages.items_pp[1];
    ASSERT_EQ(repeated_message_int32s_alloc(message_1_p, 1), 0);
    message_1_p->int32s.items_pp[0]->value = 5;

    /* strings[0..1]. */
    ASSERT_EQ(repeated_message_strings_alloc(message_p, 2), 0);
    pbtools_set_string(message_p->strings.items_pp[0], "foo");
    pbtools_set_string(message_p->strings.items_pp[1], "bar");

    size = repeated_message_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 31);
    ASSERT_MEMORY(&encoded[0],
                  "\x0a\x03\x01\x02\x03\x12\x09\x0a\x01\x09"
                  "\x12\x04\x0a\x02\x05\x07\x12\x03\x0a\x01"
                  "\x05\x1a\x03\x66\x6f\x6f\x1a\x03\x62\x61"
                  "\x72",
                  size);

    message_p = repeated_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = repeated_message_decode(message_p, &encoded[0], size);
    ASSERT_EQ(size, 31);

    /* int32s[0..2]. */
    ASSERT_EQ(message_p->int32s.length, 3);
    ASSERT_EQ(message_p->int32s.items_pp[0]->value, 1);
    ASSERT_EQ(message_p->int32s.items_pp[1]->value, 2);
    ASSERT_EQ(message_p->int32s.items_pp[2]->value, 3);

    /* strings[0..1]. */
    ASSERT_EQ(message_p->strings.length, 2);
    ASSERT_SUBSTRING(pbtools_get_string(message_p->strings.items_pp[0]),
                     "foo");
    ASSERT_SUBSTRING(pbtools_get_string(message_p->strings.items_pp[1]),
                     "bar");

    /* messages[0]. */
    ASSERT_EQ(message_p->messages.length, 2);
    message_1_p = message_p->messages.items_pp[0];

    /* messages[0].int32s[0]. */
    ASSERT_EQ(message_1_p->int32s.length, 1);
    ASSERT_EQ(message_1_p->int32s.items_pp[0]->value, 9);

    /* messages[0].messages[0]. */
    ASSERT_EQ(message_1_p->messages.length, 1);
    message_2_p = message_1_p->messages.items_pp[0];

    /* messages[0].messages[0].int32s[0..1]. */
    ASSERT_EQ(message_2_p->int32s.length, 2);
    ASSERT_EQ(message_2_p->int32s.items_pp[0]->value, 5);
    ASSERT_EQ(message_2_p->int32s.items_pp[1]->value, 7);

    /* messages[0].bytes. */
    ASSERT_EQ(message_2_p->bytes.length, 0);

    /* messages[1].int32s[0]. */
    message_1_p = message_p->messages.items_pp[1];
    ASSERT_EQ(message_1_p->int32s.length, 1);
    ASSERT_EQ(message_1_p->int32s.items_pp[0]->value, 5);

    /* messages[1]. */
    ASSERT_EQ(message_1_p->messages.length, 0);
    ASSERT_EQ(message_1_p->strings.length, 0);
    ASSERT_EQ(message_1_p->bytes.length, 0);
}

TEST(repeated_nested_decode_out_of_order)
{
    int i;
    int size;
    uint8_t workspace[1024];
    struct repeated_message_t *message_p;
    struct repeated_message_t *message_1_p;
    struct repeated_message_t *message_2_p;
    struct {
        char *encoded_p;
    } datas[] = {
         {
          "\x0a\x03\x01\x02\x03\x1a\x03\x66\x6f\x6f"
          "\x12\x09\x0a\x01\x09\x12\x04\x0a\x02\x05"
          "\x07\x12\x03\x0a\x01\x05\x1a\x03\x62\x61"
          "\x72"
         },
         {
          "\x0a\x03\x01\x02\x03\x1a\x03\x66\x6f\x6f"
          "\x12\x09\x0a\x01\x09\x12\x04\x0a\x02\x05"
          "\x07\x1a\x03\x62\x61\x72\x12\x03\x0a\x01"
          "\x05"
         }
    };

    for (i = 0; i < membersof(datas); i++) {
        message_p = repeated_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        size = repeated_message_decode(message_p,
                                       (uint8_t *)datas[i].encoded_p,
                                       31);
        ASSERT_EQ(size, 31);

        /* int32s[0..2]. */
        ASSERT_EQ(message_p->int32s.length, 3);
        ASSERT_EQ(message_p->int32s.items_pp[0]->value, 1);
        ASSERT_EQ(message_p->int32s.items_pp[1]->value, 2);
        ASSERT_EQ(message_p->int32s.items_pp[2]->value, 3);

        /* strings[0..1]. */
        ASSERT_EQ(message_p->strings.length, 2);
        ASSERT_SUBSTRING(pbtools_get_string(message_p->strings.items_pp[0]),
                         "foo");
        ASSERT_SUBSTRING(pbtools_get_string(message_p->strings.items_pp[1]),
                         "bar");

        /* messages[0]. */
        ASSERT_EQ(message_p->messages.length, 2);
        message_1_p = message_p->messages.items_pp[0];

        /* messages[0].int32s[0]. */
        ASSERT_EQ(message_1_p->int32s.length, 1);
        ASSERT_EQ(message_1_p->int32s.items_pp[0]->value, 9);

        /* messages[0].messages[0]. */
        ASSERT_EQ(message_1_p->messages.length, 1);
        message_2_p = message_1_p->messages.items_pp[0];

        /* messages[0].messages[0].int32s[0..1]. */
        ASSERT_EQ(message_2_p->int32s.length, 2);
        ASSERT_EQ(message_2_p->int32s.items_pp[0]->value, 5);
        ASSERT_EQ(message_2_p->int32s.items_pp[1]->value, 7);

        /* messages[1].int32s[0]. */
        message_1_p = message_p->messages.items_pp[1];
        ASSERT_EQ(message_1_p->int32s.length, 1);
        ASSERT_EQ(message_1_p->int32s.items_pp[0]->value, 5);
    }
}

int main(void)
{
    return RUN_TESTS(
        int32,
        int32_decode,
        int32_message_2,
        int32_decode_out_of_data,
        int32_decode_unknown_field_number,
        int32_decode_duplicated_field_number,
        int32_decode_oveflow,
        int32_decode_seek_out_of_data,
        int64,
        int64_decode,
        sint32,
        sint64,
        uint32,
        uint64,
        fixed32,
        fixed64,
        sfixed32,
        sfixed64,
        float_,
        double_,
        bool_,
        string,
        string_encode_buffer_full,
        string_decode_out_of_data,
        string_embedded_zero_termination,
        bytes,
        enum_,
        address_book,
        address_book_default,
        address_book_default_person,
        address_book_person,
        address_book_decode_issue_1,
        address_book_decode_issue_2,
        address_book_decode_issue_3,
        address_book_decode_issue_4,
        address_book_decode_issue_5,
        address_book_decode_issue_6,
        address_book_decode_issue_7,
        tags_1,
        tags_2,
        tags_3,
        tags_4,
        tags_5,
        tags_6,
        oneof_v1,
        oneof_v2,
        repeated_int32s_one_item,
        repeated_int32s_two_items,
        repeated_int32s_decode_segments,
        repeated_int32s_decode_zero_items,
        repeated_int32s_decode_error_out_of_memory,
        repeated_messages_decode_zero_items,
        repeated_messages_decode_error_too_big,
        repeated_bytes_two_items,
        repeated_bytes_decode_element_of_zero_bytes,
        repeated_nested,
        repeated_nested_decode_out_of_order
    );
}
