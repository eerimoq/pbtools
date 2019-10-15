import hello_world

# Encode.
foo = hello_world.Foo()
foo.bar = 78
encoded = foo.encode()
print(f'Successfully encoded Foo into {len(encoded)} bytes.')

# Decode.
foo = hello_world.Foo()
foo.decode(encoded)
print(f'Successfully decoded {len(encoded)} bytes into Foo.')
print(f'Foo.bar: {foo.bar}')
