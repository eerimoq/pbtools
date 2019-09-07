#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "benchmark.h"

int main(int argc, const char *argv[])
{
    int i;
    int j;
    struct benchmarks_proto3_google_message1_t *message_p;
    uint8_t encoded[256];
    uint8_t workspace[1024];

    for (i = 0; i < atoi(argv[1]); i++) {
        message_p = benchmarks_proto3_google_message1_new(&workspace[0],
                                                          sizeof(workspace));

        message_p->field2 = 8;
        message_p->field3 = 2066379;
        pbtools_set_string(&message_p->field4, "3K+6)#");
        pbtools_set_string(
            &message_p->field9,
            "10)2uiSuoXL1^)v}icF@>P(j<t#~tz\\lg??S&(<hr7EVs\'l{\'5`Gohc_(="
            "t eS s{_I?iCwaG]L\'*Pu5(&w_:4{~Z");
        message_p->field12 = true;
        message_p->field14 = true;
        message_p->field15.field1 = 25;
        message_p->field15.field2 = 36;
        pbtools_set_string(
            &message_p->field15.field15,
            "\"?6PY4]L2c<}~2;\\TVF_w^[@YfbIc*v/N+Z-oYuaWZr4C;5ib|*s@RC"
            "BbuvrQ3g(k,N");
        message_p->field15.field21 = 2813090458170031956;
        message_p->field15.field22 = 38;
        message_p->field15.field23 = true;
        pbtools_set_string(&message_p->field18, "{=Qwfe~#n{");
        message_p->field67 = 1591432;
        message_p->field100 = 31;

        assert(benchmarks_proto3_google_message1_encode(message_p,
                                                        &encoded[0],
                                                        sizeof(encoded)) == 221);

        message_p = benchmarks_proto3_google_message1_new(&workspace[0],
                                                          sizeof(workspace));
        assert(benchmarks_proto3_google_message1_decode(message_p,
                                                        &encoded[0],
                                                        221) == 221);
    }

    return (0);
}
