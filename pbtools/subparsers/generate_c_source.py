import os
import shutil

from ..parser import parse_file
from ..c_source import camel_to_snake_case
from ..c_source import generate


SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))


def _do_generate_c_source(args):
    for filename in args.infiles:
        parsed = parse_file(filename)

        basename = os.path.basename(filename)
        name = os.path.splitext(basename)[0]
        name = camel_to_snake_case(name)

        filename_h = f'{name}.h'
        filename_c = f'{name}.c'

        header, source = generate(name, parsed, filename_h)

        with open(filename_h, 'w') as fout:
            fout.write(header)

        with open(filename_c, 'w') as fout:
            fout.write(source)

        print(f'Successfully generated {filename_h} and {filename_c}.')

    for filename in ['pbtools.h', 'pbtools.c']:
        shutil.copy(
            os.path.join(SCRIPT_DIR, f'../c_source/{filename}'),
            '.')

    print(f'Successfully created pbtools.h and pbtools.c.')


def add_subparser(subparsers):
    generate_c_source_parser = subparsers.add_parser(
        'generate_c_source',
        description='Generate C source code from given protobuf file(s).')
    generate_c_source_parser.add_argument(
        '-i', '--include-path',
        action='append',
        help='Path(s) where to search for imports.')
    generate_c_source_parser.add_argument(
        'infiles',
        nargs='+',
        help='Input protobuf file(s).')
    generate_c_source_parser.set_defaults(func=_do_generate_c_source)
