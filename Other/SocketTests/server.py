import socket
import sys

from protocol import DisconnectionError, Protocol


def respond(message: bytes) -> bytes:
    return b'pong'


def handle_client(data: bytes, addr: bytes):
    protocol = Protocol(sock)
    while True:
        try:
            received_message = protocol.receive()
        except DisconnectionError:
            print("Client disconnected")
            break

        print('Received message')
        to_send = Protocol.pad(respond(received_message))
        protocol.send(to_send, addr)    


if __name__ == "__main__":
    server_addr = sys.argv[1] if len(sys.argv) > 2 else "127.0.0.1"
    server_port = int(sys.argv[2]) if len(sys.argv) > 1 else 2137

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((server_addr, server_port))
    protocol = Protocol(sock)
    while True:
        data, addr = protocol.receive()
        print(f'Client {addr} sent {data}')
        protocol.send(b'generalkenobi\0', addr)

        