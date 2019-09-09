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

    for filename in ['pbtools.h', 'pbtools.c']:
        shutil.copy(
            os.path.join(SCRIPT_DIR, f'../c_source/{filename}'),
            '.')

    print(f'Successfully created pbtools.[hc] and {name}.[hc].')


def add_subparser(subparsers):
    generate_c_source_parser = subparsers.add_parser(
        'generate_c_source',
        description='Generate C source code from given protobuf file.')
    generate_c_source_parser.add_argument(
        'infiles',
        nargs='+',
        help='Input protobuf file(s).')
    generate_c_source_parser.set_defaults(func=_do_generate_c_source)
