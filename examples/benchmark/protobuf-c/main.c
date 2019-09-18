#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "benchmark.pb-c.h"

static void fill_message_message1(Benchmark__Message *message_p,
                                  Benchmark__Message1 *message1_p,
                                  Benchmark__SubMessage *sub_message_p,
                                  char **field4_pp)
{
    message_p->oneof_case = BENCHMARK__MESSAGE__ONEOF_MESSAGE1;
    message_p->message1 = message1_p;
    message1_p->field80 = 1;
    message1_p->field2 = -336;
    message1_p->field6 = 5000;
    message1_p->field22 = 5;
    message1_p->n_field4 = 3;
    message1_p->field4 = field4_pp;
    field4_pp[0] = "The first string";
    field4_pp[1] = "The second string";
    field4_pp[2] = "The third string";
    message1_p->field15 = sub_message_p;
    sub_message_p->field1 = 0;
    sub_message_p->field3 = 9999;
    sub_message_p->field15 = (
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello!");
    sub_message_p->field12.data = (uint8_t *)(
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello!");
    sub_message_p->field12.len = 230;
    sub_message_p->field21 = 449932;
    sub_message_p->field204 = 1;
    sub_message_p->field300 = BENCHMARK__ENUM__E3;
}

static void encode_message_message1(int iterations)
{
    int i;
    uint8_t encoded[1024];
    int size;

    printf("Encoding Message.Message1 %d times...\n", iterations);

    for (i = 0; i < iterations; i++) {
        Benchmark__Message message = BENCHMARK__MESSAGE__INIT;
        Benchmark__Message1 message1 = BENCHMARK__MESSAGE1__INIT;
        Benchmark__SubMessage sub_message;
        char *field4[3];
        ProtobufCBufferSimple buffer = PROTOBUF_C_BUFFER_SIMPLE_INIT(encoded);

        benchmark__sub_message__init(&sub_message);
        fill_message_message1(&message, &message1, &sub_message, &field4[0]);
        size = benchmark__message__pack_to_buffer(&message, &buffer.base);
        assert(size == 566);
    }
}

static void decode_message_message1(int iterations)
{
    int i;
    Benchmark__Message *message_p;
    Benchmark__Message message = BENCHMARK__MESSAGE__INIT;
    Benchmark__Message1 message1 = BENCHMARK__MESSAGE1__INIT;
    Benchmark__SubMessage sub_message;
    char *field4[3];
    uint8_t encoded[1024];
    ProtobufCBufferSimple buffer = PROTOBUF_C_BUFFER_SIMPLE_INIT(encoded);
    int size;

    benchmark__sub_message__init(&sub_message);
    fill_message_message1(&message, &message1, &sub_message, &field4[0]);
    size = benchmark__message__pack_to_buffer(&message, &buffer.base);
    assert(size == 566);

    printf("Decoding Message.Message1 %d times...\n", iterations);

    for (i = 0; i < iterations; i++) {
        message_p = benchmark__message__unpack(NULL, 566, &encoded[0]);
        assert(message_p != NULL);
        benchmark__message__free_unpacked(message_p, NULL);
    }
}

static void fill_message3(Benchmark__Message3 *message_p,
                          Benchmark__Message3__SubMessage **sub_messages_pp)
{
    message_p->n_field13 = 5;
    message_p->field13 = sub_messages_pp;

    sub_messages_pp[0]->field28 = 7777777;
    sub_messages_pp[0]->field2 = -3949833;
    sub_messages_pp[0]->field12 = 1;
    sub_messages_pp[0]->field19 = "123";

    sub_messages_pp[2]->field28 = 1;
    sub_messages_pp[2]->field2 = 2;
    sub_messages_pp[2]->field12 = 3;

    sub_messages_pp[3]->field28 = 7777777;
    sub_messages_pp[3]->field2 = -3949833;
    sub_messages_pp[3]->field12 = 1;
    sub_messages_pp[3]->field19 = "123088410dhihf9q8hfqouwhfoquwh";

    sub_messages_pp[4]->field28 = 4493;
    sub_messages_pp[4]->field2 = 393211234353453ll;
}

static void encode_message3(int iterations)
{
    int i;
    uint8_t encoded[512];
    int size;

    printf("Encoding Message3 %d times...\n", iterations);

    for (i = 0; i < iterations; i++) {
        Benchmark__Message3 message = BENCHMARK__MESSAGE3__INIT;
        Benchmark__Message3__SubMessage sub_messages[5];
        Benchmark__Message3__SubMessage *sub_messages_p[5];
        ProtobufCBufferSimple buffer = PROTOBUF_C_BUFFER_SIMPLE_INIT(encoded);

        benchmark__message3__sub_message__init(&sub_messages[0]);
        sub_messages_p[0] = &sub_messages[0];
        benchmark__message3__sub_message__init(&sub_messages[1]);
        sub_messages_p[1] = &sub_messages[1];
        benchmark__message3__sub_message__init(&sub_messages[2]);
        sub_messages_p[2] = &sub_messages[2];
        benchmark__message3__sub_message__init(&sub_messages[3]);
        sub_messages_p[3] = &sub_messages[3];
        benchmark__message3__sub_message__init(&sub_messages[4]);
        sub_messages_p[4] = &sub_messages[4];

        fill_message3(&message, &sub_messages_p[0]);
        size = benchmark__message3__pack_to_buffer(&message, &buffer.base);
        assert(size == 106);
    }
}

static void decode_message3(int iterations)
{
    int i;
    Benchmark__Message3 *message_p;
    Benchmark__Message3 message = BENCHMARK__MESSAGE3__INIT;
    Benchmark__Message3__SubMessage sub_messages[5];
    Benchmark__Message3__SubMessage *sub_messages_p[5];
    uint8_t encoded[1024];
    int size;
    ProtobufCBufferSimple buffer = PROTOBUF_C_BUFFER_SIMPLE_INIT(encoded);

    benchmark__message3__sub_message__init(&sub_messages[0]);
    sub_messages_p[0] = &sub_messages[0];
    benchmark__message3__sub_message__init(&sub_messages[1]);
    sub_messages_p[1] = &sub_messages[1];
    benchmark__message3__sub_message__init(&sub_messages[2]);
    sub_messages_p[2] = &sub_messages[2];
    benchmark__message3__sub_message__init(&sub_messages[3]);
    sub_messages_p[3] = &sub_messages[3];
    benchmark__message3__sub_message__init(&sub_messages[4]);
    sub_messages_p[4] = &sub_messages[4];

    fill_message3(&message, &sub_messages_p[0]);
    size = benchmark__message3__pack_to_buffer(&message, &buffer.base);
    assert(size == 106);

    printf("Decoding Message3 %d times...\n", iterations);

    for (i = 0; i < iterations; i++) {
        message_p = benchmark__message3__unpack(NULL, 106, &encoded[0]);
        assert(message_p != NULL);
        benchmark__message3__free_unpacked(message_p, NULL);
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
