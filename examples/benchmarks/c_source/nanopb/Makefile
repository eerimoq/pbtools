all:
	gcc $(CFLAGS) $(OPT_SIZE) *.c -o main
	size main
	readelf -s -W main | sort -k3 -n
	# valgrind $(MASSIF_FLAGS) --massif-out-file=main.massif ./main 1
	# ms_print main.massif
	time ./main $(ENCODE_ITERATIONS)
	gcc $(CFLAGS) $(OPT_SPEED) *.c -o main
	time ./main $(ENCODE_ITERATIONS)

include ../common.mk
