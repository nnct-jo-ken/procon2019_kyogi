import socket

class TcpServer:
    def __init__(self, port):
        self.host = "127.0.0.1"
        self.port = port

        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.bind((self.host, self.port))

    def listen(self):
        self.sock.listen(1)
        self.client, self.client_address = self.sock.accept()

    def recv(self):
        return self.client.recv(8192)

    def send(self, strs):
        self.client.send(strs.encode('utf-8'))
    
    def close(self):
        self.client.close()