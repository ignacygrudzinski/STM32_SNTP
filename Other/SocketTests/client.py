import socket
import sys

from protocol import Protocol

if __name__ == '__main__':
    server_host = sys.argv[1] if len(sys.argv) > 1 else 'localhost'
    server_port = int(sys.argv[2]) if len(sys.argv) > 2 else 2137
    server_addr = (server_host, server_port)

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    protocol = Protocol(sock)
    while True:
        print('Press ENTER to simulate pressing blue button')
        input()
        protocol.send(b'hellothere', server_addr)
        response, _ = protocol.receive()
        print(f'Response {response} received')