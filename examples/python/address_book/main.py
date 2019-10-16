import json
import address_book


# Encode the message.
decoded = {
    'people': [
        {
            'name': "Kalle Kula",
            'id': 56,
            'email': "kalle.kula@foobar.com",
            'phones': [
                {
                    'number': "+46701232345",
                    'type': 'HOME'
                },
                {
                    'number': "+46999999999",
                    'type': 'WORK'
                }
            ]
        }
    ]
}
encoded = address_book.address_book_encode(decoded)
print(f'Successfully encoded AddressBook into {len(encoded)} bytes.')

# Decode the message.
decoded = address_book.address_book_decode(encoded)
print(f'Successfully decoded {len(encoded)} bytes.')
print(json.dumps(decoded, indent=4))
