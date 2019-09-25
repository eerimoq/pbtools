all:
	$(MAKE) -C nanopb
	$(MAKE) -C pbtools

graphs:
	echo "Encoding time:"
	termgraph termgraph/encoding-time-speed.dat | sort
	echo "Decoding time:"
	termgraph termgraph/decoding-time-speed.dat | sort
	echo "Executable size:"
	termgraph termgraph/executable-size-text-speed.dat | sort
