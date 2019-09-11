import socket
import random

# 自作モジュール
import connection
from makeJson import make_json
from actinfo import ActInfo

class Solver():
    def __init__(self, port):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect(("127.0.0.1", port))
        self.pre_turn = -1
        
    def run(self):
        # 受信
        board = connection.get_info(self.sock)

        # ターンが0以下になったら新規ゲームを要求する
        if board.turn <= 0:
            connection.send_new(self.sock)
            return

        if board.turn != self.pre_turn:
            self.pre_turn = board.turn
            # 送信用バッファ作成
            act_list = self.make_random(board)
            # ActInfo[] から json(文字列)へ変換
            strs = make_json(act_list)
            # 送信
            connection.send_act(self.sock, strs)

    # エージェントをランダムに移動させる行動リストを作る
    # AIの中心部
    def make_random(self, board):
        act_list = []
        print(board.agents_list[0])
        for i in board.agents_list[0]:
            act = ActInfo(
                i.agent_id,
                random.randint(-1, 1),
                random.randint(-1, 1),
                random.choice(["move", "remove"])
            )
            act_list.append(act)
        return act_list

    # デストラクタ
    def __del__(self):
        self.sock.close()