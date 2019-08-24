import os

from ..parser import parse_file
from ..c_source import camel_to_snake_case
from ..c_source import generate


def _do_generate_c_source(args):
    parsed = parse_file(args.infile)

    basename = os.path.basename(args.infile)
    name = os.path.splitext(basename)[0]
    name = camel_to_snake_case(name)

    filename_h = f'{name}.h'
    filename_c = f'{name}.c'

    header, source = generate(name, parsed, filename_h)

    with open(filename_h, 'w') as fout:
        fout.write(header)

    with open(filename_c, 'w') as fout:
        fout.write(source)

    print('Successfully generated {} and {}.'.format(filename_h, filename_c))


def add_subparser(subparsers):
    generate_c_source_parser = subparsers.add_parser(
        'generate_c_source',
        description='Generate C source code from given protobuf file.')
    generate_c_source_parser.add_argument(
        'infile',
        help='Input protobuf file.')
    generate_c_source_parser.set_defaults(func=_do_generate_c_source)
