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



#![allow(unused)]

enum PhoneType {
    MOBILE = 0,
    HOME = 1,
    WORK = 2
}

impl Default for PhoneType {
    fn default() -> Self { PhoneType::MOBILE }
}

#[derive(Default)]
struct PhoneNumber {
    number: String,
    type_: PhoneType
}

#[derive(Default)]
struct Person {
    name: String,
    id: i32,
    email: String,
    phones: Vec<PhoneNumber>
}

#[derive(Default)]
struct AddressBook {
    people: Vec<Person>
}

impl AddressBook {
    fn encode(&self) -> Vec<u8> {
        vec![]
    }
}

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
                        type_: PhoneType::HOME
                    },
                    PhoneNumber {
                      number: "+46999999999",
                      type_: PhoneType::WORK
                    }
                ]
            }
        ]
    };

    println!("Before encode: {:?}", address_book);
    let encoded = address_book.encode();
    println!("Encoded: {:?}", encoded);

    address_book = Default::default();
    println!("Default: {:?}", address_book);
    address_book.decode(encoded);
    println!("After decode: {:?}", address_book);
}


#![allow(unused)]

#[derive(Debug)]
enum PhoneType {
    MOBILE = 0,
    HOME = 1,
    WORK = 2
}

impl Default for PhoneType {
    fn default() -> Self { PhoneType::MOBILE }
}

#[derive(Default, Debug)]
struct PhoneNumber {
    number: String,
    type_: PhoneType
}

#[derive(Default, Debug)]
struct Person {
    name: String,
    id: i32,
    email: String,
    phones: Vec<PhoneNumber>
}

#[derive(Default, Debug)]
struct AddressBook {
    people: Vec<Person>
}

impl AddressBook {
    fn encode(&self) -> Vec<u8> {
        vec![]
    }

    fn decode(&self, encoded: Vec<u8>) {
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

    println!("Before encode: {:?}", address_book);
    let encoded = address_book.encode();
    println!("Encoded: {:?}", encoded);

    address_book = Default::default();
    println!("Default: {:?}", address_book);
    address_book.decode(encoded);
    println!("After decode: {:?}", address_book);
}


 
Standard Error

   Compiling playground v0.0.1 (/playground)
    Finished dev [unoptimized + debuginfo] target(s) in 0.61s
     Running `target/debug/playground`

Standard Output

Before encode:
AddressBook {
    people: [
        Person {
            name: "Kalle Kula",
            id: 56,
            email: "kalle.kula@foobar.com",
            phones: [
                PhoneNumber {
                    number: "+46701232345",
                    type_: HOME
                },
                PhoneNumber {
                    number: "+46999999999",
                    type_: WORK
                }
            ]
        }
    ]
}
Encoded: []
Default: AddressBook { people: [] }
After decode: AddressBook { people: [] }



 
Standard Error

   Compiling playground v0.0.1 (/playground)
    Finished dev [unoptimized + debuginfo] target(s) in 1.00s
     Running `target/debug/playground`

Standard Output

Before encode:
AddressBook {
    people: [
        Person {
            name: "Kalle Kula",
            id: 56,
            email: "kalle.kula@foobar.com",
            phones: [
                PhoneNumber {
                    number: "+46701232345",
                    type_: HOME,
                },
                PhoneNumber {
                    number: "+46999999999",
                    type_: WORK,
                },
            ],
        },
    ],
}
Encoded: []
Default:
AddressBook {
    people: [],
}
After decode:
AddressBook {
    people: [],
}



#![allow(unused)]

#[derive(Debug)]
enum PhoneType {
    MOBILE = 0,
    HOME = 1,
    WORK = 2
}

impl Default for PhoneType {
    fn default() -> Self { PhoneType::MOBILE }
}

#[derive(Default, Debug)]
struct PhoneNumber {
    number: String,
    type_: PhoneType
}

#[derive(Default, Debug)]
struct Person {
    name: String,
    id: i32,
    email: String,
    phones: Vec<PhoneNumber>
}

#[derive(Default, Debug)]
struct AddressBook {
    people: Vec<Person>
}

struct Encoder {
    encoded: Vec<u8>
}

impl Encoder {
    fn write_string(&mut self, value: &String) {
        self.write_tagged_varint(1, 2, value.len() as u64);
        self.encoded.extend(value.as_bytes());
    }
    
    fn write_tagged_varint(&mut self, field_number: u32, tag: u32, value: u64) {
        if (value > 0) {
            
        }
    }
}

impl AddressBook {
    fn encode(&self) -> Vec<u8> {
        let mut encoder = Encoder {
            encoded: vec![]
        };
        
        for item in &self.people {
            encoder.write_string(&item.name);
        }
        
        encoder.encoded
    }

