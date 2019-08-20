import binascii
import address_book_pb2

address_book = address_book_pb2.AddressBook()

# Add one person to the address book.
person = address_book.people.add()
person.name = 'Kalle Kula'
person.id = 56
person.email = 'kalle.kula@foobar.com'

# Add phone numbers.

# Home.
phone_number = person.phones.add()
phone_number.number = '+46701232345';
phone_number.type = address_book_pb2.Person.HOME

# Work.
phone_number = person.phones.add()
phone_number.number = '+46999999999'
phone_number.type = address_book_pb2.Person.WORK

print(address_book)
print(binascii.hexlify(address_book.SerializeToString()))
