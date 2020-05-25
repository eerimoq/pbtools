all:
	gcc \
	    -O2 \
	    -Wall \
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
		python3 -m pbtools generate_c_source ../repeated.proto