    fn decode(&mut self, encoded: Vec<u8>) {
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
    println!("Encoded: {:?}", encoded);

    address_book = Default::default();
    println!("Default:\n{:#?}", address_book);
    address_book.decode(encoded);
    println!("After decode:\n{:#?}", address_book);
}



Encoded: [10, 10, 75, 97, 108, 108, 101, 32, 75, 117, 108, 97]



#![allow(unused)]

#[derive(Debug)]
enum PhoneType {
    MOBILE = 0,
    HOME = 1,
    WORK = 2
}

impl Default for PhoneType {
    fn default() -> Self { PhoneType::MOBILE }
}

#[derive(Default, Debug)]
struct PhoneNumber {
    number: String,
    type_: PhoneType
}

#[derive(Default, Debug)]
struct Person {
    name: String,
    id: i32,
    email: String,
    phones: Vec<PhoneNumber>
}

#[derive(Default, Debug)]
struct AddressBook {
    people: Vec<Person>
}

struct Encoder {
    encoded: Vec<u8>
}

impl Encoder {
    fn write_string(&mut self, value: &String) {
        self.write_tagged_varint(1, 2, value.len() as u64);
        self.encoded.extend(value.as_bytes());
    }
    
    fn write_tagged_varint(&mut self, field_number: u32, wire_type: u32, value: u64) {
        if (value > 0) {
            self.write_tag(field_number, wire_type);
            self.write_varint(value);
        }
    }
    
    fn write_tag(&mut self, field_number: u32, wire_type: u32) {
        self.write_varint((field_number << 3 | wire_type) as u64);
    }
    
    fn write_varint(&mut self, mut value: u64) {
        let mut buf: [u8; 10] = [0; 10];
        let mut pos = 0;

        loop {
            buf[pos] = (value | 0x80) as u8;
            value >>= 7;
            pos += 1;
             
            if (value == 0) {
                break;   
            }
        }

        buf[pos - 1] &= 0x7f;
        self.encoded.extend(&buf[..pos]);
    }
}

impl AddressBook {
    fn encode(&self) -> Vec<u8> {
        let mut encoder = Encoder {
            encoded: vec![]
        };
        
        for item in &self.people {
            encoder.write_string(&item.name);
        }
        
        encoder.encoded
    }

    fn decode(&mut self, encoded: Vec<u8>) {
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
    println!("Encoded: {:?}", encoded);

    address_book = Default::default();
    println!("Default:\n{:#?}", address_book);
    address_book.decode(encoded);
    println!("After decode:\n{:#?}", address_book);
}




#![allow(unused)]

#[derive(Debug)]
enum PhoneType {
    MOBILE = 0,
    HOME = 1,
    WORK = 2
}

impl Default for PhoneType {
    fn default() -> Self { PhoneType::MOBILE }
}

#[derive(Default, Debug)]
struct PhoneNumber {
    number: String,
    type_: PhoneType
}

#[derive(Default, Debug)]
struct Person {
    name: String,
    id: i32,
    email: String,
    phones: Vec<PhoneNumber>
}

#[derive(Default, Debug)]
struct AddressBook {
    people: Vec<Person>
}

struct Encoder {
    encoded: Vec<u8>
}

impl Encoder {
    fn write_string(&mut self, value: &String) {
        self.write_tagged_varint(1, 2, value.len() as u64);
        self.encoded.extend(value.as_bytes());
    }
    
    fn write_tagged_varint(&mut self, field_number: u32, wire_type: u32, value: u64) {
        if (value > 0) {
            self.write_tag(field_number, wire_type);
            self.write_varint(value);
        }
    }
    
    fn write_tag(&mut self, field_number: u32, wire_type: u32) {
        self.write_varint((field_number << 3 | wire_type) as u64);
    }
    
    fn write_varint(&mut self, mut value: u64) {
        loop {
            let mut item = value as u8;
            value >>= 7;
             
            if (value == 0) {
                self.encoded.push(item);
                break;   
            } else {
                self.encoded.push(item | 0x80);
            }
        }
    }
}

impl AddressBook {
    fn encode(&self) -> Vec<u8> {
        let mut encoder = Encoder {
            encoded: vec![]
        };
        
        for item in &self.people {
            encoder.write_string(&item.name);
        }
        
        encoder.encoded
    }

