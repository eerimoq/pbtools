#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define membersof(a) (sizeof(a) / sizeof((a)[0]))

#include "files/c_source/int32.h"
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

int main(void)
{
    test_int32();
}
