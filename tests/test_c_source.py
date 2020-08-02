import os
import unittest

import pbtools.c_source

from .utils import read_file
from .utils import remove_directory
from .utils import remove_files


class CommandLineTest(unittest.TestCase):

    maxDiff = None

    def assert_files_equal(self, actual, expected):
        # open(expected, 'w').write(open(actual, 'r').read())
        self.assertEqual(read_file(actual), read_file(expected))

    def test_generate_files(self):
        spec = 'address_book'
        proto = f'tests/files/{spec}.proto'
        filename_h = f'{spec}.h'
        filename_c = f'{spec}.c'

        remove_files([filename_h, filename_c])

        pbtools.c_source.generate_files([proto])

        for filename in [filename_h, filename_c]:
            self.assert_files_equal(filename,
                                    f'tests/files/c_source/{filename}')


if __name__ == '__main__':
    unittest.main()
