mod person {
    use pbtools::Encoder;

    #[derive(Debug, Copy, Clone)]
    pub enum PhoneType {
        MOBILE = 0,
        HOME = 1,
        WORK = 2
    }

    impl Default for PhoneType {
        fn default() -> Self { PhoneType::MOBILE }
    }

    #[derive(Default, Debug)]
    pub struct PhoneNumber {
        pub number: String,
        pub type_: PhoneType
    }

    impl PhoneNumber {
        pub fn encode_inner(&self,  encoder: &mut Encoder) {
            encoder.write_string(1, &self.number);
            encoder.write_int32(2, self.type_ as i32);
        }
    }
}

use pbtools::Encoder;
use pbtools::Decoder;
use person::{PhoneNumber, PhoneType};

#[derive(Default, Debug)]
struct Person {
    name: String,
    id: i32,
    email: String,
    phones: Vec<PhoneNumber>
}

impl Person {
    fn encode_inner(&self,  encoder: &mut Encoder) {
        encoder.write_string(1, &self.name);
        encoder.write_int32(2, self.id);
        encoder.write_string(3, &self.email);

        for item in &self.phones {
            let mut item_encoder = Encoder {
                encoded: vec![]
            };
            item.encode_inner(&mut item_encoder);
            encoder.write_length_delimited(1, item_encoder.encoded.len() as u64);
            encoder.encoded.extend(item_encoder.encoded);
        }
    }

    fn decode_inner(&mut self,  decoder: &mut Decoder) {
        loop {
            if !decoder.available() {
                break;
            }

            match decoder.read_tag() {
                (1, wire_type) => { self.name = decoder.read_string(wire_type) },
                (2, wire_type) => { self.id = decoder.read_int32(wire_type) },
                (3, wire_type) => { self.email = decoder.read_string(wire_type) },
                (_, wire_type) => decoder.skip_field(wire_type)
            }
        }
    }
}

#[derive(Default, Debug)]
struct AddressBook {
    people: Vec<Person>
}

impl AddressBook {
    fn encode(&self) -> Vec<u8> {
        let mut encoder = Encoder {
            encoded: vec![]
        };
        self.encode_inner(&mut encoder);
        encoder.encoded
    }

    fn encode_inner(&self, encoder: &mut Encoder) {
        for item in &self.people {
            let mut item_encoder = Encoder {
                encoded: vec![]
            };
            item.encode_inner(&mut item_encoder);
            encoder.write_length_delimited(1, item_encoder.encoded.len() as u64);
            encoder.encoded.extend(item_encoder.encoded);
        }
    }

    fn decode(&mut self, encoded: Vec<u8>) -> Result<u64, String> {
        let mut decoder = Decoder {
            pos: 0,
            encoded: encoded
        };
        self.decode_inner(&mut decoder);
        Ok(decoder.encoded.len() as u64)
    }

    fn decode_inner(&mut self, decoder: &mut Decoder) {
        loop {
            if !decoder.available() {
                break;
            }

            match decoder.read_tag() {
                (1, _wire_type) => {
                    let mut person: Person = Default::default();
                    person.decode_inner(decoder);
                    self.people.push(person);
                },
                (_, wire_type) => decoder.skip_field(wire_type)
            }
        }
    }
}

fn main() {
    let mut address_book = AddressBook {
        people: vec![
            Person {
                name: String::from("Kalle Kula"),
                id: 56,
                email: String::from("kalle.kula@foobar.com"),
                phones: vec![
                    PhoneNumber {
                        number: String::from("+46701232345"),
                        type_: PhoneType::HOME
                    },
                    PhoneNumber {
                      number: String::from("+46999999999"),
                      type_: PhoneType::WORK
                    }
                ]
            }
        ]
    };

    println!("Before encode:\n{:#?}", address_book);
    let encoded = address_book.encode();
    println!("Encoded: {:#x?}", encoded);

    address_book = Default::default();
    println!("Default:\n{:#?}", address_book);

    match address_book.decode(encoded) {
        Ok(size) => println!("Size: {}", size),
        Err(message) => println!("Error: {}", message)
    }

    println!("After decode:\n{:#?}", address_book);
}
