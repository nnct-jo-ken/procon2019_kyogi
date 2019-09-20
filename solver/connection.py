import socket
# 自作モジュール
from board import Board

def get_info(sock):
    sock.sendall(b'INFO')
    buf = sock.recv(8192)
    strs = buf.decode().replace('\0', '')
    return Board(strs)
    
def send_act(sock, act_json):
    sock.sendall(b'ACT')
    sock.recv(128)
    sock.sendall(act_json.encode())
    sock.recv(128)
    send_update(sock)

def send_new(sock):
    sock.sendall(b'NEW')
    sock.recv(128)

def send_update(sock):
    sock.sendall(b'UPDATE')
    sock.recv(128)

class Connector():
    def __init__(self, port):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect(("127.0.0.1", port))

    def GET(self):
        return get_info(self.sock)

    def SEND(self, act_json):
        send_act(self.sock, act_json)

    def RESET(self):
        send_new(self.sock)

    def STEP(self):
        send_update(self.sock)