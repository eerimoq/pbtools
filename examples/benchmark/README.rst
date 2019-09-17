About
=====

Benchmark of protobuf libraries using gcc 8.3.0 on an Intel(R)
Core(TM) i7-4510U CPU @ 2.00GHz.

Executable size
---------------

pbtools:

.. code-block::

   $ size main-size
      text    data     bss     dec     hex filename
     12587     632       8   13227    33ab main-size
   $ size main-size-lto
      text    data     bss     dec     hex filename
     10268     632       8   10908    2a9c main-size-lto
   $ size main-speed
      text    data     bss     dec     hex filename
     30244     656       8   30908    78bc main-speed
   $ size main-speed-lto
      text    data     bss     dec     hex filename
     35052     664       8   35724    8b8c main-speed-lto

nanopb:

.. code-block::

   $ size main-size
      text    data     bss     dec     hex filename
     16092    1066      32   17190    4326 main-size
   $ size main-size-lto
      text    data     bss     dec     hex filename
     14988    1066      32   16086    3ed6 main-size-lto
   $ size main-speed
      text    data     bss     dec     hex filename
     24696    1082      32   25810    64d2 main-speed
   $ size main-speed-lto
      text    data     bss     dec     hex filename
     28846    1096      32   29974    7516 main-speed-lto

Execution time
--------------

Encoding ``benchmark.Message`` 3,000,000 times for each library and
optimization combination.

+---------+--------------+--------------------+--------+
| Library | Optimization | Exexution time [s] | Ratio  |
+=========+==============+====================+========+
| pbtools |    -O3 -flto |              0.589 |  100 % |
+---------+--------------+--------------------+--------+
| pbtools |          -O3 |              0.883 |  150 % |
+---------+--------------+--------------------+--------+
| pbtools |    -Os -flto |              2.723 |  462 % |
+---------+--------------+--------------------+--------+
| pbtools |          -Os |              2.882 |  489 % |
+---------+--------------+--------------------+--------+
| nanopb  |    -O3 -flto |              6.783 | 1151 % |
+---------+--------------+--------------------+--------+
| nanopb  |          -O3 |              7.743 | 1314 % |
+---------+--------------+--------------------+--------+
| nanopb  |    -Os -flto |             12.282 | 2085 % |
+---------+--------------+--------------------+--------+
| nanopb  |          -Os |             12.594 | 2138 % |
+---------+--------------+--------------------+--------+

Decoding ``benchmark.Message`` 3,000,000 times for each library and
optimization combination.

+---------+--------------+--------------------+--------+
| Library | Optimization | Exexution time [s] | Ratio  |
+=========+==============+====================+========+
| pbtools |    -O3 -flto |              0.510 |  100 % |
+---------+--------------+--------------------+--------+
| pbtools |          -O3 |              0.642 |  125 % |
+---------+--------------+--------------------+--------+
| pbtools |    -Os -flto |              0.890 |  174 % |
+---------+--------------+--------------------+--------+
| pbtools |          -Os |              1.221 |  239 % |
+---------+--------------+--------------------+--------+
| nanopb  |    -O3 -flto |              4.485 |  879 % |
+---------+--------------+--------------------+--------+
| nanopb  |          -O3 |              4.720 |  925 % |
+---------+--------------+--------------------+--------+
| nanopb  |    -Os -flto |              7.494 | 1469 % |
+---------+--------------+--------------------+--------+
| nanopb  |          -Os |              7.757 | 1520 % |
+---------+--------------+--------------------+--------+

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
