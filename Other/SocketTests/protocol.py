from socket import socket

class DisconnectionError(Exception):
    pass


class Protocol:
    MESSAGE_SIZE = 100

    def __init__(self, socket: socket):
        self.socket = socket

    @classmethod
    def pad(cls, message: bytes) -> bytes:
        padding_length = cls.MESSAGE_SIZE - len(message)
        padding = b' ' * padding_length
        return message + padding

    def send(self, message: bytes):
        total_sent = 0
        while total_sent < self.MESSAGE_SIZE:
            sent = self.socket.send(message[total_sent:])
            if sent == 0:
                raise DisconnectionError
            total_sent += sent

    def receive(self) -> bytes:
        chunks = []
        total_received = 0
        while total_received < self.MESSAGE_SIZE:
            chunk = self.socket.recv(self.MESSAGE_SIZE - total_received)
            if chunk == b'':
                raise DisconnectionError
            chunks.append(chunk)
            total_received += len(chunk)
        return b''.join(chunks)