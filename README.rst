|buildstatus|_
|coverage|_
|codecov|_

About
=====

Google Protocol Buffers tools in Python 3.6+.

- `C` source code generator.

- Only supports proto3.

Known limitations:

- Imports and recursive messages are not yet supported.

- Options, services (gRPC) and maps are ignored.

- The `C` type ``char`` must be 8 bits.

Project homepage: https://github.com/eerimoq/pbtools

Documentation: http://pbtools.readthedocs.org/en/latest

Installation
============

.. code-block:: python

    pip install pbtools

Example usage
=============

C source code
-------------

In this example we use the simple proto-file `hello_world.proto`_.

.. code-block:: proto

   syntax = "proto3";

   package hello_world;

   message Foo {
       int32 bar = 1;
   }

Generate C source code from the proto-file.

.. code-block:: text

   $ pbtools generate_c_source examples/hello_world/hello_world.proto
   Successfully generated hello_world.h and hello_world.c.
   Successfully created pbtools.h and pbtools.c.

See `hello_world.h`_ and `hello_world.c`_ for the contents of the
generated files.

We'll use the generated types and functions below.

.. code-block:: c

   struct hello_world_foo_t {
      struct pbtools_message_base_t base;
      int32_t bar;
   };

   struct hello_world_foo_t *hello_world_foo_new(
       void *workspace_p,
       size_t size);

   int hello_world_foo_encode(
       struct hello_world_foo_t *self_p,
       void *encoded_p,
       size_t size);

   int hello_world_foo_decode(
       struct hello_world_foo_t *self_p,
       const uint8_t *encoded_p,
       size_t size);

Encode and decode the Foo-message.

.. code-block:: c

   #include <stdio.h>
   #include "hello_world.h"

   int main(int argc, const char *argv[])
   {
       int size;
       uint8_t workspace[64];
       uint8_t encoded[16];
       struct hello_world_foo_t *foo_p;

       /* Encode. */
       foo_p = hello_world_foo_new(&workspace[0], sizeof(workspace));

       if (foo_p == NULL) {
           return (1);
       }

       foo_p->bar = 78;
       size = hello_world_foo_encode(foo_p, &encoded[0], sizeof(encoded));

       if (size < 0) {
           return (2);
       }

       printf("Successfully encoded Foo into %d bytes.\n", size);

       /* Decode. */
       foo_p = hello_world_foo_new(&workspace[0], sizeof(workspace));

       if (foo_p == NULL) {
           return (3);
       }

       size = hello_world_foo_decode(foo_p, &encoded[0], size);

       if (size < 0) {
           return (4);
       }

       printf("Successfully decoded %d bytes into Foo.\n", size);
       printf("Foo.bar: %d\n", foo_p->bar);

       return (0);
   }

See `hello_world`_ for all files used in this example.

Command line tool
-----------------

The generate C source subcommand
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Below is an example of how to generate C source code from a
proto-file.

.. code-block:: text

   $ pbtools generate_c_source examples/address_book/address_book.proto
   Successfully generated address_book.h and address_book.c.
   Successfully created pbtools.h and pbtools.c.

See `address_book.h`_ and `address_book.c`_ for the contents of the
generated files.

.. |buildstatus| image:: https://travis-ci.org/eerimoq/pbtools.svg?branch=master
.. _buildstatus: https://travis-ci.org/eerimoq/pbtools

.. |coverage| image:: https://coveralls.io/repos/github/eerimoq/pbtools/badge.svg?branch=master
.. _coverage: https://coveralls.io/github/eerimoq/pbtools

.. |codecov| image:: https://codecov.io/gh/eerimoq/pbtools/branch/master/graph/badge.svg
.. _codecov: https://codecov.io/gh/eerimoq/pbtools

.. _address_book.h: https://github.com/eerimoq/pbtools/blob/master/examples/address_book/generated/address_book.h

.. _address_book.c: https://github.com/eerimoq/pbtools/blob/master/examples/address_book/generated/address_book.c

.. _hello_world.proto: https://github.com/eerimoq/pbtools/blob/master/examples/hello_world/hello_world.proto

.. _hello_world.h: https://github.com/eerimoq/pbtools/blob/master/examples/hello_world/generated/hello_world.h

.. _hello_world.c: https://github.com/eerimoq/pbtools/blob/master/examples/hello_world/generated/hello_world.c

.. _hello_world: https://github.com/eerimoq/pbtools/blob/master/examples/hello_world
