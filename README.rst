|buildstatus|_
|coverage|_

About
=====

Google Protocol Buffers tools in Python 3.

- `C` source code generator.

- Only supports proto3.

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

See `address_book.h`_ and `address_book.c`_ for the contents of the
generated files.

.. |buildstatus| image:: https://travis-ci.org/eerimoq/pbtools.svg?branch=master
.. _buildstatus: https://travis-ci.org/eerimoq/pbtools

.. |coverage| image:: https://coveralls.io/repos/github/eerimoq/pbtools/badge.svg?branch=master
.. _coverage: https://coveralls.io/github/eerimoq/pbtools

.. _address_book.h: https://github.com/eerimoq/pbtools/blob/master/examples/address_book/generated/address_book.h

.. _address_book.c: https://github.com/eerimoq/pbtools/blob/master/examples/address_book/generated/address_book.c
