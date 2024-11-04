import argparse
import socket

parser = argparse.ArgumentParser()
parser.add_argument("ip")
parser.add_argument("port")
args = parser.parse_args()

MESSAGE = bytes([0x00, 0x00, 0x85])  # Autotune

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((args.ip, int(args.port)))
sock.send(MESSAGE)

while True:
    try:
        data = sock.recv(1024)
        print(data)
        if data[2] == 0x85:
            break;
    except socket.timeout:
        print('Request Timeout')
