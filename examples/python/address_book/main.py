import address_book

address_book = address_book.AddressBook()

# Add one person to the address book.
address_book.people.alloc(1)
person = address_book.people[0]
person.name = "Kalle Kula"
person.id = 56
person.email = "kalle.kula@foobar.com"

# Add phone numbers.
person.phones.alloc(2)

# Home.
phone_number = person.phones[0]
phone_number.number = "+46701232345"
phone_number.type = address_book.Person.PhoneType.HOME

# Work.
phone_number = person.phones[1]
phone_number.number = "+46999999999"
phone_number.type = address_book.Person.PhoneType.WORK

# Encode the message.
encoded = address_book.encode()
print(f'Successfully encoded AddressBook into {len(encoded)} bytes.')

# Decode the message.
address_book = address_book.AddressBook()
address_book.decode(encoded)
print(f'Successfully decoded {len(encoded)} bytes into AddressBook.')

# Print the decoded person.
person = address_book.people[0]
print(person.name)
print(person.id)
print(person.email)

# Print home phone number.
phone_number = person.phones[0]
print(phone_number.number)
print(phone_number.type)

# Print work phone number.
phone_number = person.phones[1]
print(phone_number.number)
print(phone_number.type)
