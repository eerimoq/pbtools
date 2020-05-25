ARGS = \
	-I ../../lib/include \
	$(CFLAGS) \
	*.c \
	../../lib/src/pbtools.c

all:
	gcc $(ARGS) $(OPT_SIZE) -o main-size
	gcc $(ARGS) $(OPT_SIZE) -flto -o main-size-lto
	gcc $(ARGS) $(OPT_SPEED) -o main-speed
	gcc $(ARGS) $(OPT_SPEED) -flto -o main-speed-lto
	readelf -s -W main-size | sort -k3 -n
	readelf -s -W main-size-lto | sort -k3 -n
	size main-size
	size main-size-lto
	size main-speed
	size main-speed-lto
	time ./main-size encode $(ITERATIONS)
	time ./main-size decode $(ITERATIONS)
	time ./main-size-lto encode $(ITERATIONS)
	time ./main-size-lto decode $(ITERATIONS)
	time ./main-speed encode $(ITERATIONS)
	time ./main-speed decode $(ITERATIONS)
	time ./main-speed-lto encode $(ITERATIONS)
	time ./main-speed-lto decode $(ITERATIONS)

generate:
	env PYTHONPATH=../.. \
	    python3 -m pbtools generate_c_source ../benchmark.proto

include ../common.mk
