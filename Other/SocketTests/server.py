import socket
import sys

from protocol import DisconnectionError, Protocol


def respond(message: bytes) -> bytes:
    return b'pong'


def handle_client(sock: socket.socket):
    protocol = Protocol(sock)
    while True:
        try:
            received_message = protocol.receive()
        except DisconnectionError:
            print("Client disconnected")
            break

        print('Received message')
        to_send = Protocol.pad(respond(received_message))
        protocol.send(to_send)    


if __name__ == "__main__":
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 2137

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind((socket.gethostname(), port))
    sock.listen(1)
    while True:
        (client_socket, addr) = sock.accept()
        print(f'Client {addr} connected')
        handle_client(client_socket) # TODO: multiple clients support

        