import json
import numpy as np
# 自作モジュール
from agent import Agent

class Board:
    def __init__(self, json_strs):
        json_dic = json.loads(json_strs)

        teams = json_dic['teams']
        self.width = json_dic['width']
        self.height = json_dic['height']
        self.turn = json_dic['turn']                    # 残りターン数
        self.agents_count = json_dic['agentCount']      # 1チームありのエージェント数

        self.tile_points = np.asarray(json_dic['points'])
        self.tile_color = np.asarray(json_dic['tiled'])
        
        self.score = np.array([[teams[0]['tilePoint'], teams[0]['areaPoint']],
                               [teams[1]['tilePoint'], teams[1]['areaPoint']]])

        self.agents_list = [[] for i in range(2)]      # agents_list[0]が自チーム, agents_list[1]が敵チーム

        self.agents_pos = np.zeros((2, self.agents_count, 2), dtype=int)

        for i, l in enumerate(self.agents_list):
            for j, e in enumerate(teams[i]['agents']):
                l.append(Agent(e['agentID'], e['x'], e['y'], i + 1, e['butting'], e['badAct']))
                self.agents_pos[i][j] = np.array([e['x'], e['y']])

        self.agents_pos = np.array([self.agents_pos[0].T, self.agents_pos[1].T])

    # デバッグ用　盤面の情報を表示する
    def print(self):
        print("幅:%d 高さ:%d" % (self.width, self.height))
        print("残り%dターン\n各チームのエージェントの数%d体" % (self.turn, self.agents_count))
        print("味方:タイルポイント%d点 エリアポイント%d点" % (self.score[0][0], self.score[0][1]))
        print("敵:タイルポイント%d点 エリアポイント%d点" % (self.score[1][0], self.score[1][1]))
        print("\nタイルの点数", end="")
        for i, p in enumerate(self.tile_points):
            if i % self.width == 0:
                print("")
            print("%3d " % p, end="")
        
        print("\n\nタイルの状態", end="")
        for i, c in enumerate(self.tile_color):
            if i % self.width == 0:
                print("")
            print("%3d " % c, end="")

        print("\n\n味方のエージェント")
        for a in self.agents_list[0]:
            print("id=%2d x=%2d y=%2d" % (a.agent_id, a.x, a.y))

        print("\n敵のエージェント")
        for a in self.agents_list[1]:
            print("id=%2d x=%2d y=%2d" % (a.agent_id, a.x, a.y))