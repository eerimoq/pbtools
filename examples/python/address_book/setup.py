#!/usr/bin/env python3

from setuptools import setup
from setuptools import find_packages
from setuptools import Extension


setup(name='address_book',
      packages=find_packages(),
      ext_modules=[
          Extension(name="address_book",
                    sources=[
                        "address_book/address_book.c",
                        "address_book/pbtools_py.c",
                        "address_book/c/address_book.c",
                        "address_book/c/pbtools.c"
                    ])
      ])
