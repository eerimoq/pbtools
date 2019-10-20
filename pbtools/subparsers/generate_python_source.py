import os
import shutil

from ..parser import parse_file
from ..parser import camel_to_snake_case
from ..c_source import generate as generate_c
from ..python_source import generate as generate_python


SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))

SETUP_PY_FMT = '''\
from setuptools import setup
from setuptools import Extension


setup(name='{name}',
      ext_modules=[
          Extension(name='{name}',
                    sources=[
                        'src/{name}.c',
                        'src/pbtools_py.c',
                        'src/c/{name}.c',
                        'src/c/pbtools.c'
                    ])
      ])
'''


def generate_c_source(name, parsed, outdir):
    os.makedirs(outdir)

    filename_h = f'{name}.h'
    filename_c = f'{name}.c'

    header, source = generate_c(name, parsed, filename_h)

    with open(os.path.join(outdir, filename_h), 'w') as fout:
        fout.write(header)

    with open(os.path.join(outdir, filename_c), 'w') as fout:
        fout.write(source)

    for filename in ['pbtools.h', 'pbtools.c']:
        shutil.copy(
            os.path.join(SCRIPT_DIR, f'../c_source/{filename}'),
            outdir)


def generate_python_source(name, parsed, outdir):
    source = generate_python(name, parsed)

    with open(os.path.join(outdir, f'{name}.c'), 'w') as fout:
        fout.write(source)

    for filename in ['pbtools_py.h', 'pbtools_py.c']:
        shutil.copy(
            os.path.join(SCRIPT_DIR, f'../python_source/{filename}'),
            outdir)


def _do_generate_python_source(args):
    for filename in args.infiles:
        parsed = parse_file(filename, args.import_path)
        basename = os.path.basename(filename)
        name = camel_to_snake_case(os.path.splitext(basename)[0])

        if os.path.exists(name):
            sys.exit(f'{name} already exists.')

        generate_c_source(name, parsed, os.path.join(name, 'src', 'c'))
        generate_python_source(name, parsed, os.path.join(name, 'src'))

        with open(os.path.join(name, 'setup.py'), 'w',) as fout:
            fout.write(SETUP_PY_FMT.format(name=name))

        print(f'Successfully generated {name}/.')


def add_subparser(subparsers):
    subparser = subparsers.add_parser(
        'generate_python_source',
        description='Generate Python source code from given protobuf file(s).')
    subparser.add_argument(
        '-I', '--import-path',
        action='append',
        default=[],
        help='Path(s) where to search for imports.')
    subparser.add_argument(
        'infiles',
        nargs='+',
        help='Input protobuf file(s).')
    subparser.set_defaults(func=_do_generate_python_source)
