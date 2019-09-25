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

static void decode_message_message1(int iterations)
{
    int i;

    flatbuffers::FlatBufferBuilder builder;
    fill_message_message1(builder);
    assert(builder.GetSize() == 720);

    printf("Decoding Message.Message1 %d times...\n", iterations);

    for (i = 0; i < iterations; i++) {
        auto message = flatbuffers::GetRoot<benchmark::Message>(builder.GetBufferPointer());
        assert(message->oneof_type() == benchmark::Oneof_message1);
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

static void decode_message3(int iterations)
{
    int i;

    flatbuffers::FlatBufferBuilder builder;
    fill_message3(builder);
    assert(builder.GetSize() == 240);

    printf("Decoding Message3 %d times...\n", iterations);

    for (i = 0; i < iterations; i++) {
        auto message3 = flatbuffers::GetRoot<benchmark::Message3>(
            builder.GetBufferPointer());
        assert(message3->field13()->Get(0)->field28() == 7777777);
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
