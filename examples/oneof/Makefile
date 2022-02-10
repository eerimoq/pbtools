all:
	gcc \
	    -O2 \
	    -Wall \
	    -Wno-gnu-alignof-expression \
	    -I ../../lib/include \
	    -Igenerated \
	    generated/*.c \
	    ../../lib/src/pbtools.c \
	    main.c \
	    -o main
	./main

generate:
	rm -rf generated
	mkdir -p generated
	cd generated && \
	    env PYTHONPATH=../../.. \
		python3 -m pbtools generate_c_source ../oneof.proto
