#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "int32.h"

static void test_int32(void)
{
    uint8_t encoded[128];
    int size;
    uint8_t workspace[512];
    struct int32_message_t *message_p;

    message_p = int32_message_init(&workspace[0], sizeof(workspace));
    assert(message_p != NULL);

    size = int32_message_encode(message_p, &encoded[0], sizeof(encoded));
    assert(size == 0);

    message_p = int32_message_init(&workspace[0], sizeof(workspace));
    assert(message_p != NULL);
    size = int32_message_decode(message_p, &encoded[0], size);
    assert(size == 0);

    assert(message_p->value == 0);

    return (0);
}

int main(void)
{
    test_int32();
}
