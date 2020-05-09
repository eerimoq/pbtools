import os
import unittest
from unittest.mock import patch
import shutil

import pbtools


def read_file(filename):
    with open(filename, 'r') as fin:
        return fin.read()


def remove_directory(name):
    if os.path.exists(name):
        shutil.rmtree(name)


class CommandLineTest(unittest.TestCase):

    maxDiff = None

    def assert_files_equal(self, actual, expected):
        # open(actual, 'w').write(open(expected, 'r').read())
        self.assertEqual(read_file(actual), read_file(expected))

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
            'uint64',
            'field_names',
            'add_and_remove_fields'
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
                self.assert_files_equal(f'tests/files/c_source/{filename}',
                                        filename)

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
            self.assert_files_equal(f'tests/files/c_source/{filename}',
                                    filename)

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
            self.assert_files_equal(filename,
                                    f'tests/files/c_source/{filename}')

        for filename in pbtools_filenames:
            self.assert_files_equal(filename,
                                    f'pbtools/c_source/{filename}')

    def test_command_line_generate_c_source_output_directory(self):
        argv = [
            'pbtools',
            'generate_c_source',
            '-o', 'generated',
            'tests/files/address_book.proto'
        ]

        remove_directory('generated')
        os.mkdir('generated')

        with patch('sys.argv', argv):
            pbtools._main()

        self.assertTrue(os.path.exists('generated/address_book.h'))
        self.assertTrue(os.path.exists('generated/address_book.c'))

if __name__ == '__main__':
    unittest.main()
