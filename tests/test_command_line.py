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
            'enum_provider',
            ('enum_user', ['.']),
            'fixed32',
            'fixed64',
            'float',
            ('imported', ['imports']),
            ('imports/imported1', ['imports']),
            ('imports/imported2', ['imports']),
            ('imports/imported3', ['imports']),
            ('imports/imported_duplicated_package', ['imports']),
            ('importing', ['.', 'imports']),
            ('no_package_importing', ['.', 'imports']),
            'no_package_imported',
            'int32',
            'int64',
            'message',
            'map',
            'no_package',
            'oneof',
            'options',
            'ordering',
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
                proto = f'tests/files/{spec[0]}.proto'
                options = []

                for include_path in spec[1]:
                    options += ['-I', f'tests/files/{include_path}']

                spec = os.path.basename(spec[0])
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
