About
=====

Google protocol buffers tools in Python 3.

- `C` source code generator.

Project homepage: https://github.com/eerimoq/pbtools

Documentation: http://pbtools.readthedocs.org/en/latest

Installation
============

.. code-block:: python

    pip install pbtools

Example usage
=============

Command line tool
-----------------

The generate C source subcommand
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Below is an example of how to generate C source code from a
proto-file.

.. code-block:: text

   $ pbtools generate_c_source examples/address_book/address_book.proto
   Successfully generated address_book.h and address_book.c.
