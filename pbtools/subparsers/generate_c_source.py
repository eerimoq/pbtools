import os

from ..parser import camel_to_snake_case
from ..c_source import generate_files


def _do_generate_c_source(args):
    generate_files(args.import_path,
                   args.output_directory,
                   args.sub_message_pointers,
                   args.infiles)


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
        '--sub-message-pointers',
        action='store_true',
        help=('Make sub-messages pointers to enable sub-message presence detection '
              'and support for recursive messages.'))
    subparser.add_argument(
        'infiles',
        nargs='+',
        help='Input protobuf file(s).')
    subparser.set_defaults(func=_do_generate_c_source)
