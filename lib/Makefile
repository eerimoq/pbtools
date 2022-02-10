CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar

LIBRARY = libpbtools.a
PREFIX ?= /usr/local

.PHONY: library install clean

library: $(LIBRARY)

install:
	find include -type f -exec install -Dm 644 "{}" "$(PREFIX)/{}" \;
	mkdir -p $(PREFIX)/lib
	install -m 644 $(LIBRARY) $(PREFIX)/lib

clean:
	rm $(LIBRARY)

$(LIBRARY):
	$(CC) $(CFLAGS_EXTRA) \
	    -Wall -O2 -std=gnu11 -Iinclude src/pbtools.c -c \
	    -o pbtools.o
	$(AR) cr $(LIBRARY) pbtools.o
