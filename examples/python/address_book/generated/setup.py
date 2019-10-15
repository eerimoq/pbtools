#!/usr/bin/env python3

from setuptools import setup
from setuptools import Extension


setup(name='address_book',
      ext_modules=[
          Extension(name="address_book",
                    sources=[
                        "src/address_book.c",
                        "src/pbtools_py.c",
                        "src/c/address_book.c",
                        "src/c/pbtools.c"
                    ])
      ])
