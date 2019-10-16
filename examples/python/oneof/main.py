import oneof


# Encode.
encoded = oneof.foo_encode({'oneof': ('fie', 789)})
print("Successfully encoded foo into {len(encoded)} bytes.")

# Decode.
decoded = oneof.foo_decode(encoded)
print("Successfully decoded {len(encoded)} bytes into Foo.")
print(decoded)
