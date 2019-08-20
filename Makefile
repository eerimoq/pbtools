test:
	python3 setup.py test

test-c:
	make -C examples/address_book
	examples/address_book/main

release-to-pypi:
	python setup.py sdist
	python setup.py bdist_wheel --universal
	twine upload dist/*
