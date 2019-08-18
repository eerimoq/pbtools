all:
	gcc -g -Wall -Igenerated generated/*.c main.c -o main
	./main

generate:
	rm -rf generated
	mkdir -p generated
	cd generated && \
	    env PYTHONPATH=../.. \
		python3 -m pbtools generate_c_source address_book.proto
