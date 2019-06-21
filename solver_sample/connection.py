import socket
# 自作モジュール
from board import Board

def get_info(sock):
    sock.sendall(b'INFO')
    buf = sock.recv(4096)
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