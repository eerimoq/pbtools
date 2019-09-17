SHELL = /usr/bin/env bash

CFLAGS = \
	-Igenerated \
        -ffunction-sections \
        -fdata-sections \
	-Wl,--gc-sections

OPT_SIZE = -Os
OPT_SPEED = -O3

ITERATIONS = 2000000
