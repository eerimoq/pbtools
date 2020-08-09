import os
import unittest
from unittest.mock import patch

import pbtools

from .utils import read_file
from .utils import remove_directory
from .utils import remove_files


class CommandLineTest(unittest.TestCase):

    maxDiff = None

    def assert_files_equal(self, actual, expected):
        # open(expected, 'w').write(open(actual, 'r').read())
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
            'optional_fields',
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

            remove_files([filename_h, filename_c])

            with patch('sys.argv', argv):
                pbtools._main()

            for filename in [filename_h, filename_c]:
                self.assert_files_equal(filename,
                                        f'tests/files/c_source/{filename}')

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

        remove_files(filenames)

        with patch('sys.argv', argv):
            pbtools._main()

        for filename in filenames:
            self.assert_files_equal(filename,
                                    f'tests/files/c_source/{filename}')

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

        remove_files(filenames)

        with patch('sys.argv', argv):
            pbtools._main()

        for filename in filenames:
            self.assert_files_equal(filename,
                                    f'tests/files/c_source/{filename}')

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

    def test_command_line_generate_c_source_sub_message_pointers(self):
        specs = [
            'sub_message_pointers_message',
            'sub_message_pointers_repeated'
        ]

        for spec in specs:
            proto = f'tests/files/{spec}.proto'
            argv = [
                'pbtools',
                'generate_c_source',
                '--sub-message-pointers',
                proto
            ]

            filename_h = f'{spec}.h'
            filename_c = f'{spec}.c'

            remove_files([filename_h, filename_c])

            with patch('sys.argv', argv):
                pbtools._main()

            for filename in [filename_h, filename_c]:
                self.assert_files_equal(filename,
                                        f'tests/files/c_source/{filename}')


if __name__ == '__main__':
    unittest.main()