    fn decode(&mut self, encoded: Vec<u8>) {
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
    println!("Encoded: {:?}", encoded);

    address_book = Default::default();
    println!("Default:\n{:#?}", address_book);
    address_book.decode(encoded);
    println!("After decode:\n{:#?}", address_book);
}




#![allow(unused)]

#[derive(Debug)]
enum PhoneType {
    MOBILE = 0,
    HOME = 1,
    WORK = 2
}

impl Default for PhoneType {
    fn default() -> Self { PhoneType::MOBILE }
}

#[derive(Default, Debug)]
struct PhoneNumber {
    number: String,
    type_: PhoneType
}

#[derive(Default, Debug)]
struct Person {
    name: String,
    id: i32,
    email: String,
    phones: Vec<PhoneNumber>
}

#[derive(Default, Debug)]
struct AddressBook {
    people: Vec<Person>
}

struct Encoder {
    encoded: Vec<u8>
}

impl Encoder {
    fn write_string(&mut self, field_number: u32, value: &String) {
        self.write_tagged_varint(field_number, 2, value.len() as u64);
        self.encoded.extend(value.as_bytes());
    }
    
    fn write_tagged_varint(&mut self, field_number: u32, wire_type: u32, value: u64) {
        if (value > 0) {
            self.write_tag(field_number, wire_type);
            self.write_varint(value);
        }
    }
    
    fn write_tag(&mut self, field_number: u32, wire_type: u32) {
        self.write_varint((field_number << 3 | wire_type) as u64);
    }
    
    fn write_varint(&mut self, mut value: u64) {
        loop {
            let mut item = value as u8;
            value >>= 7;
             
            if (value == 0) {
                self.encoded.push(item);
                break;   
            } else {
                self.encoded.push(item | 0x80);
            }
        }
    }
    
    fn write_int32(&mut self, field_number: u32, value: i32) {
        self.write_tagged_varint(field_number, 0, value as i64 as u64);
    }
}

impl AddressBook {
    fn encode(&self) -> Vec<u8> {
        let mut encoder = Encoder {
            encoded: vec![]
        };
        
        for item in &self.people {
            encoder.write_string(1, &item.name);
            encoder.write_int32(2, item.id);
            encoder.write_string(3, &item.email);
        }
        
        encoder.encoded
    }

    fn decode(&mut self, encoded: Vec<u8>) {
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
    address_book.decode(encoded);
    println!("After decode:\n{:#?}", address_book);
}



#![allow(unused)]

struct Encoder {
    encoded: Vec<u8>
}

impl Encoder {
    fn write_string(&mut self, field_number: u32, value: &String) {
        self.write_tagged_varint(field_number, 2, value.len() as u64);
        self.encoded.extend(value.as_bytes());
    }
    
    fn write_tagged_varint(&mut self, field_number: u32, wire_type: u32, value: u64) {
        if (value > 0) {
            self.write_tag(field_number, wire_type);
            self.write_varint(value);
        }
    }
    
    fn write_tag(&mut self, field_number: u32, wire_type: u32) {
        self.write_varint((field_number << 3 | wire_type) as u64);
    }
    
    fn write_varint(&mut self, mut value: u64) {
        loop {
            let mut item = value as u8;
            value >>= 7;
             
            if (value == 0) {
                self.encoded.push(item);
                break;   
            } else {
                self.encoded.push(item | 0x80);
            }
        }
    }
    
    fn write_int32(&mut self, field_number: u32, value: i32) {
        self.write_tagged_varint(field_number, 0, value as i64 as u64);
    }
}

#[derive(Debug, Copy, Clone)]
enum PhoneType {
    MOBILE = 0,
    HOME = 1,
    WORK = 2
}

impl Default for PhoneType {
    fn default() -> Self { PhoneType::MOBILE }
}

#[derive(Default, Debug)]
struct PhoneNumber {
    number: String,
    type_: PhoneType
}

impl PhoneNumber {
    fn encode_inner(&self,  encoder: &mut Encoder) {
        encoder.write_string(1, &self.number);
        encoder.write_int32(2, self.type_ as i32);
    }
}

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
            item.encode_inner(encoder);
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
            item.encode_inner(encoder);
        }
    }

    fn decode(&mut self, encoded: Vec<u8>) {
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
    address_book.decode(encoded);
    println!("After decode:\n{:#?}", address_book);
}
#![allow(unused)]

struct Encoder {
    encoded: Vec<u8>
}

impl Encoder {
    fn write_string(&mut self, field_number: u32, value: &String) {
        self.write_tagged_varint(field_number, 2, value.len() as u64);
        self.encoded.extend(value.as_bytes());
    }
    
    fn write_tagged_varint(&mut self, field_number: u32, wire_type: u32, value: u64) {
        if (value > 0) {
            self.write_tag(field_number, wire_type);
            self.write_varint(value);
        }
    }
    
    fn write_tag(&mut self, field_number: u32, wire_type: u32) {
        self.write_varint((field_number << 3 | wire_type) as u64);
    }
    
    fn write_varint(&mut self, mut value: u64) {
        loop {
            let mut item = value as u8;
            value >>= 7;
             
            if (value == 0) {
                self.encoded.push(item);
                break;   
            } else {
                self.encoded.push(item | 0x80);
            }
        }
    }
    
