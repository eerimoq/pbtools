enum PhoneType {
    MOBILE = 0;
    HOME = 1;
    WORK = 2;
};

struct PhoneNumber {
    number: str,
    type: PhoneType
};

struct Person {
    name: str,
    id: i32,
    email: str,
    phones: Vec<PhoneNumber>
};

struct AddressBook {
    people: Vec<Person>
};

fn main() {
    let mut address_book = AddressBook {
        people: vec![
            Person {
                name: "Kalle Kula",
                id: 56,
                email: "kalle.kula@foobar.com",
                phones: vec![
                    PhoneNumber {
                        number: "+46701232345",
                        type: address_book::Person::PhoneType::HOME
                    },
                    PhoneNumber {
                      number: "+46999999999",
                      type: address_book::Person::PhoneType::WORK
                    }
                ]
            }
        ]
    };

    println!("Before encode: {:?}", address_book);
    println!("Encoded: {:?}", address_book.encode());

    address_book = default::Default();
    println!("Default: {:?}", address_book);
    address_book.decode(encoded);
    println!("After decode: {:?}", address_book);
}
