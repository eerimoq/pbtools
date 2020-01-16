About
=====

Benchmark of protobuf libraries using gcc 8.3.0 on an Intel(R)
Core(TM) i7-4510U CPU @ 2.00GHz.

Libraries in this benchmark:

- `FlatBuffers`_, an efficient cross platform serialization library
  for C++.

- `Protocol Buffers for C++`_, the official Google implementation.

- `Nanopb`_, protocol buffers with small code size.

- `pbtools`_, this library.

- `protobuf-c`_, Protocol Buffers implementation in C.

Usability
---------

+-------------+---------------------------------------------------------+
| Library     | Comment                                                 |
+=============+=========================================================+
| flatbuffers | Easy to use.                                            |
+-------------+---------------------------------------------------------+
| google      | Easy to use.                                            |
+-------------+---------------------------------------------------------+
| nanopb      | Repeated, bytes, strings, oneofs and submessages are    |
|             | hard to use, otherwise easy.                            |
+-------------+---------------------------------------------------------+
| pbtools     | Easy to use.                                            |
+-------------+---------------------------------------------------------+
| protobuf-c  | Fairly easy to use, but could use an allocator when     |
|             | building messages.                                      |
+-------------+---------------------------------------------------------+

Encoding time
-------------

Encoding time in milliseconds per library. Lower is better.

.. code-block:: text

   flatbuffers: ▇▇▇▇▇▇▇▇▇▇▇▇▇ 2506
   google     : ▇▇▇▇▇▇▇▇▇▇▇ 2231
   nanopb     : ▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇ 9580
   pbtools    : ▇▇▇▇▇▇▇ 1457
   protobuf-c : ▇▇▇▇▇▇▇▇▇ 1892

Decoding time
-------------

Decoding time in milliseconds per library. Lower is better.

.. code-block:: text

   flatbuffers: ▏ 100
   google     : ▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇ 1997
   nanopb     : ▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇ 6433
   pbtools    : ▇▇▇▇▇▇▇▇▇▇▇▇ 1594
   protobuf-c : ▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇ 2508

NOTE: `flatbuffers` decodes fields when used by the application, while
other libraries decodes everything before any field can be used. The
`flatbuffers` benchmark uses all fields once, while other libraries
decodes all fields but does not use any.

Executable size
---------------

Text segment size in bytes. Lower is better.

.. code-block:: text

   flatbuffers: ▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇ 40017
   nanopb     : ▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇ 21256
   pbtools    : ▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇ 17484

See `Executable size` in the `Details` section below for information
about `google` and `protobuf-c`.

NOTE: The `pbtools` executabe size probably grows faster than others
as it generates functions instead of tables.

Details
=======

Encoding time
-------------

Encoding ``Message`` and ``Message3`` 2,000,000 times each for each
library and optimization combination.

+-------------+--------------+--------------------+
| Library     | Optimization | Exexution time [s] |
+=============+==============+====================+
| pbtools     |    -O2 -flto |              1.319 |
+-------------+--------------+--------------------+
| pbtools     |          -O2 |              1.457 |
+-------------+--------------+--------------------+
| protobuf-c  |    -O3 -flto |              1.883 |
+-------------+--------------+--------------------+
| protobuf-c  |          -O3 |              1.892 |
+-------------+--------------+--------------------+
| protobuf-c  |          -Os |              1.970 |
+-------------+--------------+--------------------+
| protobuf-c  |    -Os -flto |              2.025 |
+-------------+--------------+--------------------+
| google      |          -O3 |              2.231 |
+-------------+--------------+--------------------+
| flatbuffers |          -O2 |              2.506 |
+-------------+--------------+--------------------+
| pbtools     |    -Os -flto |              3.275 |
+-------------+--------------+--------------------+
| pbtools     |          -Os |              3.398 |
+-------------+--------------+--------------------+
| google      |          -Os |              3.713 |
+-------------+--------------+--------------------+
| flatbuffers |          -Os |              5.553 |
+-------------+--------------+--------------------+
| nanopb      |    -O2 -flto |              9.453 |
+-------------+--------------+--------------------+
| nanopb      |          -O2 |              9.580 |
+-------------+--------------+--------------------+
| nanopb      |          -Os |             12.958 |
+-------------+--------------+--------------------+
| nanopb      |    -Os -flto |             13.191 |
+-------------+--------------+--------------------+

