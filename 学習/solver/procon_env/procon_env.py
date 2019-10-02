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
        self.agents_count = 0
        self.tile_padding = np.zeros((20, 20), dtype=int)

    # 以下gymの実装する必要があるメソッド
    def _step(self, action):
        act_list = []
        for i in range(2):
            act = []
            for a in range(self.agents_count):
                #(type, x, y)
                tmp = 0
                if action[i][a] == 0:     # 停留
                    tmp = (0, 0, 0)
                elif action[i][a] == 1:   # 移動
                    tmp = (1, 0, -1)
                elif action[i][a] == 2:
                    tmp = (1, 1, -1)
                elif action[i][a] == 3:
                    tmp = (1, 1, 0)
                elif action[i][a] == 4:
                    tmp = (1, 1, 1)
                elif action[i][a] == 5:
                    tmp = (1, 0, 1)
                elif action[i][a] == 6:
                    tmp = (1, -1, 1)
                elif action[i][a] == 7:
                    tmp = (1, -1, 0)
                elif action[i][a] == 8:
                    tmp = (1, -1, -1)
                elif action[i][a] == 9:   #除去
                    tmp = (2, 0, -1)
                elif action[i][a] == 10:
                    tmp = (2, 1, -1)
                elif action[i][a] == 11:
                    tmp = (2, 1, 0)
                elif action[i][a] == 12:
                    tmp = (2, 1, 1)
                elif action[i][a] == 13:
                    tmp = (2, 0, 1)
                elif action[i][a] == 14:
                    tmp = (2, -1, 1)
                elif action[i][a] == 15:
                    tmp = (2, -1, 0)
                elif action[i][a] == 16:
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
        total_score = [board.score[0][0] + board.score[0][1], board.score[1][0] + board.score[1][1]]
        AREA_BIAS = 100
        score_tmp = np.array(board.score) - np.array(self.board[0].score)
        short_reward = (score_tmp[0, 0] + score_tmp[0, 1] * AREA_BIAS) - (score_tmp[1, 0] + score_tmp[0, 1] * AREA_BIAS)

        reward = [[0]for i in range(2)]
        for i in range(2):
            short_reward = short_reward if i == 0 else -short_reward
            final_reward = 1000 if total_score[0] > total_score[1] else -1000
            for a in range(self.agents_count):
                reward[i].append([
                    -100 if board.agents_list[i][a].done_bad_act else short_reward,
                    final_reward if i == 0 else -final_reward
                ])

        if self.board[0].turn <= 0:
            self.done = True
        else:
            self.done = False
        
        return self._observe(), reward, self.done, {}


    def _reset(self):
        self.c[0].RESET()
        for i in range(2):
            self.board[i] = self.c[i].GET()
        self.agents_count = self.board[0].agents_count
        self.tile_padding[:,:] = 0
        self.tile_padding[:self.board[0].height, :self.board[0].width] = 1
        return self._observe()


    def _render(self, mode='human', close=False):
        outfile = sys.stdout
        outfile.write("no")

    def _close(self):
        pass
    
    def _seed(self, seed=None):
        pass

    def _observe(self):
        # 観測環境を返す
        obs = [[] for i in range(2)]

        for i in range(2):
            self.board[i] = self.c[i].GET()
            util_data = util.convertArray(self.board[i])

            for a in self.board[i].agents_list[0]:
                my_pos = np.zeros((20, 20), dtype=int)
                my_pos[a.y, a.x] = 1
                a_obs = np.array([self.tile_padding,
                                  util_data[0],     # tile_points
                                  util_data[1],     # my_tiled
                                  util_data[2],     # enemy_tiled
                                  util_data[3],     # my_agents_pos
                                  util_data[4],     # enemy_agents_pos
                                  my_pos])

                obs[i].append(a_obs)

        return [self.board[0].turn, np.array(obs), self.board[0].agents_count]
        
        """return np.array([self.tile_padding,
                         self.tile_points, 
                         self.my_tiled, 
                         self.enemy_tiled, 
                         self.my_agents_pos, 
                         self.enemy_agents_pos,
                         self.my_pos])
                         
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
        self.tile_score = [self.board.my_tile_score, self.board.enemy_tile_score]"""