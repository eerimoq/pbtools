#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "address_book.h"

int main(int argc, const char *argv[])
{
    uint8_t encoded[128];
    int size;
    uint8_t workspace[1024];
    struct address_book_address_book_t *address_book_p;
    struct address_book_person_t *person_p;
    struct address_book_person_phone_number_t *phone_number_p;

    address_book_p = address_book_address_book_new(&workspace[0],
                                                   sizeof(workspace));
    assert(address_book_p != NULL);

    /* Add one person to the address book. */
    assert(address_book_address_book_people_alloc(address_book_p, 1) == 0);
    person_p = address_book_p->people.items_pp[0];
    pbtools_set_string(&person_p->name, "Kalle Kula");
    person_p->id = 56;
    pbtools_set_string(&person_p->email, "kalle.kula@foobar.com");

    /* Add phone numbers. */
    assert(address_book_person_phones_alloc(person_p, 2) == 0);

    /* Home. */
    phone_number_p = person_p->phones.items_pp[0];
    pbtools_set_string(&phone_number_p->number, "+46701232345");
    phone_number_p->type = address_book_person_phone_type_home_e;

    /* Work. */
    phone_number_p = person_p->phones.items_pp[1];
    pbtools_set_string(&phone_number_p->number, "+46999999999");
    phone_number_p->type = address_book_person_phone_type_work_e;

    /* Encode the message. */
    size = address_book_address_book_encode(address_book_p,
                                            &encoded[0],
                                            sizeof(encoded));
    assert(size == 75);
    assert(memcmp(&encoded[0],
                  "\x0a\x49\x0a\x0a\x4b\x61\x6c\x6c\x65\x20"
                  "\x4b\x75\x6c\x61\x10\x38\x1a\x15\x6b\x61"
                  "\x6c\x6c\x65\x2e\x6b\x75\x6c\x61\x40\x66"
                  "\x6f\x6f\x62\x61\x72\x2e\x63\x6f\x6d\x22"
                  "\x10\x0a\x0c\x2b\x34\x36\x37\x30\x31\x32"
                  "\x33\x32\x33\x34\x35\x10\x01\x22\x10\x0a"
                  "\x0c\x2b\x34\x36\x39\x39\x39\x39\x39\x39"
                  "\x39\x39\x39\x10\x02",
                  size) == 0);

    /* Decode the message. */
    address_book_p = address_book_address_book_new(&workspace[0],
                                                   sizeof(workspace));
    assert(address_book_p != NULL);
    size = address_book_address_book_decode(address_book_p, &encoded[0], size);
    assert(size >= 0);
    assert(address_book_p->people.length == 1);

    /* Check the decoded person. */
    person_p = address_book_p->people.items_pp[0];
    assert(person_p != NULL);
    assert(pbtools_get_string(&person_p->name) != NULL);
    assert(strcmp(pbtools_get_string(&person_p->name),
                  "Kalle Kula") == 0);
    assert(person_p->id == 56);
    assert(pbtools_get_string(&person_p->email) != NULL);
    assert(strcmp(pbtools_get_string(&person_p->email),
                  "kalle.kula@foobar.com") == 0);
    assert(person_p->phones.length == 2);

    /* Check home phone number. */
    phone_number_p = person_p->phones.items_pp[0];
    assert(strcmp(pbtools_get_string(&phone_number_p->number),
                  "+46701232345") == 0);
    assert(phone_number_p->type == address_book_person_phone_type_home_e);

    /* Check work phone number. */
    phone_number_p = person_p->phones.items_pp[1];
    assert(strcmp(pbtools_get_string(&phone_number_p->number),
                  "+46999999999") == 0);
    assert(phone_number_p->type == address_book_person_phone_type_work_e);

    return (0);
}
