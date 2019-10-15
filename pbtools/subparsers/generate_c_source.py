import os
import shutil

from ..parser import parse_file
from ..parser import camel_to_snake_case
from ..c_source import generate


SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))


def _do_generate_c_source(args):
    for filename in args.infiles:
        parsed = parse_file(filename, args.import_path)
        basename = os.path.basename(filename)
        name = camel_to_snake_case(os.path.splitext(basename)[0])

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
    subparser = subparsers.add_parser(
        'generate_c_source',
        description='Generate C source code from given protobuf file(s).')
    subparser.add_argument(
        '-I', '--import-path',
        action='append',
        default=[],
        help='Path(s) where to search for imports.')
    subparser.add_argument(
        'infiles',
        nargs='+',
        help='Input protobuf file(s).')
    subparser.set_defaults(func=_do_generate_c_source)
