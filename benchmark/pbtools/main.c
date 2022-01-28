#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "benchmark.h"

static void fill_message_message1(struct benchmark_message_t *message_p)
{
    benchmark_message_message1_alloc(message_p);
    message_p->message1_p->field80 = true;
    message_p->message1_p->field2 = -336;
    message_p->message1_p->field6 = 5000;
    message_p->message1_p->field22 = 5;
    benchmark_message1_field4_alloc(message_p->message1_p, 3);
    message_p->message1_p->field4.items_pp[0] = "The first string";
    message_p->message1_p->field4.items_pp[1] = "The second string";
    message_p->message1_p->field4.items_pp[2] = "The third string";
    benchmark_message1_field15_alloc(message_p->message1_p);
    message_p->message1_p->field15_p->field1 = 0;
    message_p->message1_p->field15_p->field3 = 9999;
    message_p->message1_p->field15_p->field15_p = (
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello!");
    message_p->message1_p->field15_p->field12.buf_p = (uint8_t *)(
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello!");
    message_p->message1_p->field15_p->field12.size = 230;
    message_p->message1_p->field15_p->field21 = 449932;
    message_p->message1_p->field15_p->field204 = 1;
    message_p->message1_p->field15_p->field300 = benchmark_enum_e3_e;
}

static void encode_message_message1(int iterations)
{
    int i;
    struct benchmark_message_t *message_p;
    uint8_t encoded[1024];
    uint8_t workspace[1024];
    int size;

    printf("Encoding Message.Message1 %d times...\n", iterations);

    for (i = 0; i < iterations; i++) {
        message_p = benchmark_message_new(&workspace[0], sizeof(workspace));
        fill_message_message1(message_p);
        size = benchmark_message_encode(message_p, &encoded[0], sizeof(encoded));
        assert(size == 566);
    }
}

static void decode_message_message1(int iterations)
{
    int i;
    struct benchmark_message_t *message_p;
    uint8_t encoded[1024];
    uint8_t workspace[1024];
    int size;

    message_p = benchmark_message_new(&workspace[0], sizeof(workspace));
    fill_message_message1(message_p);
    size = benchmark_message_encode(message_p, &encoded[0], sizeof(encoded));
    assert(size == 566);

    printf("Decoding Message.Message1 %d times...\n", iterations);

    for (i = 0; i < iterations; i++) {
        message_p = benchmark_message_new(&workspace[0], sizeof(workspace));
        size = benchmark_message_decode(message_p, &encoded[0], 566);
        assert(size == 566);
    }
}

static void fill_message3(struct benchmark_message3_t *message_p)
{
    benchmark_message3_field13_alloc(message_p, 5);

    message_p->field13.items_p[0].field28 = 7777777;
    message_p->field13.items_p[0].field2 = -3949833;
    message_p->field13.items_p[0].field12 = 1;
    message_p->field13.items_p[0].field19_p = "123";

    message_p->field13.items_p[2].field28 = 1;
    message_p->field13.items_p[2].field2 = 2;
    message_p->field13.items_p[2].field12 = 3;

    message_p->field13.items_p[3].field28 = 7777777;
    message_p->field13.items_p[3].field2 = -3949833;
    message_p->field13.items_p[3].field12 = 1;
    message_p->field13.items_p[3].field19_p = "123088410dhihf9q8hfqouwhfoquwh";

    message_p->field13.items_p[4].field28 = 4493;
    message_p->field13.items_p[4].field2 = 393211234353453ll;
}

static void encode_message3(int iterations)
{
    int i;
    struct benchmark_message3_t *message_p;
    uint8_t encoded[512];
    uint8_t workspace[512];
    int size;

    printf("Encoding Message3 %d times...\n", iterations);

    for (i = 0; i < iterations; i++) {
        message_p = benchmark_message3_new(&workspace[0], sizeof(workspace));
        fill_message3(message_p);
        size = benchmark_message3_encode(message_p, &encoded[0], sizeof(encoded));
        assert(size == 106);
    }
}

static void decode_message3(int iterations)
{
    int i;
    struct benchmark_message3_t *message_p;
    uint8_t encoded[1024];
    uint8_t workspace[1024];
    int size;

    message_p = benchmark_message3_new(&workspace[0], sizeof(workspace));
    fill_message3(message_p);
    size = benchmark_message3_encode(message_p, &encoded[0], sizeof(encoded));
    assert(size == 106);

    printf("Decoding Message3 %d times...\n", iterations);

    for (i = 0; i < iterations; i++) {
        message_p = benchmark_message3_new(&workspace[0], sizeof(workspace));
        size = benchmark_message3_decode(message_p, &encoded[0], 106);
        assert(size == 106);
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
        encode_message_message1(iterations);
        encode_message3(iterations);
    } else {
        decode_message_message1(iterations);
        decode_message3(iterations);
    }

    return (0);
}
