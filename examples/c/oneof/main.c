#include <stdio.h>
#include "oneof.h"

int main(int argc, const char *argv[])
{
    int size;
    uint8_t workspace[64];
    uint8_t encoded[16];
    struct oneof_foo_t *foo_p;

    /* Encode. */
    foo_p = oneof_foo_new(&workspace[0], sizeof(workspace));

    if (foo_p == NULL) {
        return (1);
    }

    oneof_foo_bar_fie_init(foo_p);
    foo_p->bar.value.fie = 789;
    size = oneof_foo_encode(foo_p, &encoded[0], sizeof(encoded));

    if (size < 0) {
        return (2);
    }

    printf("Successfully encoded Foo into %d bytes.\n", size);

    /* Decode. */
    foo_p = oneof_foo_new(&workspace[0], sizeof(workspace));

    if (foo_p == NULL) {
        return (3);
    }

    size = oneof_foo_decode(foo_p, &encoded[0], size);

    if (size < 0) {
        return (4);
    }

    printf("Successfully decoded %d bytes into Foo.\n", size);

    switch (foo_p->bar.choice) {

    case oneof_foo_bar_choice_fie_e:
        printf("Foo.bar.fie: %d\n", foo_p->bar.value.fie);
        break;

    case oneof_foo_bar_choice_fum_e:
        printf("Foo.bar.fum: %d\n", foo_p->bar.value.fum);
        break;

    default:
        printf("Unknown choice %d\n", foo_p->bar.choice);
        break;
    }

    return (0);
}
