all:
	cd generated && python3 setup.py build
	cp generated/build/lib.*/*.so .
	python3 main.py

clean:
	rm -rf *.so generated/build
