import sys
import os
import re
import unittest
from unittest.mock import patch

import pbtools


def read_file(filename):
    with open(filename, 'r') as fin:
        return fin.read()


class CommandLineTest(unittest.TestCase):

    maxDiff = None

    def test_command_line_generate_c_source(self):
        specs = [
            'int32',
            'int64',
            'sint32',
            'sint64',
            'uint32',
            'uint64',
            'fixed32',
            'fixed64',
            'sfixed32',
            'sfixed64',
            'float',
            'double',
            'bool',
            'tags',
            'string',
            'bytes',
            'scalar_value_types',
            'repeated',
            # 'message'
        ]

        for spec in specs:
            argv = [
                'pbtools',
                'generate_c_source',
                'tests/files/{}.proto'.format(spec)
            ]

            filename_h = f'{spec}.h'
            filename_c = f'{spec}.c'

            if os.path.exists(filename_h):
                os.remove(filename_h)

            if os.path.exists(filename_c):
                os.remove(filename_c)

            with patch('sys.argv', argv):
                pbtools._main()

            self.assertEqual(
                read_file(f'tests/files/c_source/{filename_h}'),
                read_file(filename_h))
            self.assertEqual(
                read_file(f'tests/files/c_source/{filename_c}'),
                read_file(filename_c))

    def test_command_line_generate_c_source_headers(self):
        specs = [
            'message',
            'enum',
            'address_book',
            'oneof',
        ]

        for spec in specs:
            argv = [
                'pbtools',
                'generate_c_source',
                'tests/files/{}.proto'.format(spec)
            ]

            filename_h = f'{spec}.h'

            if os.path.exists(filename_h):
                os.remove(filename_h)

            with patch('sys.argv', argv):
                pbtools._main()

            self.assertEqual(
                read_file(f'tests/files/c_source/{filename_h}'),
                read_file(filename_h))


if __name__ == '__main__':
    unittest.main()
