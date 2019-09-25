ARGS = \
	$(CFLAGS) \
	-std=c++11 \
	*.cpp

all:
	g++ $(ARGS) $(OPT_SIZE) -o main-size
	g++ $(ARGS) $(OPT_SPEED) -o main-speed
	size main-size
	size main-speed
	time ./main-size encode $(ITERATIONS)
	time ./main-size decode $(ITERATIONS)
	time ./main-speed encode $(ITERATIONS)
	time ./main-speed decode $(ITERATIONS)

generate:
	flatc --cpp benchmark.fbs

include ../common.mk
