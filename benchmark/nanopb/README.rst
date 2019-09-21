Help
----

.. code-block:: Text

   $ git clone https://github.com/nanopb/nanopb.git nanopb-repo
   $ protoc \
         --plugin=protoc-gen-nanopb=nanopb-repo/generator/protoc-gen-nanopb \
         -I=. \
         -I=nanopb-repo/generator/proto \
         --nanopb_out=. \
         benchmark.proto
