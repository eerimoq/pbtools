import binascii
import int32_pb2

message = int32_pb2.Message()

print(message)
print(binascii.hexlify(message.SerializeToString()))
