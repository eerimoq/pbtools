#include <stdio.h>
#include "repeated.h"

static int32_t xs[] = { 1, 3, 5, 7, 9 };

int main(int argc, const char *argv[])
{
    int i;
    int res;
    int size;
    uint8_t workspace[128];
    uint8_t encoded[64];
    struct coordinates_t *coordinates_p;

    /* Encode. */
    coordinates_p = coordinates_new(&workspace[0], sizeof(workspace));

    if (coordinates_p == NULL) {
        return (1);
    }

    /* Set the length and items. */
    coordinates_p->xs.length = 5;
    coordinates_p->xs.items_p = &xs[0];

    /* Alternatively, allocate a buffer in the workspace. */
    res = coordinates_ys_alloc(coordinates_p, 5);

    if (res != 0) {
        return (2);
    }

    coordinates_p->ys.items_p[0] = 0;
    coordinates_p->ys.items_p[1] = 2;
    coordinates_p->ys.items_p[2] = 4;
    coordinates_p->ys.items_p[3] = 6;
    coordinates_p->ys.items_p[4] = 8;

    size = coordinates_encode(coordinates_p, &encoded[0], sizeof(encoded));

    if (size < 0) {
        return (3);
    }

    printf("Successfully encoded Coordinates into %d bytes.\n", size);

    /* Decode. */
    coordinates_p = coordinates_new(&workspace[0], sizeof(workspace));

    if (coordinates_p == NULL) {
        return (4);
    }

    size = coordinates_decode(coordinates_p, &encoded[0], size);

    if (size < 0) {
        return (5);
    }

    printf("Successfully decoded %d bytes into Coordinates.\n", size);

    if (coordinates_p->xs.length != coordinates_p->ys.length) {
        return (6);
    }

    for (i = 0; i < coordinates_p->xs.length; i++) {
        printf("(%d, %d)\n",
               (int)coordinates_p->xs.items_p[i],
               (int)coordinates_p->ys.items_p[i]);
    }

    return (0);
}
