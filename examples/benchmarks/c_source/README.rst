About
=====

Benchmark of protobuf libraries.

.. code-block:: Text

   $ git clone https://github.com/nanopb/nanopb.git nanopb-repo
   $ protoc \
         --plugin=protoc-gen-nanopb=nanopb-repo/generator/protoc-gen-nanopb \
         --nanopb_out=nanopb \
         benchmark.proto
