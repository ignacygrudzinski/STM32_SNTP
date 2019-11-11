import socket
import sys

from protocol import Protocol

if __name__ == '__main__':
    host = sys.argv[1] if len(sys.argv) > 1 else 'localhost'
    port = int(sys.argv[2]) if len(sys.argv) > 2 else 2137

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((host, port))
    protocol = Protocol(sock)
    while True:
        print('Press ENTER to simulate pressing blue button')
        input()
        protocol.send(protocol.pad(b'ping'))
        response = protocol.receive()
        print('Response received')