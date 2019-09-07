#include <assert.h>
#include <stdlib.h>

#include "pb_encode.h"
#include "benchmark.pb.h"

int main(int argc, const char *argv[])
{
    int i;
    benchmarks_proto3_GoogleMessage1 message =
        benchmarks_proto3_GoogleMessage1_init_default;
    uint8_t encoded[256];
    pb_ostream_t stream;

    for (i = 0; i < atoi(argv[1]); i++) {
        //benchmark.v1 = 1000000;
        stream = pb_ostream_from_buffer(encoded, sizeof(encoded));
        assert(pb_encode(&stream, benchmarks_proto3_GoogleMessage1_fields, &message));
        //assert(stream.bytes_written == 4);
        //assert(memcmp(&encoded[0], "\x08\xc0\x84\x3d", 4) == 0);
    }

    return (0);
}
