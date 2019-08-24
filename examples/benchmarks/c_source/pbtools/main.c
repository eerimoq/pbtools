#include <assert.h>
#include <string.h>

#include "benchmark.h"

int main(int argc, const char *argv[])
{
    int i;
    struct benchmark_benchmark_t *benchmark_p;
    uint8_t encoded[256];
    uint8_t workspace[64];

    for (i = 0; i < atoi(argv[1]); i++) {
        benchmark_p = benchmark_benchmark_new(&workspace[0], sizeof(workspace));
        benchmark_p->v1 = 1000000;
        assert(benchmark_benchmark_encode(benchmark_p,
                                          &encoded[0],
                                          sizeof(encoded)) == 4);
        //assert(memcmp(&encoded[0], "\x08\xc0\x84\x3d", 4) == 0);
    }

    return (0);
}
