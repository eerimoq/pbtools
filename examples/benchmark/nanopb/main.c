#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "pb_encode.h"
#include "pb_decode.h"
#include "benchmark.pb.h"

static bool encode_message1_field4(pb_ostream_t *stream_p,
                                   const pb_field_t *field_p,
                                   void * const *arg_pp)
{
    char *str[3] = {
        "The first string",
        "The second string",
        "The third string"
    };
    int i;

    for (i = 0; i < 3; i++) {
        if (!pb_encode_tag_for_field(stream_p, field_p)) {
            return (false);
        }

        if (!pb_encode_string(stream_p, (uint8_t*)str[i], strlen(str[i]))) {
            return (false);
        }
    }

    return (true);
}

static bool encode_message1_field15_field15(pb_ostream_t *stream_p,
                                            const pb_field_t *field_p,
                                            void * const *arg_pp)
{
    char *str_p =
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello!";

    if (!pb_encode_tag_for_field(stream_p, field_p)) {
        return (false);
    }

    return (pb_encode_string(stream_p, (uint8_t*)str_p, strlen(str_p)));
}

static bool encode_message1_field15_field12(pb_ostream_t *stream_p,
                                            const pb_field_t *field_p,
                                            void * const *arg_pp)
{
    uint8_t *buf_p = (uint8_t *)(
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! Hello! "
        "Hello! Hello! Hello!");

    if (!pb_encode_tag_for_field(stream_p, field_p)) {
        return (false);
    }

    return (pb_encode_string(stream_p, buf_p, 230));
}

static void fill_message(benchmark_Message *message_p)
{
    message_p->message1.field80 = true;
    message_p->message1.field2 = -336;
    message_p->message1.field6 = 5000;
    message_p->message1.field22 = 5;
    message_p->message1.field4.funcs.encode = encode_message1_field4;
    message_p->message1.field15.field1 = 0;
    message_p->message1.field15.field3 = 9999;
    message_p->message1.field15.field15.funcs.encode =
        encode_message1_field15_field15;
    message_p->message1.field15.field12.funcs.encode =
        encode_message1_field15_field12;
    message_p->message1.field15.field21 = 449932;
    message_p->message1.field15.field204 = 1;
    message_p->message1.field15.field300 = benchmark_Enum_E5;
}

static void encode(int iterations)
{
    bool ok;
    int i;
    uint8_t encoded[1024];
    pb_ostream_t stream;

    printf("Encoding %d times...\n", iterations);

    for (i = 0; i < iterations; i++) {
        benchmark_Message message = benchmark_Message_init_default;

        fill_message(&message);
        stream = pb_ostream_from_buffer(encoded, sizeof(encoded));
        ok = pb_encode(&stream, benchmark_Message_fields, &message);
        assert(ok);
        assert(stream.bytes_written == 566);
    }
}

static bool decode_message1_field4(pb_istream_t *stream_p,
                                   const pb_field_t *field_p,
                                   void **arg_pp)
{
    uint8_t buffer[1024];
    int size;

    size = stream_p->bytes_left;

    if (size > sizeof(buffer) - 1) {
        return (false);
    }

    if (!pb_read(stream_p, &buffer[0], size)) {
        return (false);
    }

    buffer[size] = '\0';

    return (true);
}

static bool decode_message1_field15_field15(pb_istream_t *stream_p,
                                            const pb_field_t *field_p,
                                            void **arg_pp)
{
    uint8_t buffer[1024];
    int size;

    size = stream_p->bytes_left;

    if (size > sizeof(buffer) - 1) {
        return (false);
    }

    if (!pb_read(stream_p, &buffer[0], size)) {
        return (false);
    }

    buffer[size] = '\0';

    return (true);
}

static bool decode_message1_field15_field12(pb_istream_t *stream_p,
                                            const pb_field_t *field_p,
                                            void **arg_pp)
{
    uint8_t buffer[1024];
    int size;

    size = stream_p->bytes_left;

    if (size > sizeof(buffer) - 1) {
        return (false);
    }

    if (!pb_read(stream_p, &buffer[0], size)) {
        return (false);
    }

    return (true);
}

static void decode(int iterations)
{
    bool ok;
    int i;
    benchmark_Message emessage = benchmark_Message_init_default;
    uint8_t encoded[1024];
    pb_ostream_t ostream;
    pb_istream_t istream;

    ostream = pb_ostream_from_buffer(encoded, sizeof(encoded));
    fill_message(&emessage);
    ok = pb_encode(&ostream, benchmark_Message_fields, &emessage);
    assert(ok);
    assert(ostream.bytes_written == 566);

    printf("Decoding %d times...\n", iterations);

    for (i = 0; i < iterations; i++) {
        benchmark_Message message = benchmark_Message_init_default;

        message.message1.field4.funcs.decode = decode_message1_field4;
        message.message1.field15.field15.funcs.decode =
            decode_message1_field15_field15;
        message.message1.field15.field12.funcs.decode =
            decode_message1_field15_field12;

        istream = pb_istream_from_buffer(encoded, ostream.bytes_written);
        ok = pb_decode(&istream, benchmark_Message_fields, &message);
        assert(ok);
        assert(istream.bytes_left == 0);
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
