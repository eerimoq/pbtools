#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "benchmark_generated.h"

static void fill_message_message1(flatbuffers::FlatBufferBuilder &builder)
{
    auto field15 = builder.CreateString(
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello!");
    auto field12 = builder.CreateVector((unsigned char *)
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello!",
        230);

    benchmark::SubMessageBuilder subMessageBuilder(builder);

    subMessageBuilder.add_field1(0);
    subMessageBuilder.add_field3(9999);
    subMessageBuilder.add_field15(field15);
    subMessageBuilder.add_field12(field12);
    subMessageBuilder.add_field21(449932);
    subMessageBuilder.add_field204(1);
    subMessageBuilder.add_field300(benchmark::Enum_E3);
    auto subMessage = subMessageBuilder.Finish();

    std::vector<flatbuffers::Offset<flatbuffers::String>> items;
    items.push_back(builder.CreateString("The first string"));
    items.push_back(builder.CreateString("The second string"));
    items.push_back(builder.CreateString("The third string"));
    auto field4 = builder.CreateVector(items);

    benchmark::Message1Builder message1Builder(builder);
    message1Builder.add_field80(true);
    message1Builder.add_field2(-336);
    message1Builder.add_field6(5000);
    message1Builder.add_field22(5);
    message1Builder.add_field4(field4);
    message1Builder.add_field15(subMessage);
    auto message1 = message1Builder.Finish();

    auto message = CreateMessage(builder,
                                 benchmark::Oneof_message1,
                                 message1.Union());

    builder.Finish(message);
}

static void encode_message_message1(int iterations)
{
    int i;

    printf("Encoding Message.Message1 %d times...\n", iterations);

    for (i = 0; i < iterations; i++) {
        flatbuffers::FlatBufferBuilder builder;
        fill_message_message1(builder);
        assert(builder.GetSize() == 720);
    }
}

static void assert_message_message1(const benchmark::Message *message_p)
{
    assert(message_p->oneof_type() == benchmark::Oneof_message1);

    auto message1_p = message_p->oneof_as_message1();
    assert(message1_p->field80() == true);
    assert(message1_p->field81() == false);
    assert(message1_p->field2() == -336);
    assert(message1_p->field3() == 0);
    assert(message1_p->field6() == 5000);
    assert(message1_p->field22() == 5);
    assert(message1_p->field4()->size() == 3);
    assert(message1_p->field4()->Get(0) != nullptr);
    assert(message1_p->field4()->Get(0)->c_str()[0] == 'T');
    assert(message1_p->field4()->Get(1) != nullptr);
    assert(message1_p->field4()->Get(1)->c_str()[0] == 'T');
    assert(message1_p->field4()->Get(2) != nullptr);
    assert(message1_p->field4()->Get(2)->c_str()[0] == 'T');
    assert(message1_p->field59() == false);
    assert(message1_p->field16() == 0);
    assert(message1_p->field150() == 0);
    assert(message1_p->field23() == 0);
    assert(message1_p->field24() == false);
    assert(message1_p->field25() == 0);
    auto subMessage_p = message1_p->field15();
    assert(subMessage_p->field1() == 0);
    assert(subMessage_p->field2() == 0);
    assert(subMessage_p->field3() == 9999);
    assert(subMessage_p->field15() != nullptr);
    assert(subMessage_p->field15()->c_str()[0] == 'H');
    assert(subMessage_p->field12() != nullptr);
    assert(subMessage_p->field12()->Get(0) == 'H');
    assert(subMessage_p->field13() == 0);
    assert(subMessage_p->field14() == 0);
    assert(subMessage_p->field21() == 449932);
    assert(subMessage_p->field22() == 0);
    assert(subMessage_p->field206() == false);
    assert(subMessage_p->field203() == 0);
    assert(subMessage_p->field204() == 1);
    assert(subMessage_p->field205() == nullptr);
    assert(subMessage_p->field207() == 0);
    assert(subMessage_p->field300() == benchmark::Enum_E3);
    assert(message1_p->field78() == false);
    assert(message1_p->field67() == 0);
    assert(message1_p->field68() == 0);
    assert(message1_p->field128() == 0);
    assert(message1_p->field129() == nullptr);
    assert(message1_p->field131() == 0);
}

static void decode_message_message1(int iterations)
{
    int i;

    flatbuffers::FlatBufferBuilder builder;
    fill_message_message1(builder);
    assert(builder.GetSize() == 720);

    printf("Decoding Message.Message1 %d times...\n", iterations);

    for (i = 0; i < iterations; i++) {
        auto message_p = flatbuffers::GetRoot<benchmark::Message>(
            builder.GetBufferPointer());
        assert_message_message1(message_p);
    }
}

static void fill_message3(flatbuffers::FlatBufferBuilder &builder)
{
    std::vector<flatbuffers::Offset<benchmark::SubMessage2>> items;

    items.push_back(
        benchmark::CreateSubMessage2(builder,
                                     7777777,
                                     -3949833,
                                     1,
                                     builder.CreateString("123")));
    items.push_back(benchmark::CreateSubMessage2(builder));
    items.push_back(
        benchmark::CreateSubMessage2(builder,
                                     1,
                                     2,
                                     3));
    items.push_back(
        benchmark::CreateSubMessage2(
            builder,
            7777777,
            -3949833,
            1,
            builder.CreateString("123088410dhihf9q8hfqouwhfoquwh")));
    items.push_back(
        benchmark::CreateSubMessage2(builder,
                                     4493,
                                     393211234353453ll));

    auto message3 = CreateMessage3(builder, builder.CreateVector(items));
    builder.Finish(message3);
}

static void encode_message3(int iterations)
{
    int i;
    std::string encoded;
    int size;

    printf("Encoding Message3 %d times...\n", iterations);

    for (i = 0; i < iterations; i++) {
        flatbuffers::FlatBufferBuilder builder;
        fill_message3(builder);
        assert(builder.GetSize() == 240);
    }
}

static void assert_message3(const benchmark::Message3 *message3_p)
{
        assert(message3_p->field13()->size() == 5);

        auto item_p = message3_p->field13()->Get(0);
        assert(item_p->field28() == 7777777);
        assert(item_p->field2() == -3949833);
        assert(item_p->field12() == 1);
        assert(item_p->field19() != nullptr);
        assert(item_p->field19()->c_str()[0] == '1');
        assert(item_p->field11() == 0);

        item_p = message3_p->field13()->Get(1);
        assert(item_p->field28() == 0);
        assert(item_p->field2() == 0);
        assert(item_p->field12() == 0);
        assert(item_p->field19() == nullptr);
        assert(item_p->field11() == 0);

        item_p = message3_p->field13()->Get(2);
        assert(item_p->field28() == 1);
        assert(item_p->field2() == 2);
        assert(item_p->field12() == 3);
        assert(item_p->field19() == nullptr);
        assert(item_p->field11() == 0);

        item_p = message3_p->field13()->Get(3);
        assert(item_p->field28() == 7777777);
        assert(item_p->field2() == -3949833);
        assert(item_p->field12() == 1);
        assert(item_p->field19() != nullptr);
        assert(item_p->field19()->c_str()[0] == '1');
        assert(item_p->field11() == 0);

        item_p = message3_p->field13()->Get(4);
        assert(item_p->field28() == 4493);
        assert(item_p->field2() == 393211234353453ll);
        assert(item_p->field12() == 0);
        assert(item_p->field19() == nullptr);
        assert(item_p->field11() == 0);
}

static void decode_message3(int iterations)
{
    int i;

    flatbuffers::FlatBufferBuilder builder;
    fill_message3(builder);
    assert(builder.GetSize() == 240);

    printf("Decoding Message3 %d times...\n", iterations);

    for (i = 0; i < iterations; i++) {
        auto message3_p = flatbuffers::GetRoot<benchmark::Message3>(
            builder.GetBufferPointer());
        assert_message3(message3_p);
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
