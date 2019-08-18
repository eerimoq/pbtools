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
    uint8_t *encoded_p;
    int size;
    struct address_book_adderss_book_t *address_book_p;
    struct address_book_person_t *person_p;
    struct address_book_person_phone_number_t *phone_number_p;

    address_book_p = address_book_adderss_book_alloc();

    /* Add one person to the address book. */
    person_p = address_book_adderss_book_people_add(address_book_p);
    assert(person_p != NULL);
    address_book_person_set_name(person_p, "Kalle Kula");
    address_book_person_set_id(person_p, 56);
    address_book_person_set_email(person_p, "kalle.kula@foobar.com");

    /* Add home phone number. */
    phone_number_p = address_book_person_phones_add(person_p);
    assert(phone_number_p != NULL);
    address_book_person_phone_number_set_number(phone_number_p, "+46701232345");
    address_book_person_phone_number_set_type(phone_number_p,
                                              address_book_person_phone_type_home_e);

    /* Add work phone number. */
    phone_number_p = address_book_person_phones_add(person_p);
    assert(phone_number_p != NULL);
    address_book_person_phone_number_set_number(phone_number_p, "+46999999999");
    address_book_person_phone_number_set_type(phone_number_p,
                                              address_book_person_phone_type_work_e);

    /* Encode the message. */
    size = address_book_adderss_book_encode(address_book_p, &encoded_p);
    assert(size == 44);
    assert(memcmp(encoded_p,
                  "\x80\xbc\x61\x4e\x02\x0f\xff\xff\xff\xf1\x00\x00\x81\x18"
                  "\x00\x08\x10\x1a\x00\x00\x81\x01\x82\x7e\xb4\xb4\xb4\xb4"
                  "\xb4\xb4\xb4\xb4\xb4\xb4\xb4\xb4\xb4\xb4\xb4\xb4",
                  size) == 0);
    address_book_adderss_book_free(address_book_p);

    /* Decode the message. */
    size = address_book_adderss_book_decode(encoded_p, &address_book_p);
    assert(size == 44);
    assert(address_book_p != NULL);
    assert(address_book_adderss_book_people_length(address_book_p) == 1);

    /* Check the decoded person. */
    person_p = address_book_adderss_book_people_get(address_book_p, 0);
    assert(person_p != NULL);
    assert(strcmp(address_book_person_get_name(person_p), "Kalle Kula") == 0);
    assert(address_book_person_get_id(person_p) == 56);
    assert(strcmp(address_book_person_get_email(person_p),
                  "kalle.kula@foobar.com") == 0);
    assert(address_book_person_phones_length(person_p) == 2);

    /* Check home phone number. */
    phone_number_p = address_book_person_phones_get(person_p, 0);
    assert(strcmp(address_book_person_phone_number_get_number(phone_number_p),
                  "+46701232345") == 0);
    assert(address_book_person_phone_number_get_type(phone_number_p) ==
           address_book_person_phone_type_home_e);

    /* Check work phone number. */
    phone_number_p = address_book_person_phones_get(person_p, 1);
    assert(strcmp(address_book_person_phone_number_get_number(phone_number_p),
                  "+46999999999") == 0);
    assert(address_book_person_phone_number_get_type(phone_number_p) ==
           address_book_person_phone_type_work_e);

    address_book_adderss_book_free(address_book_p);

    return (0);
}
