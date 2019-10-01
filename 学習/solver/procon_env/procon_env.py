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
    area_score = []                                # 領域得点  [0] = 自チーム, [1]敵チーム
    tile_score = []                                # タイル得点  [0] = 自チーム, [2]敵チーム
    board = []

    def __init__(self):
        super().__init__()

        self.c = [Connector(7755), Connector(7756)]

        # ８方向　移動、削除、停留
        self.action_space = gym.spaces.Discrete(17) 
        self.observation_space = gym.spaces.Box(        # ここ怪しい
            low = 0,
            high = 1,
            shape = (7, 20, 20)
        )
        self.reward_range = [-5000, 5000]
        self.board = [[0] for i in range(2)]

    # 以下gymの実装する必要があるメソッド
    def _step(self, action):
        act_list = []
        for i in range(2):
            act = []
            for a in range(self.agents_count):
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

                act.append(ActInfo(
                    self.board[0].agents_list[i][a].agent_id,
                    tmp[1],
                    tmp[2],
                    act_type
                ))
            
            act_list.append(act)

        for i in range(2):
            self.c[i].SEND(make_json(act_list[i]))

        self.c[0].STEP()

        board = self.c[0].GET()
        score_a = board.my_area_score + board.my_tile_score
        score_b = board.enemy_tile_score + board.enemy_tile_score
        AREA_BIAS = 100
        short_reward = ((board.my_area_score - self.area_score[0]) * AREA_BIAS + (board.my_tile_score - self.tile_score[0])) - ((board.enemy_area_score  - self.area_score[1]) * AREA_BIAS + (board.enemy_tile_score - self.tile_score[1]))
        reward = [[0]for i in range(2)]
        for i in range(2):
            short_reward = short_reward if i == 0 else -short_reward
            final_reward = 1000 if score_a > score_b else -1000
            for a in range(self.board.agents_count):
                reward[i].append(
                    -100 if board.agents_list[i][a].done_bad_act else short_reward,
                    final_reward if i == 0 else -final_reward
                )

        if self.board[0].turn <= 0:
            self.done = True
        else:
            self.done = False
        
        observation = self._observe()

        return [self.board[0].turn, observation], reward, self.done, {}


    def _reset(self):
        self.c[0].RESET()
        for i in range(2):
            self.board[i] = self.c[i].GET()
        self.agents_count = self.board.agents_count
        return [self.board[0].turn, self._observe()]


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
        for i in range(2):
            self.board[i] = self.c[i].GET()

            util_data = util.convertArray(self.board[i])
            
            for a in range(self.board[0].agents_count):
                
                

        tmp = util.convertArray(self.board)
        self.tile_points = tmp[0]
        self.my_tiled = tmp[1]
        self.enemy_tiled = tmp[2]
        self.my_agents_pos = tmp[3]
        self.enemy_agents_pos = tmp[4]
        self.my_pos = np.zeros_like(self.my_pos)
        a = self.board.agents_list[0][self.my_index]
        self.my_pos[a.y][a.x] = 1
        self.area_score = [self.board.my_area_score, self.board.enemy_area_score]
        self.tile_score = [self.board.my_tile_score, self.board.enemy_tile_score]