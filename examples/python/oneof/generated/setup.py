#!/usr/bin/env python3

from setuptools import setup
from setuptools import find_packages
from setuptools import Extension


setup(name='oneof',
      packages=find_packages(),
      ext_modules=[
          Extension(name="oneof",
                    sources=[
                        "src/oneof.c",
                        "src/pbtools_py.c",
                        "src/c/oneof.c",
                        "src/c/pbtools.c"
                    ])
      ])
