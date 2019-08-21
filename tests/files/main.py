import struct
import binascii

import int32_pb2
import int64_pb2
import sint32_pb2
import sint64_pb2
import uint32_pb2
import uint64_pb2
import fixed32_pb2
import fixed64_pb2
import sfixed32_pb2
import sfixed64_pb2
import float_pb2
import double_pb2
import bool_pb2
import string_pb2
import bytes_pb2
import enum_pb2
import message_pb2
import repeated_pb2


def output_int32():
    message = int32_pb2.Message()

    print('----------------- int32 begin -----------------')

    values = [
        -(2 ** 31),
        -1,
        0,
        1,
        2 ** 7 - 1,
        2 ** 7,
        2 ** 14 - 1,
        2 ** 14,
        2 ** 21 - 1,
        2 ** 21,
        2 ** 28 - 1,
        2 ** 28,
        2 ** 31 - 1
    ]

    for value in values:
        message.value = value
        print(f'------ value {value:#x} -------')
        print('Message:')
        print(str(message).strip())
        print('Encoded:')
        print(binascii.hexlify(message.SerializeToString()))

    print('------------------ int32 end ------------------')


def output_int64():
    message = int64_pb2.Message()

    print('----------------- int64 begin -----------------')

    values = [
        -(2 ** 63),
        -1,
        0,
        1,
        2 ** 7 - 1,
        2 ** 7,
        2 ** 14 - 1,
        2 ** 14,
        2 ** 21 - 1,
        2 ** 21,
        2 ** 28 - 1,
        2 ** 28,
        2 ** 35 - 1,
        2 ** 35,
        2 ** 42 - 1,
        2 ** 42,
        2 ** 49 - 1,
        2 ** 49,
        2 ** 56 - 1,
        2 ** 56,
        2 ** 63 - 1
    ]

    for value in values:
        message.value = value
        print(f'------ value {value:#x} -------')
        print('Message:')
        print(str(message).strip())
        print('Encoded:')
        print(binascii.hexlify(message.SerializeToString()))

    print('------------------ int64 end ------------------')


def output_sint32():
    message = sint32_pb2.Message()

    print('----------------- sint32 begin -----------------')

    values = [
        -(2 ** 31),
        -(2 ** 27 + 1),
        -(2 ** 27),
        -(2 ** 20 + 1),
        -(2 ** 20),
        -(2 ** 13 + 1),
        -(2 ** 13),
        -(2 ** 6 + 1),
        -(2 ** 6),
        -1,
        0,
        1,
        2 ** 7 - 1,
        2 ** 7,
        2 ** 14 - 1,
        2 ** 14,
        2 ** 21 - 1,
        2 ** 21,
        2 ** 28 - 1,
        2 ** 28,
        2 ** 31 - 1
    ]

    for value in values:
        message.value = value
        print(f'------ value {value:#x} -------')
        print('Message:')
        print(str(message).strip())
        print('Encoded:')
        print(binascii.hexlify(message.SerializeToString()))

    print('------------------ sint32 end ------------------')


def output_sint64():
    message = sint64_pb2.Message()

    print('----------------- sint64 begin -----------------')

    values = [
        -(2 ** 63),
        -(2 ** 62 + 1),
        -(2 ** 62),
        -(2 ** 55 + 1),
        -(2 ** 55),
        -(2 ** 48 + 1),
        -(2 ** 48),
        -(2 ** 41 + 1),
        -(2 ** 41),
        -(2 ** 34 + 1),
        -(2 ** 34),
        -(2 ** 27 + 1),
        -(2 ** 27),
        -(2 ** 20 + 1),
        -(2 ** 20),
        -(2 ** 13 + 1),
        -(2 ** 13),
        -(2 ** 6 + 1),
        -(2 ** 6),
        -1,
        0,
        1,
        2 ** 6 - 1,
        2 ** 6,
        2 ** 13 - 1,
        2 ** 13,
        2 ** 20 - 1,
        2 ** 20,
        2 ** 27 - 1,
        2 ** 27,
        2 ** 34 - 1,
        2 ** 34,
        2 ** 41 - 1,
        2 ** 41,
        2 ** 48 - 1,
        2 ** 48,
        2 ** 55 - 1,
        2 ** 55,
        2 ** 62 - 1,
        2 ** 62,
        2 ** 63 - 1
    ]

    for value in values:
        message.value = value
        print(f'------ value {value:#x} -------')
        print('Message:')
        print(str(message).strip())
        print('Encoded:')
        print(binascii.hexlify(message.SerializeToString()))

    print('------------------ sint64 end ------------------')


def output_uint32():
    message = uint32_pb2.Message()

    print('----------------- uint32 begin -----------------')

    values = [
        0,
        1,
        2 ** 7 - 1,
        2 ** 7,
        2 ** 14 - 1,
        2 ** 14,
        2 ** 21 - 1,
        2 ** 21,
        2 ** 28 - 1,
        2 ** 28,
        2 ** 32 - 1
    ]

    for value in values:
        message.value = value
        print(f'------ value {value:#x} -------')
        print('Message:')
        print(str(message).strip())
        print('Encoded:')
        print(binascii.hexlify(message.SerializeToString()))

    print('------------------ uint32 end ------------------')


