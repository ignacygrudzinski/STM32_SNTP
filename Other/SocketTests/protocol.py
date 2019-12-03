from socket import socket

class DisconnectionError(Exception):
    pass


class Protocol:
    MESSAGE_SIZE = 32

    def __init__(self, socket: socket):
        self.socket = socket

    @classmethod
    def pad(cls, message: bytes) -> bytes:
        padding_length = cls.MESSAGE_SIZE - len(message)
        padding = b' ' * padding_length
        return message + padding

    def send(self, message: bytes, address):
        self.socket.sendto(self.pad(message), 0, address)
        

    def receive(self) -> (bytes, bytes):
        data, addr = self.socket.recvfrom(self.MESSAGE_SIZE)
        return data, addr