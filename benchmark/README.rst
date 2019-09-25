About
=====

Benchmark of protobuf libraries using gcc 8.3.0 on an Intel(R)
Core(TM) i7-4510U CPU @ 2.00GHz.

Usability
---------

+-------------+---------------------------------------------------------+
| Library     | Comment                                                 |
+=============+=========================================================+
| pbtools     | Easy to use.                                            |
+-------------+---------------------------------------------------------+
| nanopb      | Repeated, bytes, strings, oneofs and submessages are    |
|             | hard to use, otherwise easy.                            |
+-------------+---------------------------------------------------------+
| google      | Easy to use.                                            |
+-------------+---------------------------------------------------------+
| protobuf-c  | Fairly easy to use, but could use an allocator when     |
|             | building messages.                                      |
+-------------+---------------------------------------------------------+
| flatbuffers | Easy to use.                                            |
+-------------+---------------------------------------------------------+

Executable size
---------------

Text segment size in bytes. Lower is better.

.. code-block:: text

   flatbuffers: ▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇ 35049
   nanopb     : ▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇ 26928
   pbtools    : ▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇ 32132

See `Executable size` in the `Details` section below for information
about `google` and `protobuf-c`.

Execution time
--------------

Encoding time in milliseconds per library. Lower is better.

.. code-block:: text

   flatbuffers: ▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇ 2554
   google     : ▇▇▇▇▇▇▇▇▇▇▇▇▇ 2231
   nanopb     : ▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇ 8480
   pbtools    : ▇▇▇▇▇▇ 1097
   protobuf-c : ▇▇▇▇▇▇▇▇▇▇▇ 1892

Decoding time in milliseconds per library. Lower is better.

.. code-block:: text

   flatbuffers: ▏ 12
   google     : ▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇ 1997
   nanopb     : ▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇ 6133
   pbtools    : ▇▇▇▇▇▇▇▇ 1002
   protobuf-c : ▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇▇ 2508

NOTE: `flatbuffers` decodes fields when useed by the application,
while other libraries decodes everything before any field can be
used. The `flatbuffers` benchmark only uses one field, while other
libraries decodes all fields but does not use any.

Details
=======

Executable size
---------------

pbtools
^^^^^^^

.. code-block::

   $ size main-size
      text    data     bss     dec     hex filename
     13972     632       8   14612    3914 main-size
   $ size main-size-lto
      text    data     bss     dec     hex filename
     11596     632       8   12236    2fcc main-size-lto
   $ size main-speed
      text    data     bss     dec     hex filename
     32148     656       8   32812    802c main-speed
   $ size main-speed-lto
      text    data     bss     dec     hex filename
     66636     656       8   67300   106e4 main-speed-lto

nanopb
^^^^^^

.. code-block::

   $ size main-size
      text    data     bss     dec     hex filename
     18036    1066      32   19134    4abe main-size
   $ size main-size-lto
      text    data     bss     dec     hex filename
     17002    1050      32   18084    46a4 main-size-lto
   $ size main-speed
      text    data     bss     dec     hex filename
     26928    1082      32   28042    6d8a main-speed
   $ size main-speed-lto
      text    data     bss     dec     hex filename
     36574    1088      32   37694    933e main-speed-lto

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

   $ size main-size
      text    data     bss     dec     hex filename
     13510     816       8   14334    37fe main-size
   $ size main-speed
      text    data     bss     dec     hex filename
     35049     728       8   35785    8bc9 main-speed

Execution time
--------------

Encoding ``Message`` and ``Message3`` 2,000,000 times each for each
library and optimization combination.

+-------------+--------------+--------------------+
| Library     | Optimization | Exexution time [s] |
+=============+==============+====================+
| pbtools     |    -O3 -flto |              0.658 |
+-------------+--------------+--------------------+
| pbtools     |          -O3 |              1.118 |
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
| flatbuffers |          -O3 |              2.554 |
+-------------+--------------+--------------------+
| pbtools     |    -Os -flto |              3.421 |
+-------------+--------------+--------------------+
| pbtools     |          -Os |              3.470 |
+-------------+--------------+--------------------+
| google      |          -Os |              3.713 |
+-------------+--------------+--------------------+
| flatbuffers |          -Os |              5.638 |
+-------------+--------------+--------------------+
| nanopb      |    -O3 -flto |              7.453 |
+-------------+--------------+--------------------+
| nanopb      |          -O3 |              8.480 |
+-------------+--------------+--------------------+
| nanopb      |          -Os |             12.958 |
+-------------+--------------+--------------------+
| nanopb      |    -Os -flto |             13.191 |
+-------------+--------------+--------------------+

Decoding ``Message`` and ``Message3`` 2,000,000 times each for each
library and optimization combination.

+-------------+--------------+--------------------+
| Library     | Optimization | Exexution time [s] |
+=============+==============+====================+
| flatbuffers |          -O3 |              0.012 |
+-------------+--------------+--------------------+
| flatbuffers |          -Os |              0.023 |
+-------------+--------------+--------------------+
| pbtools     |    -O3 -flto |              0.681 |
+-------------+--------------+--------------------+
| pbtools     |          -O3 |              1.069 |
+-------------+--------------+--------------------+
| pbtools     |    -Os -flto |              1.270 |
+-------------+--------------+--------------------+
| pbtools     |          -Os |              1.576 |
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
| nanopb      |    -O3 -flto |              5.767 |
+-------------+--------------+--------------------+
| nanopb      |          -O3 |              6.133 |
+-------------+--------------+--------------------+
| nanopb      |          -Os |              9.685 |
+-------------+--------------+--------------------+
| nanopb      |    -Os -flto |             10.035 |
+-------------+--------------+--------------------+
