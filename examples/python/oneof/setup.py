#!/usr/bin/env python3

from setuptools import setup
from setuptools import find_packages
from setuptools import Extension


setup(name='oneof',
      packages=find_packages(),
      ext_modules=[
          Extension(name="oneof",
                    sources=[
                        "oneof/oneof.c",
                        "oneof/pbtools_py.c",
                        "oneof/c/oneof.c",
                        "oneof/c/pbtools.c"
                    ])
      ])
