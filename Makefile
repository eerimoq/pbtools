INC += pbtools/c_source/
SRC ?= \
	pbtools/c_source/pbtools.c \
	tests/files/c_source/int32.c \
	tests/files/c_source/int64.c \
	tests/files/c_source/sint32.c \
	tests/files/c_source/sint64.c \
	tests/files/c_source/uint32.c \
	tests/files/c_source/uint64.c \
	tests/files/c_source/fixed32.c \
	tests/files/c_source/fixed64.c \
	tests/files/c_source/sfixed32.c \
	tests/files/c_source/sfixed64.c \
	tests/files/c_source/float.c \
	tests/files/c_source/double.c \
	tests/files/c_source/bool.c \
	tests/files/c_source/string.c \
	tests/files/c_source/bytes.c \
	tests/files/c_source/enum.c \
	tests/files/c_source/address_book.c \
	tests/files/c_source/tags.c \
	tests/files/c_source/oneof.c \
	tests/files/c_source/repeated.c \
	tests/files/c_source/scalar_value_types.c \
	tests/files/c_source/benchmark.c \
	tests/files/c_source/message.c \
	tests/files/c_source/no_package.c \
	tests/files/c_source/ordering.c \
	tests/files/c_source/importing.c \
	tests/files/c_source/imported.c \
	tests/files/c_source/imported1.c \
	tests/files/c_source/imported2.c \
	tests/files/c_source/imported3.c \
	tests/files/c_source/imported_duplicated_package.c \
	tests/files/c_source/no_package_importing.c \
	tests/files/c_source/no_package_imported.c \
	tests/files/c_source/enum_user.c \
	tests/files/c_source/enum_provider.c \
	tests/files/c_source/field_names.c
CFLAGS += $(INC:%=-I%)
CFLAGS += -O2
CFLAGS += -c
CFLAGS += -Wall
CFLAGS += -Wextra

test:
	$(MAKE) test-python
	$(MAKE) test-c

test-python:
	python3 setup.py test

test-c:
	$(MAKE) -C tests
	$(MAKE) -C lib

test-c-avr:
	avr-gcc $(CFLAGS) $(SRC)

test-c-arm:
	arm-none-eabi-gcc $(CFLAGS) $(SRC)

test-c-fuzzer:
	$(MAKE) -C tests fuzzer

release-to-pypi:
	python setup.py sdist
	python setup.py bdist_wheel --universal
	twine upload dist/*

generate:
	$(MAKE) -C examples/c generate
	$(MAKE) -C benchmark/pbtools generate
	$(MAKE) -C tests fuzzer-generate
