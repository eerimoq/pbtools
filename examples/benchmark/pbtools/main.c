#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "benchmark.h"

static void fill_message(struct benchmark_message_t *message_p)
{
    benchmark_message_oneof_message1_init(message_p);
    message_p->oneof.value.message1.field80 = true;
    message_p->oneof.value.message1.field2 = -336;
    message_p->oneof.value.message1.field6 = 5000;
    message_p->oneof.value.message1.field22 = 5;
    benchmark_message1_field4_alloc(&message_p->oneof.value.message1, 3);
    message_p->oneof.value.message1.field4.items_pp[0]->value_p = "The first string";
    message_p->oneof.value.message1.field4.items_pp[1]->value_p = "The second string";
    message_p->oneof.value.message1.field4.items_pp[2]->value_p = "The third string";
    message_p->oneof.value.message1.field15.field1 = 0;
    message_p->oneof.value.message1.field15.field3 = 9999;
    message_p->oneof.value.message1.field15.field15_p = (
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello!");
    message_p->oneof.value.message1.field15.field12.buf_p = (uint8_t *)(
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello!");
    message_p->oneof.value.message1.field15.field12.size = 230;
    message_p->oneof.value.message1.field15.field21 = 449932;
    message_p->oneof.value.message1.field15.field204 = 1;
    message_p->oneof.value.message1.field15.field300 = benchmark_enum_e3_e;
}

static void encode(int iterations)
{
    int i;
    struct benchmark_message_t *message_p;
    uint8_t encoded[1024];
    uint8_t workspace[1024];
    int size;

    printf("Encoding %d times...\n", iterations);
    
    for (i = 0; i < iterations; i++) {
        message_p = benchmark_message_new(&workspace[0], sizeof(workspace));
        fill_message(message_p);
        size = benchmark_message_encode(message_p, &encoded[0], sizeof(encoded));
        assert(size == 566);
    }
}

static void decode(int iterations)
{
    int i;
    struct benchmark_message_t *message_p;
    uint8_t encoded[1024];
    uint8_t workspace[1024];
    int size;

    message_p = benchmark_message_new(&workspace[0], sizeof(workspace));
    fill_message(message_p);
    size = benchmark_message_encode(message_p, &encoded[0], sizeof(encoded));
    assert(size == 566);

    printf("Decoding %d times...\n", iterations);

    for (i = 0; i < iterations; i++) {
        message_p = benchmark_message_new(&workspace[0], sizeof(workspace));
        benchmark_message_decode(message_p, &encoded[0], size);
    }
}

int main(int argc, const char *argv[])
{
    int iterations;

    if (argc != 3) {
        return (1);
    }
    
    iterations = atoi(argv[2]);

    if (strcmp(argv[1], "encode") == 0) {
        encode(iterations);
    } else {
        decode(iterations);
    }

    return (0);
}
