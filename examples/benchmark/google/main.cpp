#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "benchmark.pb.h"

static void fill_message_message1(benchmark::Message *message_p)
{
    benchmark::Message1 *message1_p;
    benchmark::SubMessage *sub_message_p;

    message1_p = message_p->mutable_message1();
    message1_p->set_field80(true);
    message1_p->set_field2(-336);
    message1_p->set_field6(5000);
    message1_p->set_field22(5);
    message1_p->add_field4("The first string");
    message1_p->add_field4("The second string");
    message1_p->add_field4("The third string");
    sub_message_p = message1_p->mutable_field15();
    sub_message_p->set_field1(0);
    sub_message_p->set_field3(9999);
    sub_message_p->set_field15(
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello!");
    sub_message_p->set_field12(
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello!",
        230);
    sub_message_p->set_field21(449932);
    sub_message_p->set_field204(1);
    sub_message_p->set_field300(benchmark::E3);
}

static void encode_message_message1(int iterations)
{
    int i;
    std::string encoded;

    printf("Encoding Message.Message1 %d times...\n", iterations);

    for (i = 0; i < iterations; i++) {
        benchmark::Message message;
        fill_message_message1(&message);
        message.SerializeToString(&encoded);
        assert(encoded.size() == 566);
    }
}

static void decode_message_message1(int iterations)
{
    int i;
    benchmark::Message message;
    std::string encoded;
    bool ok;

    fill_message_message1(&message);
    message.SerializeToString(&encoded);
    assert(encoded.size() == 566);

    printf("Decoding Message.Message1 %d times...\n", iterations);

    for (i = 0; i < iterations; i++) {
        ok = message.ParseFromString(encoded);
        assert(ok);
    }
}

static void fill_message3(benchmark::Message3 *message_p)
{
    benchmark::Message3_SubMessage *sub_message_p;

    sub_message_p = message_p->add_field13();
    sub_message_p->set_field28(7777777);
    sub_message_p->set_field2(-3949833);
    sub_message_p->set_field12(1);
    sub_message_p->set_field19("123");

    sub_message_p = message_p->add_field13();

    sub_message_p = message_p->add_field13();
    sub_message_p->set_field28(1);
    sub_message_p->set_field2(2);
    sub_message_p->set_field12(3);

    sub_message_p = message_p->add_field13();
    sub_message_p->set_field28(7777777);
    sub_message_p->set_field2(-3949833);
    sub_message_p->set_field12(1);
    sub_message_p->set_field19("123088410dhihf9q8hfqouwhfoquwh");

    sub_message_p = message_p->add_field13();
    sub_message_p->set_field28(4493);
    sub_message_p->set_field2(393211234353453ll);
}

static void encode_message3(int iterations)
{
    int i;
    std::string encoded;
    int size;

    printf("Encoding Message3 %d times...\n", iterations);

    for (i = 0; i < iterations; i++) {
        benchmark::Message3 message;
        fill_message3(&message);
        message.SerializeToString(&encoded);
        assert(encoded.size() == 106);
    }
}

static void decode_message3(int iterations)
{
    int i;
    benchmark::Message3 message;
    std::string encoded;
    bool ok;

    fill_message3(&message);
    message.SerializeToString(&encoded);
    assert(encoded.size() == 106);

    printf("Decoding Message3 %d times...\n", iterations);

    for (i = 0; i < iterations; i++) {
        ok = message.ParseFromString(encoded);
        assert(ok);
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
