import json
import hello_world


# Encode.
decoded = {'bar': 78}
print(json.dumps(decoded, indent=4))
encoded = hello_world.foo_encode(decoded)
print(f'Successfully encoded Foo into {len(encoded)} bytes.')

# Decode.
decoded = hello_world.foo_decode(encoded)
print(f'Successfully decoded {len(encoded)} bytes.')
print(json.dumps(decoded, indent=4))
