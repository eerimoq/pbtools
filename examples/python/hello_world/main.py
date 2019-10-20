import hello_world


# Encode.
encoded = hello_world.foo_encode({'bar': 78})
print(f'Successfully encoded Foo into {len(encoded)} bytes.')

# Decode.
decoded = hello_world.foo_decode(encoded)
print(f'Successfully decoded {len(encoded)} bytes.')
