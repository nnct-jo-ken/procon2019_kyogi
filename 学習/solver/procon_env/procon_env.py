import gym
import numpy as np
import gym.spaces
import procon_env.utility as util
import socket
import time

import sys
sys.path.append('../')
from board import Board
from connection import Connector
from actinfo import ActInfo
from makeJson import make_json

class ProconEnv(gym.Env):

    tile_points = np.zeros((20, 20), dtype=int)
    my_tiled = np.zeros((20, 20), dtype=int)
    enemy_tiled = np.zeros((20, 20), dtype=int)
    tile_padding = np.zeros((20, 20), dtype=int)   # フィールドのサイズを表す 
    my_agents_pos = np.zeros((20, 20), dtype=int)
    enemy_agents_pos = np.zeros((20, 20), dtype=int)
    my_pos = np.zeros((20, 20), dtype=int)
    turns = 0                                      # ターン数　ステップ数でもある
    area_score = []                                # 領域得点  [0] = 自チーム, [1]敵チーム
    tile_score = []                                # タイル得点  [0] = 自チーム, [2]敵チーム
    my_index = 0                                      # 仮ID
    my_id = 0

    def __init__(self):
        super().__init__()

        self.c = Connector(7755)

        # ８方向　移動、削除、停留
        self.action_space = gym.spaces.Discrete(17) 
        self.observation_space = gym.spaces.Box(        # ここ怪しい
            low = 0,
            high = 1,
            shape = (7, 20, 20)
        )
        self.reward_range = [-5000, 5000]

    # 以下gymの実装する必要があるメソッド
    def _step(self, action):
        #(type, x, y)
        tmp = 0
        if action == 0:     # 停留
            tmp = (0, 0, 0)
        elif action == 1:   # 移動
            tmp = (1, 0, -1)
        elif action == 2:
            tmp = (1, 1, -1)
        elif action == 3:
            tmp = (1, 1, 0)
        elif action == 4:
            tmp = (1, 1, 1)
        elif action == 5:
            tmp = (1, 0, 1)
        elif action == 6:
            tmp = (1, -1, 1)
        elif action == 7:
            tmp = (1, -1, 0)
        elif action == 8:
            tmp = (1, -1, -1)
        elif action == 9:   #除去
            tmp = (2, 0, -1)
        elif action == 10:
            tmp = (2, 1, -1)
        elif action == 11:
            tmp = (2, 1, 0)
        elif action == 12:
            tmp = (2, 1, 1)
        elif action == 13:
            tmp = (2, 0, 1)
        elif action == 14:
            tmp = (2, -1, 1)
        elif action == 15:
            tmp = (2, -1, 0)
        elif action == 16:
            tmp = (2, -1, -1)
                
        # 行動させる
        if tmp[0] == 1:
            act_type = "move"
        else:
            act_type = "remove"

        act = ActInfo(
            self.my_id,
            tmp[1],
            tmp[2],
            act_type
        )

        self.c.SEND(make_json([act]))
        self.c.STEP()

        board = self.c.GET()
        score_a = board.my_area_score + board.my_tile_score
        score_b = board.enemy_tile_score + board.enemy_tile_score
        AREA_BIAS = 100
        short_reward = ((board.my_area_score - self.area_score[0]) * AREA_BIAS + (board.my_tile_score - self.tile_score[0])) - ((board.enemy_area_score  - self.area_score[1]) * AREA_BIAS + (board.enemy_tile_score - self.tile_score[1]))
        reward = [
            -100 if board.agents_list[0][self.my_index].done_bad_act else short_reward,
            1000 if score_a > score_b else -1000
            ]
        self._update_observe()

        if self.turns <= 0:
            self.done = True
        else:
            self.done = False
        
        observation = self._observe()

        return [self.turns, observation], reward, self.done, {}


    def _reset(self):
        self.c.RESET()
        board = self.c.GET()
        self.my_id = board.agents_list[0][self.my_index].agent_id
        self._update_observe()
        return [board.turn, self._observe()]


    def _render(self, mode='human', close=False):
        outfile = sys.stdout
        outfile.write(
            self.area_score[0] + ' ' +
            self.area_score[1] + ' ' +
            self.tile_score[0] + ' ' +
            self.tile_score[1] + ' ')

    def _close(self):
        pass
    
    def _seed(self, seed=None):
        pass

    def _observe(self):
        # 観測環境を返す
        return np.array([self.tile_padding,
                         self.tile_points, 
                         self.my_tiled, 
                         self.enemy_tiled, 
                         self.my_agents_pos, 
                         self.enemy_agents_pos,
                         self.my_pos])
        
    def _update_observe(self):
        board = self.c.GET()
        tmp = util.convertArray(board)
        self.tile_points = tmp[0]
        self.my_tiled = tmp[1]
        self.enemy_tiled = tmp[2]
        self.my_agents_pos = tmp[3]
        self.enemy_agents_pos = tmp[4]
        self.my_pos = np.zeros_like(self.my_pos)
        a = board.agents_list[0][self.my_index]
        self.my_pos[a.y][a.x] = 1
        self.turns = board.turn
        self.area_score = [board.my_area_score, board.enemy_area_score]
        self.tile_score = [board.my_tile_score, board.enemy_tile_score]