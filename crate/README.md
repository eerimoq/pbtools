# Google Protocol Buffers encoding and decoding

🚧 🚧 🚧 🚧 🚧 **Under construction - DO NOT USE** 🚧 🚧 🚧 🚧 🚧

Install the Python package
[pbtools](https://pypi.org/project/pbtools/) and use it to generate
Rust source code from protobuf specification(s). Add the generated
files to your projects crate. Add this crate as a dependency in your
project's Cargo.toml file and you should be good to go.

``` console
$ pip install pbtools
$ pbtools generate_rust_source address_book.proto
$ ls -l
address_book.rs
```

🚧 🚧 🚧 🚧 🚧 **Under construction - DO NOT USE** 🚧 🚧 🚧 🚧 🚧

# Example usage

``` rust
use address_book::{AddressBook, Person};
use address_book::person::{PhoneNumber, PhoneType};

fn main() {
    // Encode.
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

    let encoded = address_book.encode();
    println!("Encoded: {:?}", encoded);

    // Decode.
    address_book = Default::default();

    match address_book.decode(encoded) {
        Ok(()) => println!("Ok!"),
        Err(message) => println!("Error: {}", message)
    }

    println!("Decoded:\n{:#?}", address_book);
}
```
