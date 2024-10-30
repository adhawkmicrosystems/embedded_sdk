import argparse
import socket

parser = argparse.ArgumentParser()
parser.add_argument("ip")
parser.add_argument("port")
args = parser.parse_args()

MESSAGE = bytes([0x00, 0x00, 0x85])  # Autotune

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.settimeout(5)
sock.sendto(MESSAGE, (args.ip, int(args.port)))

try:
    data, server = sock.recvfrom(1024)
    print(data)
except socket.timeout:
    print('Request Timeout')
