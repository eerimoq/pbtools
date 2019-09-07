all:
	gcc \
	    -I../../../../src \
	    $(CFLAGS) \
	    $(OPT_SIZE) \
	    *.c \
	    ../../../../src/pbtools.c \
	    -o main-size
	size main-size
	readelf -s -W main-size | sort -k3 -n
	time ./main-size $(ENCODE_ITERATIONS)
	gcc \
	    -I../../../../src \
	    $(CFLAGS) \
	    $(OPT_SPEED) \
	    *.c \
	    ../../../../src/pbtools.c \
	    -o main-speed
	time ./main-speed $(ENCODE_ITERATIONS)

include ../common.mk
