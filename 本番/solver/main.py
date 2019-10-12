# 自作モジュール
import requests
import json
import time
import socket
import subprocess
import threading
from datetime import datetime
from board import Board

TOKEN_STR = 'procon30_example_token'    # 仮トークン
HOST = '127.0.0.1'  # 仮
PORT = '8001'      # 仮

class game_info:
    def __init__(self, match):
        self.id  = match['id']
        self.all_turns = match['turns']
        self.team_id = match['teamID']
        self.intervall = match['intervalMillis'] / 1000
        self.turn_time = match['turnMillis'] / 10000
        self.get_request = 'http://' + HOST + ':' + PORT + '/matches/' + str(self.id)
        self.last_move_time = 0

    def debug_print(self):
        print('game id is : ', self.id, type(self.id))
        print('all turns is : ', self.all_turns, type(self.all_turns))
        print('team id is : ', self.team_id, type(self.team_id))
        print('intervall is : ', self.intervall, type(self.intervall))
        print('http requests is : ', self.get_request, type(self.get_request))

def ready_geme(token):
    while (True):   
        # 試合前情報取得
        match = requests.get('http://' + HOST + ':' + PORT + '/matches', headers=token)
        print(match)
        if (match.status_code == 200):
            print(match.text)
            return json.loads(match.text)

        time.sleep(1)

def turn_update(i, matches, token, post_header):
    print(datetime.now().timestamp())
    while ((datetime.now().timestamp() - matches[i].last_move_time) / matches[i].turn_time < 1):
        print("ealy")
        time.sleep(1)
    board_info = requests.get(matches[i].get_request, headers=token)
    print(board_info)
    print(board_info.text)

    while (board_info.status_code != 200):
        time.sleep(1)
        board_info = requests.get(matches[i].get_request, headers=token)
        print(board_info)
        print("match id : ", matches[i].id, board_info.status_code)
        print(board_info.text)

    print(board_info.text)

    # ソルバーへ送信
    clientsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    clientsock.connect(("127.0.0.1", 7755 + i))
    clientsock.send(board_info.text.encode())
        
    # ここでソルバーが計算

    # ソルバーから行動結果を受信
    act_json = ""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.bind(('127.0.0.1', 5577 + i))
        sock.listen(1)

        conn, addr = sock.accept()
        with conn:
            act_json = conn.recv(4096)

    if not act_json:
        print("ソルバーからjsonを受け取れませんでした")

    print("----------------act json------------------------")
    print(act_json.decode('utf-8').strip('\0'))
    result = requests.post(matches[i].get_request + "/action", act_json, headers=post_header)
    matches[i].last_move_time = datetime.now().timestamp()
    print(result)


if __name__ == '__main__':
    token = {'Authorization' : TOKEN_STR}
    post_header = {"Content-Type" : "application/json", 'Authorization' : TOKEN_STR}
    match = ready_geme(token)
    print("マッチ取得完了")
    matches = []
    for e in match:
        matches.append(game_info(e))

    INDEX = [1] # 0 ~ 2まで

    """
    ここで　チームidに合わせて　最大3つのソルバープロセスを起動する
    ポート番号は 5577 + index(0 ~ 2)
    """

    thread_list = []
    for i in INDEX:
        #subprocess.Popen("../Procon30/App/Procon30(64-bit).exe")
        my_info = json.dumps({"all_turns":matches[i].all_turns, "real_team_id":matches[i].team_id, "team_index":i})
        time.sleep(1)
        clientsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        clientsock.connect(("127.0.0.1", 7777))
        time.sleep(1)
        print(type(my_info))
        print(my_info)
        clientsock.send(my_info.encode())
        print("初期情報送信完了")
        thread_list.append(threading.Thread(target=turn_update, args=([i, matches, token, post_header])))

    while(True):
        turn_update(1, matches, token,post_header)
    #for e in thread_list:
        #e.start()

    #for e in thread_list:
        #e.join()