Decoding time
-------------

Decoding ``Message`` and ``Message3`` 2,000,000 times each for each
library and optimization combination.

+-------------+--------------+--------------------+
| Library     | Optimization | Exexution time [s] |
+=============+==============+====================+
| flatbuffers |          -O2 |              0.100 |
+-------------+--------------+--------------------+
| flatbuffers |          -Os |              0.549 |
+-------------+--------------+--------------------+
| pbtools     |    -O2 -flto |              1.239 |
+-------------+--------------+--------------------+
| pbtools     |    -Os -flto |              1.416 |
+-------------+--------------+--------------------+
| pbtools     |          -O2 |              1.594 |
+-------------+--------------+--------------------+
| pbtools     |          -Os |              1.749 |
+-------------+--------------+--------------------+
| google      |          -O3 |              1.997 |
+-------------+--------------+--------------------+
| google      |          -Os |              2.350 |
+-------------+--------------+--------------------+
| protobuf-c  |    -O3 -flto |              2.501 |
+-------------+--------------+--------------------+
| protobuf-c  |          -O3 |              2.508 |
+-------------+--------------+--------------------+
| protobuf-c  |    -Os -flto |              2.602 |
+-------------+--------------+--------------------+
| protobuf-c  |          -Os |              2.609 |
+-------------+--------------+--------------------+
| nanopb      |    -O2 -flto |              6.367 |
+-------------+--------------+--------------------+
| nanopb      |          -O2 |              6.433 |
+-------------+--------------+--------------------+
| nanopb      |          -Os |              9.685 |
+-------------+--------------+--------------------+
| nanopb      |    -Os -flto |             10.035 |
+-------------+--------------+--------------------+

Executable size
---------------

pbtools
^^^^^^^

.. code-block::

   size main-size
      text    data     bss     dec     hex filename
     14188     632       8   14828    39ec main-size
   size main-size-lto
      text    data     bss     dec     hex filename
     11732     632       8   12372    3054 main-size-lto
   size main-speed
      text    data     bss     dec     hex filename
     17484     656       8   18148    46e4 main-speed
   size main-speed-lto
      text    data     bss     dec     hex filename
     15308     656       8   15972    3e64 main-speed-lto

nanopb
^^^^^^

.. code-block::

   size main-size
      text    data     bss     dec     hex filename
     18036    1066      32   19134    4abe main-size
   size main-size-lto
      text    data     bss     dec     hex filename
     17002    1050      32   18084    46a4 main-size-lto
   size main-speed
      text    data     bss     dec     hex filename
     21256    1082      32   22370    5762 main-speed
   size main-speed-lto
      text    data     bss     dec     hex filename
     20566    1066      32   21664    54a0 main-speed-lto

google (C++ with libprotoc 3.6.1 as shared library)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   $ size main-size
      text    data     bss     dec     hex filename
     58440    3032     664   62136    f2b8 main-size
   $ size main-speed
      text    data     bss     dec     hex filename
     77538    3032     664   81234   13d52 main-speed

protobuf-c (libprotobuf-c statically linked)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

   $ size main-size
      text    data     bss     dec     hex filename
    738258   25364    5760  769382   bbd66 main-size
   $ size main-size-lto
      text    data     bss     dec     hex filename
    738034   25364    5760  769158   bbc86 main-size-lto
   $ size main-speed
      text    data     bss     dec     hex filename
    738914   25236    5760  769910   bbf76 main-speed
   $ size main-speed-lto
      text    data     bss     dec     hex filename
    739010   25236    5760  770006   bbfd6 main-speed-lto

flatbuffers
^^^^^^^^^^^

.. code-block::

   size main-size
      text    data     bss     dec     hex filename
     19838     816       8   20662    50b6 main-size
   size main-speed
      text    data     bss     dec     hex filename
     40017     728       8   40753    9f31 main-speed

.. _FlatBuffers: https://google.github.io/flatbuffers/

.. _Protocol Buffers for C++: https://developers.google.com/protocol-buffers/docs/cpptutorial

.. _Nanopb: https://jpa.kapsi.fi/nanopb/

.. _pbtools: https://github.com/eerimoq/pbtools

.. _protobuf-c: https://github.com/protobuf-c/protobuf-c
