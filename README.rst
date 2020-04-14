|buildstatus|_
|appveyor|_
|coverage|_
|codecov|_
|nala|_

About
=====

`Google Protocol Buffers`_ tools in Python 3.6+.

- `C` source code generator.

- `proto3`_ language parser.

Known limitations:

- Options, services (gRPC) and reserved fields are ignored.

- Public imports are not implemented.

Project homepage: https://github.com/eerimoq/pbtools

Documentation: https://pbtools.readthedocs.io

Installation
============

.. code-block:: python

   pip install pbtools

C source code design
====================

The C source code is designed with the following in mind:

- Clean and easy to use API.

- No malloc/free. Uses a workspace/areana for memory allocations.

- Fast encoding and decoding.

- Small memory footprint.

Known limitations:

- Recursive messages are not supported.

- Submessage presence detection isn't implemented.

- ``char`` must be 8 bits.

Support for recursive messages and submessage presence detection has
been implemented on branch
``submessage-presence-and-recursive-messages``. However, it makes the
generated code slightly harder to use, so it has not been merged to
the master branch yet.

Memory management
-----------------

A workspace, or arena, is used to allocate memory when encoding and
decoding messages. For simplicity, allocated memory can't be freed,
which puts restrictions on how a message can be modified between
encodings (if one want to do that). Scalar value type fields (ints,
strings, bytes, etc.) can be modified, but the length of repeated
fields can't.

Benchmark
---------

See `benchmark`_ for a benchmark of a few C/C++ protobuf libraries.

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

   $ pbtools generate_c_source examples/c/hello_world/hello_world.proto
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

Encode and decode the Foo-message in `main.c`_.

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

Build and run the program.

.. code-block:: text

   $ gcc main.c hello_world.c pbtools.c -o main
   $ ./main
   Successfully encoded Foo into 2 bytes.
   Successfully decoded 2 bytes into Foo.
   Foo.bar: 78

See `c/hello_world`_ for all files used in this example.

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

.. |appveyor| image:: https://ci.appveyor.com/api/projects/status/github/eerimoq/pbtools?svg=true
.. _appveyor: https://ci.appveyor.com/project/eerimoq/pbtools/branch/master

.. |coverage| image:: https://coveralls.io/repos/github/eerimoq/pbtools/badge.svg?branch=master
.. _coverage: https://coveralls.io/github/eerimoq/pbtools

.. |codecov| image:: https://codecov.io/gh/eerimoq/pbtools/branch/master/graph/badge.svg
.. _codecov: https://codecov.io/gh/eerimoq/pbtools

.. |nala| image:: https://img.shields.io/badge/nala-test-blue.svg
.. _nala: https://github.com/eerimoq/nala

.. _Google Protocol Buffers: https://developers.google.com/protocol-buffers

.. _proto3: https://developers.google.com/protocol-buffers/docs/proto3

.. _address_book.h: https://github.com/eerimoq/pbtools/blob/master/examples/c/address_book/generated/address_book.h

.. _address_book.c: https://github.com/eerimoq/pbtools/blob/master/examples/c/address_book/generated/address_book.c

.. _hello_world.proto: https://github.com/eerimoq/pbtools/blob/master/examples/c/hello_world/hello_world.proto

.. _hello_world.h: https://github.com/eerimoq/pbtools/blob/master/examples/c/hello_world/generated/hello_world.h

.. _hello_world.c: https://github.com/eerimoq/pbtools/blob/master/examples/c/hello_world/generated/hello_world.c

.. _main.c: https://github.com/eerimoq/pbtools/blob/master/examples/c/hello_world/main.c

.. _c/hello_world: https://github.com/eerimoq/pbtools/blob/master/examples/c/hello_world

.. _benchmark: https://github.com/eerimoq/pbtools/blob/master/benchmark