    fn write_int32(&mut self, field_number: u32, value: i32) {
        self.write_tagged_varint(field_number, 0, value as i64 as u64);
    }
}

#[derive(Debug, Copy, Clone)]
enum PhoneType {
    MOBILE = 0,
    HOME = 1,
    WORK = 2
}

impl Default for PhoneType {
    fn default() -> Self { PhoneType::MOBILE }
}

#[derive(Default, Debug)]
struct PhoneNumber {
    number: String,
    type_: PhoneType
}

impl PhoneNumber {
    fn encode_inner(&self,  encoder: &mut Encoder) {
        encoder.write_string(1, &self.number);
        encoder.write_int32(2, self.type_ as i32);
    }
}

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
            item.encode_inner(encoder);
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
            item.encode_inner(encoder);
        }
    }

    fn decode(&mut self, encoded: Vec<u8>) {
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
    address_book.decode(encoded);
    println!("After decode:\n{:#?}", address_book);
}


#![allow(unused)]

struct Encoder {
    encoded: Vec<u8>
}

impl Encoder {
    fn write_string(&mut self, field_number: u32, value: &String) {
        self.write_tagged_varint(field_number, 2, value.len() as u64);
        self.encoded.extend(value.as_bytes());
    }
    
    fn write_tagged_varint(&mut self, field_number: u32, wire_type: u32, value: u64) {
        if (value > 0) {
            self.write_tag(field_number, wire_type);
            self.write_varint(value);
        }
    }
    
    fn write_tag(&mut self, field_number: u32, wire_type: u32) {
        self.write_varint((field_number << 3 | wire_type) as u64);
    }
    
    fn write_varint(&mut self, mut value: u64) {
        loop {
            let mut item = value as u8;
            value >>= 7;
             
            if (value == 0) {
                self.encoded.push(item);
                break;   
            } else {
                self.encoded.push(item | 0x80);
            }
        }
    }
    
    fn write_int32(&mut self, field_number: u32, value: i32) {
        self.write_tagged_varint(field_number, 0, value as i64 as u64);
    }
}

#[derive(Debug, Copy, Clone)]
enum PhoneType {
    MOBILE = 0,
    HOME = 1,
    WORK = 2
}

impl Default for PhoneType {
    fn default() -> Self { PhoneType::MOBILE }
}

#[derive(Default, Debug)]
struct PhoneNumber {
    number: String,
    type_: PhoneType
}

impl PhoneNumber {
    fn encode_inner(&self,  encoder: &mut Encoder) {
        encoder.write_string(1, &self.number);
        encoder.write_int32(2, self.type_ as i32);
    }
}

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
            item.encode_inner(encoder);
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
            item.encode_inner(encoder);
        }
    }

    fn decode(&mut self, encoded: Vec<u8>) {
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
    address_book.decode(encoded);
    println!("After decode:\n{:#?}", address_book);
}




#![allow(unused)]

struct Encoder {
    encoded: Vec<u8>
}

impl Encoder {
    fn write_string(&mut self, field_number: u32, value: &String) {
        self.write_tagged_varint(field_number, 2, value.len() as u64);
        self.encoded.extend(value.as_bytes());
    }
    
    fn write_tagged_varint(&mut self, field_number: u32, wire_type: u32, value: u64) {
        if (value > 0) {
            self.write_tag(field_number, wire_type);
            self.write_varint(value);
        }
    }
    
    fn write_tag(&mut self, field_number: u32, wire_type: u32) {
        self.write_varint((field_number << 3 | wire_type) as u64);
    }
    
    fn write_varint(&mut self, mut value: u64) {
        loop {
            let mut item = value as u8;
            value >>= 7;
             
            if (value == 0) {
                self.encoded.push(item);
                break;   
            } else {
                self.encoded.push(item | 0x80);
            }
        }
    }
    
    fn write_int32(&mut self, field_number: u32, value: i32) {
        self.write_tagged_varint(field_number, 0, value as i64 as u64);
    }
}

#[derive(Debug, Copy, Clone)]
enum PhoneType {
    MOBILE = 0,
    HOME = 1,
    WORK = 2
}

impl Default for PhoneType {
    fn default() -> Self { PhoneType::MOBILE }
}

#[derive(Default, Debug)]
struct PhoneNumber {
    number: String,
    type_: PhoneType
}

impl PhoneNumber {
    fn encode_inner(&self,  encoder: &mut Encoder) {
        encoder.write_string(1, &self.number);
        encoder.write_int32(2, self.type_ as i32);
    }
}

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
            item.encode_inner(encoder);
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
            item.encode_inner(encoder);
        }
    }

    fn decode(&mut self, encoded: Vec<u8>) -> Result<u64, String> {
        //Ok(encoded.len() as u64)
        Err(String::from("apa"))
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
