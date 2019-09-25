all:
	$(MAKE) -C nanopb
	$(MAKE) -C pbtools

graphs:
	termgraph termgraph/encoding-time-speed.dat
	termgraph termgraph/decoding-time-speed.dat
	termgraph termgraph/executable-size-text-speed.dat
