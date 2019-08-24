About
=====

Benchmark of protobuf libraries.

Executable size
---------------

pbtools:

.. code-block::

   text	   data	    bss	    dec	    hex	filename
   2672	    632	     16	   3320	    cf8	main

nanopb:

.. code-block::

   text	   data	    bss	    dec	    hex	filename
   6617	    642	     16	   7275	   1c6b	main

Execution time
--------------

Building and encoding the message 20,000,000 times for each tool and
optimization combination.

+---------+--------------+--------------------+
| Tool    | Optimization | Exexution time [s] |
+=========+==============+====================+
| pbtools |          -O3 |              0.684 |
+---------+--------------+--------------------+
| pbtools |          -Os |              0.864 |
+---------+--------------+--------------------+
| nanopb  |          -O3 |              1.355 |
+---------+--------------+--------------------+
| nanopb  |          -Os |              2.025 |
+---------+--------------+--------------------+

Help
----

.. code-block:: Text

   $ git clone https://github.com/nanopb/nanopb.git nanopb-repo
   $ protoc \
         --plugin=protoc-gen-nanopb=nanopb-repo/generator/protoc-gen-nanopb \
         --nanopb_out=nanopb \
         benchmark.proto
