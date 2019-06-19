from agent import Agent
import board_random
import random

class Game:
    def __init__(self):
        # 盤面を作成する
        x, y, board, my_agents_list, enemy_agents_list = board_random.create_board()
        self.width = x
        self.height = y
        self.tile_points = []   # タイルの得点（1次元配列）
        self.tile_color = []    # タイルの色 0:中立, 1:自陣, 2:敵陣 
        self.my_agents = []     # Agent型
        self.enemy_agents = []
        self.agents_count = len(my_agents_list) # 1チームのエージェントの数
        self.turn = random.randint(30, 60)

        for line in board:
            for p in line:
                self.tile_points.append(p)

        for i in range(0, self.width * self.height):    
            self.tile_color.append(0)

        # 自陣のエージェント作成
        for i, e in enumerate(my_agents_list):
            self.my_agents.append(Agent(i, e[0], e[1], 1))

        # 敵陣のエージェント作成
        for i, e in enumerate(enemy_agents_list):
            self.enemy_agents.append(Agent(self.agents_count + i, e[0], e[1], 2))

        # エージェントの位置を塗る
        for e1, e2 in zip(self.my_agents, self.enemy_agents):
            self.tile_color[e1.y * self.width + e1.x] = 1
            self.tile_color[e2.y * self.width + e2.x] = 2

        print(self.tile_color)