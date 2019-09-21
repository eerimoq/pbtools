About
=====

Benchmark of protobuf libraries using gcc 8.3.0 on an Intel(R)
Core(TM) i7-4510U CPU @ 2.00GHz.

Usability
---------

pbtools
^^^^^^^

Easy to use.

nanopb
^^^^^^

Repeated, bytes, strings, oneofs and submessages are hard to use,
otherwise easy.

google
^^^^^^

Easy to use.

protobuf-c
^^^^^^^^^^

Fairly easy to use, but could use an allocator when building messages.

Executable size
---------------

pbtools
^^^^^^^

.. code-block::

   $ size main-size
      text    data     bss     dec     hex filename
     14076     632       8   14716    397c main-size
   $ size main-size-lto
      text    data     bss     dec     hex filename
     11612     632       8   12252    2fdc main-size-lto
   $ size main-speed
      text    data     bss     dec     hex filename
     32132     656       8   32796    801c main-speed
   $ size main-speed-lto
      text    data     bss     dec     hex filename
     44908     664       8   45580    b20c main-speed-lto

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

Execution time
--------------

Encoding ``Message`` and ``Message3`` 2,000,000 times each for each
library and optimization combination.

+------------+--------------+--------------------+--------+
| Library    | Optimization | Exexution time [s] | Ratio  |
+============+==============+====================+========+
| pbtools    |    -O3 -flto |              0.723 |  100 % |
+------------+--------------+--------------------+--------+
| pbtools    |          -O3 |              1.097 |  151 % |
+------------+--------------+--------------------+--------+
| protobuf-c |    -O3 -flto |              1.883 |  260 % |
+------------+--------------+--------------------+--------+
| protobuf-c |          -O3 |              1.892 |  261 % |
+------------+--------------+--------------------+--------+
| protobuf-c |          -Os |              1.970 |  272 % |
+------------+--------------+--------------------+--------+
| protobuf-c |    -Os -flto |              2.025 |  280 % |
+------------+--------------+--------------------+--------+
| google     |          -O3 |              2.231 |  308 % |
+------------+--------------+--------------------+--------+
| pbtools    |    -Os -flto |              3.584 |  495 % |
+------------+--------------+--------------------+--------+
| pbtools    |          -Os |              3.607 |  498 % |
+------------+--------------+--------------------+--------+
| google     |          -Os |              3.713 |  513 % |
+------------+--------------+--------------------+--------+
| nanopb     |    -O3 -flto |              7.453 | 1030 % |
+------------+--------------+--------------------+--------+
| nanopb     |          -O3 |              8.480 | 1172 % |
+------------+--------------+--------------------+--------+
| nanopb     |          -Os |             12.958 | 1792 % |
+------------+--------------+--------------------+--------+
| nanopb     |    -Os -flto |             13.191 | 1824 % |
+------------+--------------+--------------------+--------+

Decoding ``Message`` and ``Message3`` 2,000,000 times each for each
library and optimization combination.

+------------+--------------+--------------------+--------+
| Library    | Optimization | Exexution time [s] | Ratio  |
+============+==============+====================+========+
| pbtools    |    -O3 -flto |              0.677 |  100 % |
+------------+--------------+--------------------+--------+
| pbtools    |          -O3 |              1.002 |  148 % |
+------------+--------------+--------------------+--------+
| pbtools    |    -Os -flto |              1.270 |  187 % |
+------------+--------------+--------------------+--------+
| pbtools    |          -Os |              1.857 |  274 % |
+------------+--------------+--------------------+--------+
| google     |          -O3 |              1.997 |  294 % |
+------------+--------------+--------------------+--------+
| google     |          -Os |              2.350 |  347 % |
+------------+--------------+--------------------+--------+
| protobuf-c |    -O3 -flto |              2.501 |  369 % |
+------------+--------------+--------------------+--------+
| protobuf-c |          -O3 |              2.508 |  370 % |
+------------+--------------+--------------------+--------+
| protobuf-c |    -Os -flto |              2.602 |  384 % |
+------------+--------------+--------------------+--------+
| protobuf-c |          -Os |              2.609 |  385 % |
+------------+--------------+--------------------+--------+
| nanopb     |    -O3 -flto |              5.767 |  851 % |
+------------+--------------+--------------------+--------+
| nanopb     |          -O3 |              6.133 |  905 % |
+------------+--------------+--------------------+--------+
| nanopb     |          -Os |              9.685 | 1430 % |
+------------+--------------+--------------------+--------+
| nanopb     |    -Os -flto |             10.035 | 1482 % |
+------------+--------------+--------------------+--------+

Help
----

.. code-block:: Text

   $ git clone https://github.com/nanopb/nanopb.git nanopb-repo
   $ protoc \
         --plugin=protoc-gen-nanopb=nanopb-repo/generator/protoc-gen-nanopb \
         -I=nanopb \
         -I=nanopb-repo/generator/proto \
         --nanopb_out=nanopb \
         benchmark.proto
