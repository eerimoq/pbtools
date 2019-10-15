#!/usr/bin/env python3

from setuptools import setup
from setuptools import find_packages
from setuptools import Extension


setup(name='hello_world',
      packages=find_packages(),
      ext_modules=[
          Extension(name="hello_world", sources=["hello_world/hello_world.c"])
      ])
