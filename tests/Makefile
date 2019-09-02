FUZZER_CC ?= clang
FUZZER_EXE = main_fuzzer
FUZZER_C_SOURCES = \
	main_fuzzer.c \
	../src/pbtools.c \
	files/c_source/address_book.c
FUZZER_CFLAGS = \
	-fprofile-instr-generate \
	-fcoverage-mapping \
	-I../src \
	-g \
	-fsanitize=address,fuzzer \
	-fsanitize=signed-integer-overflow \
	-fno-sanitize-recover=all
FUZZER_EXECUTION_TIME ?= 30

CFLAGS ?= \
	-fprofile-arcs \
	-ftest-coverage \
	-fsanitize=address \
	-fsanitize=signed-integer-overflow

all:
	gcc \
	    -Wall \
	    -g \
	    -Og \
	    -std=gnu11  \
	    $(CFLAGS) \
	    -I../src \
	    narwhal.c \
	    ../src/pbtools.c \
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
	    main.c
	./a.out

fuzzer:
	$(FUZZER_CC) $(FUZZER_CFLAGS) $(FUZZER_C_SOURCES) -o $(FUZZER_EXE)
	rm -f $(FUZZER_EXE).profraw
	LLVM_PROFILE_FILE="$(FUZZER_EXE).profraw" \
	    ./$(FUZZER_EXE) \
	    -max_total_time=$(FUZZER_EXECUTION_TIME) \
	    -print_final_stats
	llvm-profdata merge \
	    -sparse $(FUZZER_EXE).profraw \
	    -o $(FUZZER_EXE).profdata
	llvm-cov show ./$(FUZZER_EXE) \
	    -instr-profile=$(FUZZER_EXE).profdata
