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

- Optional fields (added in protobuf 3.12) are not implemented.

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

- No malloc/free. Uses a workspace/arena for memory allocations.

- Fast encoding and decoding.

- Small memory footprint.

- Thread safety.

Known limitations:

- ``char`` must be 8 bits.

Memory management
-----------------

A workspace, or arena, is used to allocate memory when encoding and
decoding messages. For simplicity, allocated memory can't be freed,
which puts restrictions on how a message can be modified between
encodings (if one want to do that). Scalar value type fields (ints,
strings, bytes, etc.) can be modified, but the length of repeated
fields can't.

Scalar Value Types
------------------

Protobuf scalar value types are mapped to C types as shown in the
table below.

+---------------+--------------------------------------------+
| Protubuf Type | C Type                                     |
+===============+============================================+
| ``double``    | ``double``                                 |
+---------------+--------------------------------------------+
| ``float``     | ``float``                                  |
+---------------+--------------------------------------------+
| ``int32``     | ``int32_t``                                |
+---------------+--------------------------------------------+
| ``int64``     | ``int64_t``                                |
+---------------+--------------------------------------------+
| ``uint32``    | ``uint32_t``                               |
+---------------+--------------------------------------------+
| ``uint64``    | ``uint64_t``                               |
+---------------+--------------------------------------------+
| ``sint32``    | ``int32_t``                                |
+---------------+--------------------------------------------+
| ``sint64``    | ``int64_t``                                |
+---------------+--------------------------------------------+
| ``fixed32``   | ``int32_t``                                |
+---------------+--------------------------------------------+
| ``fixed64``   | ``int64_t``                                |
+---------------+--------------------------------------------+
| ``sfixed32``  | ``int32_t``                                |
+---------------+--------------------------------------------+
| ``sfixed64``  | ``int64_t``                                |
+---------------+--------------------------------------------+
| ``bool``      | ``bool``                                   |
+---------------+--------------------------------------------+
| ``string``    | ``char *``                                 |
+---------------+--------------------------------------------+
| ``bytes``     | ``struct { uint8_t *buf_p, size_t size }`` |
+---------------+--------------------------------------------+

Message
-------

A message is a struct in C.

For example, let's create a protocol specification.

.. code-block:: proto

   syntax = "proto3";

   package foo;

   message Bar {
       bool v1 = 1;
   }

   message Fie {
       int32 v2 = 1;
       Bar v3 = 2;
   }

One struct is generated per message.

.. code-block:: c

   struct foo_bar_t {
       bool v1;
   };

   struct foo_fie_t {
       int32_t v2;
       struct foo_bar_t v3;
   };

The generated code can encode and decode messages.

.. code-block:: c

   struct foo_fie_t *fie_p;

   /* Encode. */
   fie_p = foo_fie_new(...);
   fie_p->v2 = 5;
   fie_p->v3.v1 = true;
   foo_fie_encode(fie_p, ...);

   /* Decode. */
   fie_p = foo_fie_new(...);
   foo_fie_decode(fie_p, ...);
   printf("%d\n", fie_p->v2);
   printf("%d\n", fie_p->v3.v1);

Give ``--sub-message-pointers`` to enable sub-message presence
detection and support for recursive message.

The ``v3`` field is now a pointer.

.. code-block:: c

   struct foo_bar_t {
       bool v1;
   };

   struct foo_fie_t {
       int32_t v2;
       struct foo_bar_t *v3_p;
   };

``v3`` has to be allocated before encoding and checked if ``NULL``
after decoding.

.. code-block:: c

   struct foo_fie_t *fie_p;

   /* Encode. */
   fie_p = foo_fie_new(...);
   fie_p->v2 = 5;
   foo_fie_v3_alloc(fie_p);
   fie_p->v3_p->v1 = true;
   foo_fie_encode(fie_p, ...);

   /* Decode. */
   fie_p = foo_fie_new(...);
   foo_fie_decode(fie_p, ...);
   printf("%d\n", fie_p->v2);

   if (fie_p->v3_p != NULL) {
       printf("%d\n", fie_p->v3_p->v1);
   }

Oneof
-----

A oneof is an enum (the choice) and a union in C.

For example, let's create a protocol specification.

.. code-block:: proto

   syntax = "proto3";

   package foo;

   message Bar {
       oneof fie {
           int32 v1 = 1;
           bool v2 = 2;
       };
   }

One enum and one struct is generated per oneof.

.. code-block:: c

   enum foo_bar_fie_choice_e {
       foo_bar_fie_choice_none_e = 0,
       foo_bar_fie_choice_v1_e = 1,
       foo_bar_fie_choice_v2_e = 2
   };

   struct foo_bar_fie_oneof_t {
       enum foo_bar_fie_choice_e choice;
       union {
           int32_t v1;
           bool v2;
       } value;
   };

   struct foo_bar_t {
       struct foo_bar_fie_oneof_t fie;
   };

The generated code can encode and decode messages. Call
``_<field>_init()`` to select which oneof field to encode. Use the
``choice`` member to check which oneof field was decoded (if any).

.. code-block:: c

   struct foo_bar_t *bar_p;

   /* Encode with choice v1. */
   bar_p = foo_bar_new(...);
   foo_bar_fie_v1_init(bar_p);
   bar_p->fie.value.v1 = -2;
   foo_bar_encode(bar_p, ...);

   /* Decode. */
   bar_p = foo_bar_new(...);
   foo_bar_decode(bar_p, ...);

   switch (bar_p->fie.choice) {

   case foo_bar_fie_choice_none_e:
       printf("Not present.\n");
       break;

   case foo_bar_fie_choice_v1_e:
       printf("%d\n", bar_p->fie.value.v1);
       break;

   case foo_bar_fie_choice_v2_e:
       printf("%d\n", bar_p->fie.value.v2);
       break;

   default:
       printf("Can not happen.\n");
       break;
   }

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

   $ pbtools generate_c_source examples/hello_world/hello_world.proto

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

   $ gcc -I lib/include main.c hello_world.c lib/src/pbtools.c -o main
   $ ./main
   Successfully encoded Foo into 2 bytes.
   Successfully decoded 2 bytes into Foo.
   Foo.bar: 78

See `examples/hello_world`_ for all files used in this example.

Command line tool
-----------------

The generate C source subcommand
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Below is an example of how to generate C source code from a
proto-file.

Give ``--sub-message-pointers`` to make sub-messages pointers to
enable sub-message presence detection and support for recursive
messages.

.. code-block:: text

   $ pbtools generate_c_source examples/address_book/address_book.proto

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

.. _address_book.h: https://github.com/eerimoq/pbtools/blob/master/examples/address_book/generated/address_book.h

.. _address_book.c: https://github.com/eerimoq/pbtools/blob/master/examples/address_book/generated/address_book.c

.. _hello_world.proto: https://github.com/eerimoq/pbtools/blob/master/examples/hello_world/hello_world.proto

.. _hello_world.h: https://github.com/eerimoq/pbtools/blob/master/examples/hello_world/generated/hello_world.h

.. _hello_world.c: https://github.com/eerimoq/pbtools/blob/master/examples/hello_world/generated/hello_world.c

.. _main.c: https://github.com/eerimoq/pbtools/blob/master/examples/hello_world/main.c

.. _examples/hello_world: https://github.com/eerimoq/pbtools/blob/master/examples/hello_world

.. _benchmark: https://github.com/eerimoq/pbtools/blob/master/benchmark
