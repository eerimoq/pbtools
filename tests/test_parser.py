import unittest

import pbtools


class ParserTest(unittest.TestCase):

    def test_int32(self):
        parsed = pbtools.parse_file('tests/files/int32.proto')

        self.assertEqual(parsed.package, 'int32')
        self.assertEqual(len(parsed.messages), 2)

        message = parsed.messages[0]
        self.assertEqual(len(message.fields), 1)
        self.assertEqual(len(message.enums), 0)
        self.assertEqual(len(message.messages), 0)

        field = message.fields[0]
        self.assertEqual(field.type, 'int32')
        self.assertEqual(field.name, 'value')
        self.assertEqual(field.field_number, 1)
        self.assertFalse(field.repeated)

        message = parsed.messages[1]
        self.assertEqual(len(message.fields), 1)
        self.assertEqual(len(message.enums), 0)
        self.assertEqual(len(message.messages), 0)

        field = message.fields[0]
        self.assertEqual(field.type, 'int32')
        self.assertEqual(field.name, 'value')
        self.assertEqual(field.field_number, 16)
        self.assertFalse(field.repeated)

    def test_repeated(self):
        parsed = pbtools.parse_file('tests/files/repeated.proto')

        self.assertEqual(parsed.package, 'repeated')
        self.assertEqual(len(parsed.messages), 1)

        message = parsed.messages[0]
        self.assertEqual(len(message.fields), 2)
        self.assertEqual(len(message.enums), 0)
        self.assertEqual(len(message.messages), 0)

        field = message.fields[0]
        self.assertEqual(field.type, 'int32')
        self.assertEqual(field.name, 'int32s')
        self.assertEqual(field.field_number, 1)
        self.assertTrue(field.repeated)

        field = message.fields[1]
        self.assertEqual(field.type, 'Message')
        self.assertEqual(field.name, 'messages')
        self.assertEqual(field.field_number, 2)
        self.assertTrue(field.repeated)

    def test_address_book(self):
        parsed = pbtools.parse_file('tests/files/address_book.proto')

        self.assertEqual(parsed.package, 'address_book')
        self.assertEqual(len(parsed.messages), 2)

        # Person.
        message = parsed.messages[0]
        self.assertEqual(len(message.fields), 4)
        self.assertEqual(len(message.enums), 1)
        self.assertEqual(len(message.messages), 1)

        field = message.fields[0]
        self.assertEqual(field.type, 'string')
        self.assertEqual(field.name, 'name')
        self.assertEqual(field.field_number, 1)
        self.assertFalse(field.repeated)

        field = message.fields[1]
        self.assertEqual(field.type, 'int32')
        self.assertEqual(field.name, 'id')
        self.assertEqual(field.field_number, 2)
        self.assertFalse(field.repeated)

        field = message.fields[2]
        self.assertEqual(field.type, 'string')
        self.assertEqual(field.name, 'email')
        self.assertEqual(field.field_number, 3)
        self.assertFalse(field.repeated)

        field = message.fields[3]
        self.assertEqual(field.type, 'PhoneNumber')
        self.assertEqual(field.name, 'phones')
        self.assertEqual(field.field_number, 4)
        self.assertTrue(field.repeated)

        # Person.PhoneType
        enum = message.enums[0]
        self.assertEqual(len(enum.fields), 3)

        field = enum.fields[0]
        self.assertEqual(field.name, 'MOBILE')
        self.assertEqual(field.field_number, 0)

        field = enum.fields[1]
        self.assertEqual(field.name, 'HOME')
        self.assertEqual(field.field_number, 1)

        field = enum.fields[2]
        self.assertEqual(field.name, 'WORK')
        self.assertEqual(field.field_number, 2)

        # Person.PhoneNumber
        inner_message = message.messages[0]
        self.assertEqual(len(inner_message.fields), 2)
        self.assertEqual(len(inner_message.enums), 0)
        self.assertEqual(len(inner_message.messages), 0)

        field = inner_message.fields[0]
        self.assertEqual(field.type, 'string')
        self.assertEqual(field.name, 'number')
        self.assertEqual(field.field_number, 1)
        self.assertFalse(field.repeated)

        field = inner_message.fields[1]
        self.assertEqual(field.type, 'PhoneType')
        self.assertEqual(field.name, 'type')
        self.assertEqual(field.field_number, 2)
        self.assertFalse(field.repeated)

        # AddressBook.
        message = parsed.messages[1]
        self.assertEqual(len(message.fields), 1)

        field = message.fields[0]
        self.assertEqual(field.type, 'Person')
        self.assertEqual(field.name, 'people')
        self.assertEqual(field.field_number, 1)
        self.assertTrue(field.repeated)

    def test_service(self):
        parsed = pbtools.parse_file('tests/files/service.proto')

        self.assertEqual(parsed.package, 'service')
        self.assertEqual(len(parsed.messages), 2)
        self.assertEqual(len(parsed.services), 1)

        service = parsed.services[0]
        self.assertEqual(len(service.rpcs), 2)

        rpc = service.rpcs[0]
        self.assertEqual(rpc.name, 'Foo')
        self.assertEqual(rpc.request_type, 'Request')
        self.assertFalse(rpc.request_stream)
        self.assertEqual(rpc.response_type, 'Response')
        self.assertFalse(rpc.response_stream)

        rpc = service.rpcs[1]
        self.assertEqual(rpc.name, 'Bar')
        self.assertEqual(rpc.request_type, 'Request')
        self.assertFalse(rpc.request_stream)
        self.assertEqual(rpc.response_type, 'Response')
        self.assertFalse(rpc.response_stream)

    def test_oneof(self):
        parsed = pbtools.parse_file('tests/files/oneof.proto')

        self.assertEqual(parsed.package, 'oneof')
        self.assertEqual(len(parsed.messages), 1)

        message = parsed.messages[0]
        oneof = message.oneofs[0]
        self.assertEqual(oneof.name, 'value')

    def test_enum(self):
        parsed = pbtools.parse_file('tests/files/enum.proto')

        self.assertEqual(parsed.package, 'enum')
        self.assertEqual(len(parsed.messages), 2)
        self.assertEqual(len(parsed.enums), 1)

        enum = parsed.enums[0]
        self.assertEqual(enum.name, 'Enum')
        self.assertEqual(len(enum.fields), 2)

        field = enum.fields[0]
        self.assertEqual(field.name, 'C')
        self.assertEqual(field.field_number, 0)

        field = enum.fields[1]
        self.assertEqual(field.name, 'D')
        self.assertEqual(field.field_number, 1)

        message = parsed.messages[0]
        enum = message.enums[0]
        self.assertEqual(enum.name, 'Enum')
        self.assertEqual(len(enum.fields), 2)

        field = enum.fields[0]
        self.assertEqual(field.name, 'A')
        self.assertEqual(field.field_number, 0)

        field = enum.fields[1]
        self.assertEqual(field.name, 'B')
        self.assertEqual(field.field_number, 1)

        message = parsed.messages[1]
        enum = message.enums[0]
        self.assertEqual(enum.name, 'InnerEnum')
        self.assertEqual(len(enum.fields), 2)

        field = enum.fields[0]
        self.assertEqual(field.name, 'E')
        self.assertEqual(field.field_number, 0)

        field = enum.fields[1]
        self.assertEqual(field.name, 'F')
        self.assertEqual(field.field_number, 1)

    def test_options(self):
        parsed = pbtools.parse_file('tests/files/options.proto')

        self.assertEqual(parsed.package, 'options')
        self.assertEqual(len(parsed.messages), 1)


if __name__ == '__main__':
    unittest.main()
