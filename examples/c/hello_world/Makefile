all:
	gcc \
	    -O2 \
	    -Wall \
	    -I../../../pbtools/c_source \
	    -Igenerated \
	    generated/*.c \
	    ../../../pbtools/c_source/pbtools.c \
	    main.c \
	    -o main
	./main

generate:
	rm -rf generated
	mkdir -p generated
	cd generated && \
	    env PYTHONPATH=../../.. \
		python3 -m pbtools generate_c_source ../hello_world.proto
