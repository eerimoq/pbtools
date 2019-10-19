import oneof


# Encode.
decoded = {'oneof': ('fie', 789)}
print(decoded)
encoded = oneof.foo_encode(decoded)
print(f"Successfully encoded foo into {len(encoded)} bytes.")

# Decode.
decoded = oneof.foo_decode(encoded)
print(f"Successfully decoded {len(encoded)} bytes into Foo.")
print(decoded)
