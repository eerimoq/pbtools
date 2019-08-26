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
        ASSERT_EQ(memcmp(&encoded[0], datas[i].encoded_p, size), 0);

        message_p = int32_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        size = int32_message_decode(message_p, &encoded[0], size);
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
    ASSERT_EQ(memcmp(&encoded[0], encoded_p, size), 0);

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
    ASSERT_EQ(size, -EOUTOFDATA);

    message_p = int32_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = int32_message_decode(message_p, (uint8_t *)"\x08\x80", 2);
    ASSERT_EQ(size, -EOUTOFDATA);
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
        ASSERT_EQ(memcmp(&encoded[0], datas[i].encoded_p, size), 0);

        message_p = int64_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        size = int64_message_decode(message_p, &encoded[0], size);
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
        ASSERT_EQ(memcmp(&encoded[0], datas[i].encoded_p, size), 0);

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
        ASSERT_EQ(memcmp(&encoded[0], datas[i].encoded_p, size), 0);

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
        ASSERT_EQ(memcmp(&encoded[0], datas[i].encoded_p, size), 0);

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
        ASSERT_EQ(memcmp(&encoded[0], datas[i].encoded_p, size), 0);

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
        ASSERT_EQ(memcmp(&encoded[0], datas[i].encoded_p, size), 0);

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
        ASSERT_EQ(memcmp(&encoded[0], datas[i].encoded_p, size), 0);

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
        ASSERT_EQ(memcmp(&encoded[0], datas[i].encoded_p, size), 0);

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
        ASSERT_EQ(memcmp(&encoded[0], datas[i].encoded_p, size), 0);

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
        ASSERT_EQ(memcmp(&encoded[0], datas[i].encoded_p, size), 0);

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
        ASSERT_EQ(memcmp(&encoded[0], datas[i].encoded_p, size), 0);

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
        ASSERT_EQ(memcmp(&encoded[0], datas[i].encoded_p, size), 0);

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
        char *decoded_p;
        int size;
        const char *encoded_p;
    } datas[] = {
        { "",  0, "" },
        { "1", 3, "\x0a\x01\x31" },
        { "123456789012345678901234567890123456789012345678901234567890"
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
        message_p->value_p = datas[i].decoded_p;
        size = string_message_encode(message_p, &encoded[0], sizeof(encoded));
        ASSERT_EQ(size, datas[i].size);
        ASSERT_EQ(memcmp(&encoded[0], datas[i].encoded_p, size), 0);

        message_p = string_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        size = string_message_decode(message_p, &encoded[0], size);
        ASSERT_EQ(size, datas[i].size);
        ASSERT_EQ(message_p->value_p, datas[i].decoded_p);
    }
}

TEST(string_encode_out_of_memory)
{
    uint8_t encoded[150];
    int size;
    uint8_t workspace[512];
    struct string_message_t *message_p;

    message_p = string_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    message_p->value_p = (
        "123456789012345678901234567890123456789012345678901234567890"
        "123456789012345678901234567890123456789012345678901234567890"
        "123456789012345678901234567890");
    size = string_message_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, -STRING_ENCODE_BUFFER_FULL);
}

TEST(string_decode_out_of_data)
{
    int size;
    uint8_t workspace[512];
    struct string_message_t *message_p;

    message_p = string_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = string_message_decode(message_p, (uint8_t *)"\x0a\x01", 2);
    ASSERT_EQ(size, -EOUTOFDATA);

    message_p = string_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = string_message_decode(message_p,
                                 (uint8_t *)"\x0a\x96\x01\x31",
                                 4);
    ASSERT_EQ(size, -EOUTOFDATA);
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
        ASSERT_EQ(memcmp(&encoded[0], datas[i].encoded_p, size), 0);

        message_p = bytes_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        size = bytes_message_decode(message_p, &encoded[0], size);
        ASSERT_EQ(size, datas[i].size);
        ASSERT_EQ(message_p->value.size, datas[i].decoded_size);
        ASSERT_EQ(memcmp(message_p->value.buf_p,
                         datas[i].decoded_p,
                         datas[i].decoded_size), 0);
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
        ASSERT_EQ(memcmp(&encoded[0], datas[i].encoded_p, size), 0);

        message_p = enum_message_new(&workspace[0], sizeof(workspace));
        ASSERT_NE(message_p, NULL);
        size = enum_message_decode(message_p, &encoded[0], size);
        ASSERT_EQ(size, datas[i].size);
        ASSERT_EQ(message_p->value, datas[i].decoded);
    }
}

