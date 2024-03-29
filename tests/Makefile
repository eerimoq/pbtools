FUZZER_CC ?= clang
FUZZER_EXE = main_fuzzer
FUZZER_C_SOURCES = \
	main_fuzzer.c \
	../lib/src/pbtools.c \
	files/c_source/fuzzer.c
FUZZER_CFLAGS = \
	-fprofile-instr-generate \
	-fcoverage-mapping \
	-I ../lib/include \
	-g \
	-fsanitize=address,fuzzer \
	-fsanitize=signed-integer-overflow \
	-fno-sanitize-recover=all
FUZZER_EXECUTION_TIME ?= 30

CFLAGS ?= \
	-Wno-gnu-alignof-expression \
	-Wno-unused-command-line-argument \
	-fprofile-arcs \
	-ftest-coverage \
	-fsanitize=address \
	-fsanitize=signed-integer-overflow \
	-Wdouble-promotion \
	-Wfloat-equal \
	-Wformat=2 \
	-Wshadow \
	-Werror \
	-O1 \
	-g \
	-no-pie \
	-std=gnu11 \
	-Wconversion \
	-Wno-sign-conversion \
	-Wall \
	-Wextra \
	-Wpedantic

all:
	gcc \
	    -D PBTOOLS_CONFIG_FLOAT=0 \
	    -I ../lib/include \
	    -c \
	    -Wno-gnu-alignof-expression \
	    ../lib/src/pbtools.c \
	    -o no-float.o
	gcc -O2 -g nala.c -c -o nala.o
	gcc -O2 -g -I ../lib/include main.c -c -o main.o
	gcc \
	    $(CFLAGS) \
	    -I ../lib/include \
	    nala.o \
	    ../lib/src/pbtools.c \
	    files/c_source/int32.c \
	    files/c_source/int64.c \
	    files/c_source/sint32.c \
	    files/c_source/sint64.c \
	    files/c_source/uint32.c \
	    files/c_source/uint64.c \
	    files/c_source/fixed32.c \
	    files/c_source/fixed64.c \
	    files/c_source/sfixed32.c \
	    files/c_source/sfixed64.c \
	    files/c_source/float.c \
	    files/c_source/double.c \
	    files/c_source/bool.c \
	    files/c_source/string.c \
	    files/c_source/bytes.c \
	    files/c_source/enum.c \
	    files/c_source/address_book.c \
	    files/c_source/tags.c \
	    files/c_source/oneof.c \
	    files/c_source/repeated.c \
	    files/c_source/scalar_value_types.c \
	    files/c_source/benchmark.c \
	    files/c_source/message.c \
	    files/c_source/no_package.c \
	    files/c_source/ordering.c \
	    files/c_source/optional_fields.c \
	    files/c_source/importing.c \
	    files/c_source/imported.c \
	    files/c_source/imported1.c \
	    files/c_source/imported2.c \
	    files/c_source/imported3.c \
	    files/c_source/imported_duplicated_package.c \
	    files/c_source/no_package_importing.c \
	    files/c_source/no_package_imported.c \
	    files/c_source/enum_user.c \
	    files/c_source/enum_provider.c \
	    files/c_source/field_names.c \
	    files/c_source/map.c \
	    files/c_source/add_and_remove_fields.c \
	    main.o
	./a.out

fuzzer-generate:
	cd files/c_source && \
	    env PYTHONPATH=../../.. \
	        python3 -m pbtools generate_c_source ../fuzzer.proto

fuzzer:
	$(FUZZER_CC) $(FUZZER_CFLAGS) $(FUZZER_C_SOURCES) -o $(FUZZER_EXE)
	rm -f $(FUZZER_EXE).profraw
	LLVM_PROFILE_FILE="$(FUZZER_EXE).profraw" \
	    ./$(FUZZER_EXE) \
	    -max_total_time=$(FUZZER_EXECUTION_TIME) \
	    -print_final_stats \
	    -max_len=1024
	llvm-profdata merge \
	    -sparse $(FUZZER_EXE).profraw \
	    -o $(FUZZER_EXE).profdata
	llvm-cov show ./$(FUZZER_EXE) \
	    -instr-profile=$(FUZZER_EXE).profdata
