all:
	gcc \
	    -Wall \
	    -g \
	    -Og \
	    -std=gnu11  \
	    -fprofile-arcs \
	    -ftest-coverage \
	    -I../src \
	    narwhal.c \
	    ../src/pbtools.c \
	    files/c_source/int32.c \
	    files/c_source/int64.c \
	    files/c_source/sint32.c \
	    files/c_source/sint64.c \
	    files/c_source/uint32.c \
	    files/c_source/uint64.c \
	    files/c_source/fixed32.c \
	    files/c_source/fixed64.c \
	    files/c_source/sfixed32.c \
	    files/c_source/sfixed64.c \
	    files/c_source/float.c \
	    files/c_source/double.c \
	    files/c_source/bool.c \
	    files/c_source/string.c \
	    files/c_source/bytes.c \
	    files/c_source/enum.c \
	    files/c_source/address_book.c \
	    files/c_source/tags.c \
	    files/c_source/oneof.c \
	    files/c_source/repeated.c \
	    main.c
	./a.out
