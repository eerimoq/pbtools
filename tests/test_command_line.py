import sys
import os
import re
import unittest
from unittest.mock import patch

import pbtools


def remove_date_time(string):
    return re.sub(r'.* This file was generated.*', '', string)


def read_file(filename):
    with open(filename, 'r') as fin:
        return remove_date_time(fin.read())


class CommandLineTest(unittest.TestCase):

    maxDiff = None

    def test_command_line_generate_c_source(self):
        specs = [
            'int32'
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


if __name__ == '__main__':
    unittest.main()
