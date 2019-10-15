import hello_world

# Encode.
foo = hello_world.Foo()
foo.bar = 78
encoded = foo.to_bytes()

# Decode.
foo = hello_world.Foo()
foo.from_bytes(encoded)
