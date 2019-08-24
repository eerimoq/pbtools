test:
	$(MAKE) test-python
	$(MAKE) test-c

test-python:
	python3 setup.py test

test-c:
	make -C examples/address_book
	make -C examples/benchmarks/c_source
	make -C tests

release-to-pypi:
	python setup.py sdist
	python setup.py bdist_wheel --universal
	twine upload dist/*
