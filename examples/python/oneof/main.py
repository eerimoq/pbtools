import oneof


# Encode.
foo = oneof.Foo()
foo.bar.choice = oneof.Foo.Bar.FIE
foo.bar.value.fie = 789
encoded = foo.encode()

print("Successfully encoded Foo into {len(encoded)} bytes.")

# Decode.
foo = oneof.Foo()
foo.decode(encoded)

print("Successfully decoded {len(encoded)} bytes into Foo.")

if foo.bar.choice == oneof.Foo.Bar.FIE:
    print("Foo.bar.fie: {foo.bar.value.fie}")
elif foo.bar.choice == oneof.Foo.Bar.FUM:
    print("Foo.bar.fum: {foo.bar.value.fum}")
else:
    print("Unknown choice {foo.bar.choice}.")
