import os
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
            'address_book',
            'benchmark',
            'bool',
            'bytes',
            'double',
            'enum',
            'fixed32',
            'fixed64',
            'float',
            # 'imported',
            # ('imported2', 'imports'),
            # 'importing',
            'int32',
            'int64',
            'message',
            'no_package',
            # 'oneof',
            'options',
            'repeated',
            'scalar_value_types',
            'service',
            'sfixed32',
            'sfixed64',
            'sint32',
            'sint64',
            'string',
            'tags',
            'uint32',
            'uint64'
        ]

        for spec in specs:
            if isinstance(spec, tuple):
                proto = f'tests/files/{spec[1]}/{spec[0]}.proto'
                options = ['-i', f'tests/files/{spec[1]}']
            else:
                proto = f'tests/files/{spec}.proto'
                options = []

            argv = [
                'pbtools',
                'generate_c_source',
                *options,
                proto
            ]

            filename_h = f'{spec}.h'
            filename_c = f'{spec}.c'

            for filename in [filename_h, filename_c]:
                if os.path.exists(filename):
                    os.remove(filename)

            with patch('sys.argv', argv):
                pbtools._main()

            for filename in [filename_h, filename_c]:
                self.assertEqual(
                    read_file(f'tests/files/c_source/{filename}'),
                    read_file(filename))

    def test_command_line_generate_c_source_headers(self):
        specs = [
            'oneof'
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

    def test_command_line_generate_c_source_multiple_input_files(self):
        argv = [
            'pbtools',
            'generate_c_source',
            'tests/files/int32.proto',
            'tests/files/int64.proto'
        ]

        filenames = [
            'int32.h',
            'int32.c',
            'int64.h',
            'int64.c'
        ]

        for filename in filenames:
            if os.path.exists(filename):
                os.remove(filename)

        with patch('sys.argv', argv):
            pbtools._main()

        for filename in filenames:
            self.assertEqual(
                read_file(f'tests/files/c_source/{filename}'),
                read_file(filename))

    def test_command_line_generate_c_source_pbtools_h_c(self):
        argv = [
            'pbtools',
            'generate_c_source',
            'tests/files/int32.proto'
        ]

        filenames = [
            'int32.h',
            'int32.c'
        ]
        pbtools_filenames = [
            'pbtools.h',
            'pbtools.c'
        ]

        for filename in filenames + pbtools_filenames:
            if os.path.exists(filename):
                os.remove(filename)

        with patch('sys.argv', argv):
            pbtools._main()

        for filename in filenames:
            self.assertEqual(
                read_file(f'tests/files/c_source/{filename}'),
                read_file(filename))

        for filename in pbtools_filenames:
            self.assertEqual(
                read_file(f'pbtools/c_source/{filename}'),
                read_file(filename))


if __name__ == '__main__':
    unittest.main()