def output_uint64():
    message = uint64_pb2.Message()

    print('----------------- uint64 begin -----------------')

    values = [
        0,
        1,
        2 ** 7 - 1,
        2 ** 7,
        2 ** 14 - 1,
        2 ** 14,
        2 ** 21 - 1,
        2 ** 21,
        2 ** 28 - 1,
        2 ** 28,
        2 ** 35 - 1,
        2 ** 35,
        2 ** 42 - 1,
        2 ** 42,
        2 ** 49 - 1,
        2 ** 49,
        2 ** 56 - 1,
        2 ** 56,
        2 ** 64 - 1
    ]

    for value in values:
        message.value = value
        print(f'------ value {value:#x} -------')
        print('Message:')
        print(str(message).strip())
        print('Encoded:')
        print(binascii.hexlify(message.SerializeToString()))

    print('------------------ uint64 end ------------------')


def output_fixed32():
    message = fixed32_pb2.Message()

    print('----------------- fixed32 begin -----------------')

    values = [
        0,
        1,
        2 ** 32 - 1
    ]

    for value in values:
        message.value = value
        print(f'------ value {value:#x} -------')
        print('Message:')
        print(str(message).strip())
        print('Encoded:')
        print(binascii.hexlify(message.SerializeToString()))

    print('------------------ fixed32 end ------------------')


def output_fixed64():
    message = fixed64_pb2.Message()

    print('----------------- fixed64 begin -----------------')

    values = [
        0,
        1,
        2 ** 64 - 1
    ]

    for value in values:
        message.value = value
        print(f'------ value {value:#x} -------')
        print('Message:')
        print(str(message).strip())
        print('Encoded:')
        print(binascii.hexlify(message.SerializeToString()))

    print('------------------ fixed64 end ------------------')


def output_sfixed32():
    message = sfixed32_pb2.Message()

    print('----------------- sfixed32 begin -----------------')

    values = [
        -(2 ** 31),
        -1,
        0,
        1,
        2 ** 31 - 1
    ]

    for value in values:
        message.value = value
        print(f'------ value {value:#x} -------')
        print('Message:')
        print(str(message).strip())
        print('Encoded:')
        print(binascii.hexlify(message.SerializeToString()))

    print('------------------ sfixed32 end ------------------')


def output_sfixed64():
    message = sfixed64_pb2.Message()

    print('----------------- sfixed64 begin -----------------')

    values = [
        -(2 ** 63),
        -1,
        0,
        1,
        2 ** 63 - 1
    ]

    for value in values:
        message.value = value
        print(f'------ value {value:#x} -------')
        print('Message:')
        print(str(message).strip())
        print('Encoded:')
        print(binascii.hexlify(message.SerializeToString()))

    print('------------------ sfixed64 end ------------------')


def output_float():
    message = float_pb2.Message()

    print('----------------- float begin -----------------')

    values = [
        -500,
        -1,
        0,
        1,
        500
    ]

    for value in values:
        message.value = value
        print(f'------ value {value} -------')
        print('Message:')
        print(str(message).strip())
        print('Encoded:')
        print(binascii.hexlify(message.SerializeToString()))
        print(binascii.hexlify(struct.pack('f', value)))
        print(struct.unpack('f', struct.pack('f', value))[0])

    print('------------------ float end ------------------')


def output_double():
    message = double_pb2.Message()

    print('----------------- double begin -----------------')

    values = [
        -500,
        -1,
        0,
        1,
        500
    ]

    for value in values:
        message.value = value
        print(f'------ value {value} -------')
        print('Message:')
        print(str(message).strip())
        print('Encoded:')
        print(binascii.hexlify(message.SerializeToString()))
        print(binascii.hexlify(struct.pack('d', value)))
        print(struct.unpack('d', struct.pack('d', value))[0])

    print('------------------ double end ------------------')


def output_bool():
    message = bool_pb2.Message()

    print('----------------- bool begin -----------------')

    values = [
        True,
        False
    ]

    for value in values:
        message.value = value
        print(f'------ value {value} -------')
        print('Message:')
        print(str(message).strip())
        print('Encoded:')
        print(binascii.hexlify(message.SerializeToString()))

    print('------------------ bool end ------------------')


def output_string():
    message = string_pb2.Message()

    print('----------------- string begin -----------------')

    values = [
        '',
        '1',
        15 * '1234567890'
    ]

    for value in values:
        message.value = value
        print(f'------ value {value} -------')
        print('Message:')
        print(str(message).strip())
        print('Encoded:')
        print(binascii.hexlify(message.SerializeToString()))

    print('------------------ string end ------------------')


def output_bytes():
    message = bytes_pb2.Message()

    print('----------------- bytes begin -----------------')

    values = [
        b'',
        b'1',
        15 * b'1234567890'
    ]

    for value in values:
        message.value = value
        print(f'------ value {value} -------')
        print('Message:')
        print(str(message).strip())
        print('Encoded:')
        print(binascii.hexlify(message.SerializeToString()))

    print('------------------ bytes end ------------------')


def output_enum():
    message = enum_pb2.Message()

    print('----------------- enum begin -----------------')

    values = [
        enum_pb2.Message.A,
        enum_pb2.Message.B
    ]

    for value in values:
        message.value = value
        print(f'------ value {value} -------')
        print('Message:')
        print(str(message).strip())
        print('Encoded:')
        print(binascii.hexlify(message.SerializeToString()))

    print('------------------ enum end ------------------')


def main():
    output_int32()
    output_int64()
    output_sint32()
    output_sint64()
    output_uint32()
    output_uint64()
    output_fixed32()
    output_fixed64()
    output_sfixed32()
    output_sfixed64()
    output_float()
    output_double()
    output_bool()
    output_string()
    output_bytes()
    output_enum()


if __name__ == '__main__':
    main()
