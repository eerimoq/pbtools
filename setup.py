#!/usr/bin/env python

from setuptools import setup
from setuptools import find_packages
import re


def find_version():
    return re.search(r"^__version__ = '(.*)'$",
                     open('pbtools/version.py', 'r').read(),
                     re.MULTILINE).group(1)


setup(name='pbtools',
      version=find_version(),
      description='Google protocol buffers tools.',
      long_description=open('README.rst', 'r').read(),
      author='Erik Moqvist',
      author_email='erik.moqvist@gmail.com',
      license='MIT',
      classifiers=[
          'License :: OSI Approved :: MIT License',
          'Programming Language :: Python :: 3',
      ],
      keywords=['protobuf', 'proto', 'protocol buffers'],
      url='https://github.com/eerimoq/pbtools',
      packages=find_packages(exclude=['tests']),
      install_requires=[
          'textparser>=0.21.1'
      ],
      test_suite="tests",
      entry_points = {
          'console_scripts': ['pbtools=pbtools.__init__:_main']
      })
