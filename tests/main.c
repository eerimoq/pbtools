#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define membersof(a) (sizeof(a) / sizeof((a)[0]))

#include "files/c_source/int32.h"
#include "files/c_source/int64.h"
#include "files/c_source/sint32.h"
#include "files/c_source/sint64.h"
#include "files/c_source/address_book.h"

static void test_int32(void)
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

    for (i = 0; i < membersof(datas); i++) {
        printf("Value: %d\n", datas[i].decoded);

        message_p = int32_message_new(&workspace[0], sizeof(workspace));
        assert(message_p != NULL);
        message_p->value = datas[i].decoded;

        size = int32_message_encode(message_p, &encoded[0], sizeof(encoded));
        assert(size == datas[i].size);
        assert(memcmp(&encoded[0], datas[i].encoded_p, size) == 0);

        /* message_p = int32_message_init(&workspace[0], sizeof(workspace)); */
        /* assert(message_p != NULL); */
        /* size = int32_message_decode(message_p, &encoded[0], size); */
        /* assert(size == datas[i].size); */

        /* assert(message_p->value == datas[i].decoded); */
    }
}

static void test_int64(void)
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

    for (i = 0; i < membersof(datas); i++) {
        printf("Value: %lld\n", (long long)datas[i].decoded);

        message_p = int64_message_new(&workspace[0], sizeof(workspace));
        assert(message_p != NULL);
        message_p->value = datas[i].decoded;

        size = int64_message_encode(message_p, &encoded[0], sizeof(encoded));
        assert(size == datas[i].size);
        assert(memcmp(&encoded[0], datas[i].encoded_p, size) == 0);

        /* message_p = int64_message_init(&workspace[0], sizeof(workspace)); */
        /* assert(message_p != NULL); */
        /* size = int64_message_decode(message_p, &encoded[0], size); */
        /* assert(size == datas[i].size); */

        /* assert(message_p->value == datas[i].decoded); */
    }
}

static void test_sint32(void)
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

    for (i = 0; i < membersof(datas); i++) {
        printf("Value: %d\n", datas[i].decoded);

        message_p = sint32_message_new(&workspace[0], sizeof(workspace));
        assert(message_p != NULL);
        message_p->value = datas[i].decoded;

        size = sint32_message_encode(message_p, &encoded[0], sizeof(encoded));
        assert(size == datas[i].size);
        assert(memcmp(&encoded[0], datas[i].encoded_p, size) == 0);

        /* message_p = sint32_message_init(&workspace[0], sizeof(workspace)); */
        /* assert(message_p != NULL); */
        /* size = sint32_message_decode(message_p, &encoded[0], size); */
        /* assert(size == datas[i].size); */

        /* assert(message_p->value == datas[i].decoded); */
    }
}

static void test_sint64(void)
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

    for (i = 0; i < membersof(datas); i++) {
        printf("Value: %lld\n", (long long)datas[i].decoded);

        message_p = sint64_message_new(&workspace[0], sizeof(workspace));
        assert(message_p != NULL);
        message_p->value = datas[i].decoded;

        size = sint64_message_encode(message_p, &encoded[0], sizeof(encoded));
        assert(size == datas[i].size);
        assert(memcmp(&encoded[0], datas[i].encoded_p, size) == 0);

        /* message_p = sint64_message_init(&workspace[0], sizeof(workspace)); */
        /* assert(message_p != NULL); */
        /* size = sint64_message_decode(message_p, &encoded[0], size); */
        /* assert(size == datas[i].size); */

        /* assert(message_p->value == datas[i].decoded); */
    }
}

int main(void)
{
    test_int32();
    test_int64();
    test_sint32();
    test_sint64();
}
