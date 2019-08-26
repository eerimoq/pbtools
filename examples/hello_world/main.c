#include <string.h>
#include <stdio.h>
#include "hello_world.h"

int main(int argc, const char *argv[])
{
    int size;
    uint8_t workspace[64];
    uint8_t encoded[16];
    struct hello_world_foo_t *foo_p;

    /* Encode. */
    foo_p = hello_world_foo_new(&workspace[0], sizeof(workspace));

    if (foo_p != NULL) {
        foo_p->bar = 78;
        size = hello_world_foo_encode(foo_p, &encoded[0], sizeof(encoded));

        if (size >= 0) {
            printf("Successfully encoded Foo into %d bytes.\n", size);
        }
    }

    /* Decode. */
    foo_p = hello_world_foo_new(&workspace[0], sizeof(workspace));

    if (foo_p != NULL) {
        size = hello_world_foo_decode(foo_p, &encoded[0], size);

        if (size >= 0) {
            printf("Successfully decoded %d bytes into Foo.\n", size);
            printf("Foo.bar: %d\n", foo_p->bar);
        }
    }

    return (0);
}
