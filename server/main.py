from game import Game
from tcp_server import TcpServer
import threading
import time

def game_loop(game):
    print(game.get_board_json())

def gui_connection_loop(game):
    server = TcpServer(2525)

    server.listen()

    while (True):
        # クライアントから何をするかの要求を受け取る
        # "INFO"    情報要求
        # "ACT"     行動送信
        # "NEW"
        # "UPDATE"
        cmd = server.recv()
        if (cmd == "INFO"):
            server.send(game.get_board_json())

    server.close()

if __name__ == '__main__':
    game = Game()
    game_thread = threading.Thread(target=game_loop, args=(game,))
    server_thread = threading.Thread(target=gui_connection_loop, args=(game,))

    game_thread.start()
    server_thread.start()

    game_thread.join()
    server_thread.join()