TEST(address_book)
{
    uint8_t encoded[75];
    int size;
    uint8_t workspace[512];
    struct address_book_address_book_t *address_book_p;
    struct address_book_person_t *person_p;
    struct address_book_person_phone_number_t *phone_number_p;

    address_book_p = address_book_address_book_new(&workspace[0],
                                                   sizeof(workspace));
    ASSERT_NE(address_book_p, NULL);

    /* Add one person to the address book. */
    ASSERT_EQ(address_book_address_book_people_alloc(address_book_p, 1), 0);
    person_p = &address_book_p->people.items_p[0];
    ASSERT_NE(person_p, NULL);
    person_p->name_p = "Kalle Kula";
    person_p->id = 56;
    person_p->email_p = "kalle.kula@foobar.com";

    /* Add phone numbers. */
    ASSERT_EQ(address_book_person_phones_alloc(person_p, 2), 0);

    /* Home. */
    phone_number_p = &person_p->phones.items_p[0];
    ASSERT_NE(phone_number_p, NULL);
    phone_number_p->number_p = "+46701232345";
    phone_number_p->type = address_book_person_phone_type_home_e;

    /* Work. */
    phone_number_p = &person_p->phones.items_p[1];
    ASSERT_NE(phone_number_p, NULL);
    phone_number_p->number_p = "+46999999999";
    phone_number_p->type = address_book_person_phone_type_work_e;

    /* Encode the message. */
    size = address_book_address_book_encode(address_book_p,
                                            &encoded[0],
                                            sizeof(encoded));
    ASSERT_EQ(size, 75);
    ASSERT_EQ(memcmp(&encoded[0],
                     "\x0a\x49\x0a\x0a\x4b\x61\x6c\x6c\x65\x20"
                     "\x4b\x75\x6c\x61\x10\x38\x1a\x15\x6b\x61"
                     "\x6c\x6c\x65\x2e\x6b\x75\x6c\x61\x40\x66"
                     "\x6f\x6f\x62\x61\x72\x2e\x63\x6f\x6d\x22"
                     "\x10\x0a\x0c\x2b\x34\x36\x37\x30\x31\x32"
                     "\x33\x32\x33\x34\x35\x10\x01\x22\x10\x0a"
                     "\x0c\x2b\x34\x36\x39\x39\x39\x39\x39\x39"
                     "\x39\x39\x39\x10\x02",
                     size), 0);

    /* Decode the message. */
    address_book_p = address_book_address_book_new(&workspace[0],
                                                   sizeof(workspace));
    ASSERT_NE(address_book_p, NULL);
    size = address_book_address_book_decode(address_book_p, &encoded[0], size);
    ASSERT_EQ(size, 75);
    ASSERT_EQ(address_book_p->people.length, 1);

    /* Check the decoded person. */
    person_p = &address_book_p->people.items_p[0];
    ASSERT_NE(person_p, NULL);
    ASSERT_NE(person_p->name_p, NULL);
    ASSERT_SUBSTRING(person_p->name_p, "Kalle Kula");
    ASSERT_EQ(person_p->id, 56);
    ASSERT_NE(person_p->email_p, NULL);
    ASSERT_SUBSTRING(person_p->email_p, "kalle.kula@foobar.com");
    ASSERT_EQ(person_p->phones.length, 2);

    /* Check home phone number. */
    phone_number_p = &person_p->phones.items_p[0];
    ASSERT_SUBSTRING(phone_number_p->number_p, "+46701232345");
    ASSERT_EQ(phone_number_p->type, address_book_person_phone_type_home_e);

    /* Check work phone number. */
    phone_number_p = &person_p->phones.items_p[1];
    ASSERT_SUBSTRING(phone_number_p->number_p, "+46999999999");
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
    ASSERT_EQ(memcmp(&encoded[0], "\x0a\x00", size), 0);

    /* Decode the message. */
    address_book_p = address_book_address_book_new(&workspace[0],
                                                   sizeof(workspace));
    ASSERT_NE(address_book_p, NULL);
    size = address_book_address_book_decode(address_book_p, &encoded[0], size);
    ASSERT_EQ(size, 2);
    ASSERT_EQ(address_book_p->people.length, 1);

    /* Check the decoded person. */
    person_p = &address_book_p->people.items_p[0];
    ASSERT_NE(person_p, NULL);
    ASSERT_NE(person_p->name_p, NULL);
    ASSERT_SUBSTRING(person_p->name_p, "");
    ASSERT_EQ(person_p->id, 0);
    ASSERT_NE(person_p->email_p, NULL);
    ASSERT_SUBSTRING(person_p->email_p, "");
    ASSERT_EQ(person_p->phones.length, 0);
}

