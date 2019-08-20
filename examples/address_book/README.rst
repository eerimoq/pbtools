ToDo: Decode fields in any order.

.. code-block:: text

   0a49 # person
     0a0a # name(1)
     4b616c6c65204b756c61
     10 # id(2)
     38
     1a15 # email(3)
     6b616c6c652e6b756c6140666f6f6261722e636f6d
     2210 # phones #1 (4)
       0a0c # number(1)
       2b3436373031323332333435
       10 # type(2)
       01
     2210 # phones #2 (4)
       0a0c # number(1)
       2b3436393939393939393939
       10 # type(2)
       02
