import sys
import socket
import random
import time
from board import Board
from actinfo import ActInfo
from makeJson import make_json
import connection
import threading

# エージェントをランダムに移動させる行動リストを作る
def make_random(board):
    act_list = []
    for i in board.agents_list[0]:
        act = ActInfo(
            i.agent_id,
            random.randint(-1, 1),
            random.randint(-1, 1),
            random.choice(["move", "remove"])
        )
        act_list.append(act)
    return act_list

def solver(port):
    pre_turn = -1
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        # 接続 7755 or 7756
        s.connect(('127.0.0.1', port))

        while True:
            # 受信
            board = connection.get_info(s)

            # 新規ゲーム
            if board.turn <= 0:
                connection.send_new(s)
                continue

            if board.turn != pre_turn:
                pre_turn = board.turn
                # 送信用バッファ作成
                act_list = make_random(board)
                # ActInfo[] から json(文字列)へ変換
                strs = make_json(act_list)

                # 送信
                connection.send_act(s, strs)


if __name__ == "__main__":
    '''
    スレッドを使用してソルバーを実行しています
    そのため、Ctrl + C を押しても止まりません
    ごめんなさい
    サーバーを終了するとエラーとともに停止します
    そのうちCtrl + C で止められるようにします
    '''
    # スレッド作成
    # ポート7755(味方)、又は7756(敵)で接続
    my_solver = threading.Thread(target=solver, args=(7755,))
    enemy_solver = threading.Thread(target=solver, args=(7756,))

    # スレッドスタート
    my_solver.start()
    enemy_solver.start()

    # スレッドが終了するまで待つ
    my_solver.join()
    enemy_solver.join()