TEST(tags_1)
{
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct tags_message_1_t *message_p;

    message_p = tags_message_1_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    message_p->value = true;
    size = tags_message_1_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 2);
    ASSERT_EQ(memcmp(&encoded[0], "\x08\x01", size), 0);

    message_p = tags_message_1_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = tags_message_1_decode(message_p, &encoded[0], size);
    ASSERT_EQ(size, 2);
    ASSERT_EQ(message_p->value, true);
}

TEST(tags_2)
{
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct tags_message_2_t *message_p;

    message_p = tags_message_2_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    message_p->value = true;
    size = tags_message_2_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 2);
    ASSERT_EQ(memcmp(&encoded[0], "\x78\x01", size), 0);

    message_p = tags_message_2_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = tags_message_2_decode(message_p, &encoded[0], size);
    ASSERT_EQ(size, 2);
    ASSERT_EQ(message_p->value, true);
}

TEST(tags_3)
{
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct tags_message_3_t *message_p;

    message_p = tags_message_3_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    message_p->value = true;
    size = tags_message_3_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 3);
    ASSERT_EQ(memcmp(&encoded[0], "\x80\x01\x01", size), 0);

    message_p = tags_message_3_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = tags_message_3_decode(message_p, &encoded[0], size);
    ASSERT_EQ(size, 3);
    ASSERT_EQ(message_p->value, true);
}

TEST(tags_4)
{
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct tags_message_4_t *message_p;

    message_p = tags_message_4_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    message_p->value = true;
    size = tags_message_4_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 3);
    ASSERT_EQ(memcmp(&encoded[0], "\xf8\x7f\x01", size), 0);

    message_p = tags_message_4_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = tags_message_4_decode(message_p, &encoded[0], size);
    ASSERT_EQ(size, 3);
    ASSERT_EQ(message_p->value, true);
}

TEST(tags_5)
{
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct tags_message_5_t *message_p;

    message_p = tags_message_5_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    message_p->value = true;
    size = tags_message_5_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 4);
    ASSERT_EQ(memcmp(&encoded[0], "\x80\x80\x01\x01", size), 0);

    message_p = tags_message_5_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = tags_message_5_decode(message_p, &encoded[0], size);
    ASSERT_EQ(size, 4);
    ASSERT_EQ(message_p->value, true);
}

TEST(tags_6)
{
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct tags_message_6_t *message_p;

    message_p = tags_message_6_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    message_p->value = true;
    size = tags_message_6_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 6);
    ASSERT_EQ(memcmp(&encoded[0], "\xf8\xff\xff\xff\x0f\x01", size), 0);

    message_p = tags_message_6_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = tags_message_6_decode(message_p, &encoded[0], size);
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
    ASSERT_EQ(memcmp(&encoded[0], "\x08\x41", size), 0);

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
    message_p->value.value.v2_p = "Hello!";
    size = oneof_message_encode(message_p, &encoded[0], sizeof(encoded));
    ASSERT_EQ(size, 8);
    ASSERT_EQ(memcmp(&encoded[0], "\x12\x06Hello!", size), 0);

    message_p = oneof_message_new(&workspace[0], sizeof(workspace));
    ASSERT_NE(message_p, NULL);
    size = oneof_message_decode(message_p, &encoded[0], size);
    ASSERT_EQ(size, 8);
    ASSERT_EQ(message_p->value.choice, oneof_message_value_v2_e);
    ASSERT_EQ(strcmp(message_p->value.value.v2_p, "Hello!"), 0);
}

int main(void)
{
    return RUN_TESTS(
        int32,
        int32_message_2,
        int32_decode_out_of_data,
        int64,
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
        string_encode_out_of_memory,
        string_decode_out_of_data,
        bytes,
        enum_,
        address_book,
        address_book_default,
        address_book_default_person,
        tags_1,
        tags_2,
        tags_3,
        tags_4,
        tags_5,
        tags_6,
        oneof_v1,
        oneof_v2
    );
}
