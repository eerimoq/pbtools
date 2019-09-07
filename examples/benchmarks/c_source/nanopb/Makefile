all:
	gcc -I. $(CFLAGS) $(OPT_SIZE) *.c -o main
	size main
	readelf -s -W main | sort -k3 -n
	time ./main $(ENCODE_ITERATIONS)
	gcc -I. $(CFLAGS) $(OPT_SPEED) *.c -o main
	time ./main $(ENCODE_ITERATIONS)

include ../common.mk
