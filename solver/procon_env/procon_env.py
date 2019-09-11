import gym
import numpy as np
import gym.spaces

class ProconEnv(gym.Env):

    # 以下gymの実装する必要があるメソッド
    def _step(self, action):
        pass

    def _reset(self):
        pass

    def _render(self, mode='human', close=False):
        pass

    def _close(self):
        pass
    
    def _seed(self, seed=None):
        pass