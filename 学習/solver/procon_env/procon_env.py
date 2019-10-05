import gym
import numpy as np
import gym.spaces
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

        # ８方向　移動、削除
        self.action_space = gym.spaces.Discrete(16) 
        self.observation_space = gym.spaces.Box(        # ここ怪しい
            low = 0,
            high = 1,
            shape = (2, 6, 20, 20)
        )
        self.board = [0, 0]
        self.agents_count = 0
        self.tile_padding = np.zeros((20, 20), dtype=int)
        self.but_act_count = 0
        self.act_count = 0
        self.but_act_rate = [0, 0]

    # 以下gymの実装する必要があるメソッド
    def _step(self, action):
        #print(action.shape)
        act_list = []
        for i in range(2):

            act = []
            for a in range(self.agents_count):
                #(type, x, y)
                tmp = 0
                #停留はしない
                if action[i][a] == 0:   # 移動
                    tmp = (1, 0, -1)
                elif action[i][a] == 1:
                    tmp = (1, 1, -1)
                elif action[i][a] == 2:
                    tmp = (1, 1, 0)
                elif action[i][a] == 3:
                    tmp = (1, 1, 1)
                elif action[i][a] == 4:
                    tmp = (1, 0, 1)
                elif action[i][a] == 5:
                    tmp = (1, -1, 1)
                elif action[i][a] == 6:
                    tmp = (1, -1, 0)
                elif action[i][a] == 7:
                    tmp = (1, -1, -1)
                elif action[i][a] == 8:   #除去
                    tmp = (2, 0, -1)
                elif action[i][a] == 9:
                    tmp = (2, 1, -1)
                elif action[i][a] == 10:
                    tmp = (2, 1, 0)
                elif action[i][a] == 11:
                    tmp = (2, 1, 1)
                elif action[i][a] == 12:
                    tmp = (2, 0, 1)
                elif action[i][a] == 13:
                    tmp = (2, -1, 1)
                elif action[i][a] == 14:
                    tmp = (2, -1, 0)
                elif action[i][a] == 15:
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

        reward = np.zeros((2, 2), dtype=int)
        for i in range(2):
            short_reward = short_reward if i == 0 else -short_reward
            final_reward = 1000 if total_score[0] > total_score[1] else -1000
            for a in range(self.agents_count):
                # エージェントの行動率調査
                self.act_count += 1
                if board.agents_list[i][a].done_bad_act:
                    self.but_act_count += 1

                reward[i] = np.array([-100 if board.agents_list[i][a].done_bad_act else short_reward,
                                      final_reward if i == 0 else -final_reward])

        if self.board[0].turn <= 0:
            self.done = True
        else:
            self.done = False
        
        return self._observe(), reward, self.done, {}


    def _reset(self):
        # 学習率調査
        if self.act_count != 0:
            self.but_act_rate.append(self.but_act_count / self.act_count)
            if len(self.but_act_rate) % 100 == 0:
                with open('BA_rate.txt', 'w') as f:
                    f.write(str(self.but_act_rate))
        self.c[0].RESET()
        for i in range(2):
            self.board[i] = self.c[i].GET()
        self.agents_count = self.board[0].agents_count
        self.tile_padding = np.zeros((20, 20), dtype=int)
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
        common_obs = []
        personal_obs = []
        for i in range(2):
            self.board[i] = self.c[i].GET()
            pos_tmp = np.zeros((2, 20, 20), dtype=int)
            # エージェントの座標を行列に
            for i in range(2):
                pos_tmp[i][self.board[i].agents_pos[i][1], self.board[i].agents_pos[i][0]] = 1

            width, height = self.board[i].width, self.board[i].height
            # 得点
            points = np.pad(self.board[i].tile_points, [(0, 20 - height), (0, 20 - width)], 'constant')
            # 色
            colors = np.pad(self.board[i].tile_color, [(0, 20 - height), (0, 20 - width)], 'constant')
            util_data = [
                points,
                1 * (colors == 1),
                2 * (colors == 2),
                pos_tmp[0],
                pos_tmp[1]
            ]

            team_obs = np.array([self.tile_padding,
                        util_data[0] / 16,     # tile_points
                        util_data[1],     # my_tiled
                        util_data[2],     # enemy_tiled
                        util_data[3],     # my_agents_pos
                        util_data[4],     # enemy_agents_pos
                        ])
            team_personal_obs = []
            for a in self.board[0].agents_list[0]:
                my_pos = np.zeros((20, 20), dtype=int)
                my_pos[a.y, a.x] = 1
                rule = np.zeros((20, 20), dtype=int)
                rule[a.y - 1 if a.y - 1 > 0 else 0 : a.y + 2 if a.y + 2 < 20 else 20, a.x - 1 if a.x - 1 > 0 else 0  : a.x + 2 if a.x + 2  < 20 else 20] = 1
                rule[a.y, a.x] = 0
                rule_move = 1 * rule * ((colors == 0) + (colors == 1))
                rule_delete = 1 * rule * ((colors == 1) + (colors == 2))
                team_personal_obs.append([my_pos, rule, rule_move, rule_delete])

            common_obs.append(team_obs)
            personal_obs.append(team_personal_obs)
        return [np.array(common_obs), np.array(personal_obs), self.board[0].turn, self.board[0].agents_count]