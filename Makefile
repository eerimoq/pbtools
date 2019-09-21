test:
	$(MAKE) test-python
	$(MAKE) test-c

test-python:
	python3 setup.py test

test-c:
	make -C tests

test-c-avr:
	avr-gcc -Wall -O2 -c pbtools/c_source/pbtools.c -o pbtools.avr.o
	avr-size pbtools.avr.o
	avr-objdump -d pbtools.avr.o

test-c-arm:
	arm-none-eabi-gcc -Wall -O2 -c pbtools/c_source/pbtools.c -o pbtools.arm.o
	arm-none-eabi-size pbtools.arm.o
	arm-none-eabi-objdump -d pbtools.arm.o

test-c-fuzzer:
	make -C tests fuzzer

release-to-pypi:
	python setup.py sdist
	python setup.py bdist_wheel --universal
	twine upload dist/*
