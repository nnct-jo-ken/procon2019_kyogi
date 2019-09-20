import socket
import numpy as np

import connection
from board import Board

class Connector():
    def __init__(self, port):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect(("127.0.0.1", port))

    def GET(self):
        return connection.get_info(self.sock)

    def SEND(self, act_json):
        connection.send_act(self.sock, act_json)

    def RESET(self):
        connection.send_new(self.sock)

    def STEP(self):
        connection.send_update(self.sock)

if __name__ == '__main__':
    c = Connector(7755)

    board = c.GET()
    board.print()