import os

from ..parser import camel_to_snake_case
from ..c_source import generate_files
from ..c_source import GeneratorOptions


def _do_generate_c_source(args):
    gen_opts = GeneratorOptions()
    gen_opts.enum_prefix = args.enum_prefix
    gen_opts.enum_upper = args.enum_upper
    generate_files(args.infiles,
                   args.import_path,
                   args.output_directory,
                   gen_opts)


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
        '-o', '--output-directory',
        default='.',
        help='Output directory (default: %(default)s).')
    subparser.add_argument(
        'infiles',
        nargs='+',
        help='Input protobuf file(s).')
    subparser.add_argument(
        '--enum-prefix',
        default=2,
        type=int,
        help='set generated enum prefix, 0: none, 1: namespace, else: namespace and type')
    subparser.add_argument(
        '--enum-upper',
        action='store_true',
        help='use upper case for generated enum')
    subparser.set_defaults(func=_do_generate_c_source)
