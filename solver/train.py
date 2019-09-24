<<<<<<< HEAD
import numpy as np
import gym
import copy
import time
import random
from gym import wrappers
import procon_env
import torch
import torch.nn as nn
import torch.nn.functional as F
import torchvision.transforms as T
import torch.optim as optim
from torch.autograd import Variable


device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
# https://pytorch.org/tutorials/intermediate/reinforcement_q_learning.html
Transition = namedtuple('Transition', ('state', 'action', 'next_state', 'reward'))

class ReplayMemory(object):

    def __init__(self, capacity):
        self.capacity = capacity
        self.memory = []
        self.position = 0

    def push(self, *args):
        if len(self.memory) < self.capacity:
            self.memory.append(None)
        self.memory[self.position] = Transition(*args)
        self.position = (self.position + 1) % self.capacity

    def sample(self, batch_size):
        return random.sample(self.memory, batch_size)

    def __len__(self):
        return len(self.memory)


class NN(nn.Module):
    def __init__(self):
        super(NN, self).__init__()
        self.block1 = nn.Sequential(
            nn.Conv2d(in_channels=6, out_channels=32, kernel_size=3, padding=1),
            nn.ReLU(inplace=True),
            nn.Conv2d(in_channels=32, out_channels=32, kernel_size=3, padding=1),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(kernel_size=2, stride=1),
            nn.BatchNorm2d(32)
        )

        self.block2 = nn.Sequential(
            nn.Conv2d(in_channels=32, out_channels=64, kernel_size=3, padding=1),
            nn.ReLU(inplace=True),
            nn.Conv2d(in_channels=64, out_channels=64, kernel_size=3, padding=1),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(kernel_size=2, stride=1),
            nn.BatchNorm2d(64)
        )

        self.full_connection = nn.Sequential(
            nn.Linear(in_features=64*18*18+1, out_features=1024),
            nn.ReLU(),
            nn.Dropout(),
            nn.Linear(in_features=1024, out_features=17, bias=False)
        )

    def forward(self, in_observ, in_turns):
        x = self.block1(in_observ)
        x = self.block2(x)
        x = x.view(x.size(0), 64 * 18 * 18)
        x = torch.cat([x, in_turns], dim=1)
        y = self.full_connection(x)
        return y

BATCH_SIZE = 128
GAMMA = 0.999
EPS_START = 0.9
EPS_END = 0.03
STEP_MAX = 60

env = gym.make('procon_env-v0')

# 確認用
print('observation space:', env.observation_space)
print('action space:', env.action_space)
obs_size = env.obsercation_space.shape[0] * env.observation_space.shape[1] * env.observation_space.shape[2]
print('obsercation size:', obs_size)
print('n actions:', env.action_space.n)
# 確認終わり

def optimize_model():
    if len(memory) < BATCH_SIZE:    # バッチサイズ分たまってるか見る
        return

    transitions = memory.sample(BATCH_SIZE)

    batch = Transitions(*zip(*transitions))

    non_final_mask = torch.tensor(tuple(map(lambda s: s is not None, batch.next_state)), device=device, dtype=torch.uint8)
    non_final_next_states = torch.cat([s for s in batch.next_state if s is not None])

    state_batch = torch.cat(batch.state)
    action_batch = torch.cat(batch.action)
    reward_batch = torch.cat(batch.reward)

    state_action_values = policy_net(state_batch).gather(1, action_batch)

    next_state_values = torch.zeros(BATCH_SIZE, device=device)
    next_state_values[non_final_mask] = target_net(non_final_next_states).max(1)[0].detach()

    expected_state_action_values = (next_state_values * GAMMA) + reward_batch

policy_net = NN().to(device) # ネットワーク
target_net = NN().to(device)
optimizer = optim.Adam(policy_net.parameters())

total_step = 0
memory = ReplayMemory(100000)
total_rewards = []

print("start train")
start = time.time()

NUM_EPISODES = 100  # エポック数　このゲームの場合試合数
for epoch in range(NUM_EPISODES):
    env.reset() # 環境初期化
    step = 0
    done = False
    total_reward = 0

    while not done:
        action = env.action_space.sample()
        _, reward, done, _ = env.step(action.item())

        rward = torch.tensor([reward], device=device)

        # Observe new state
        #ここでステップ移行

        memory.push(state, action, next_state, reward)
=======
import numpy as np
import gym
import copy
import time
import random
from gym import wrappers
import procon_env
import torch
import torch.nn as nn
import torch.nn.functional as F
import torchvision.transforms as T
import torch.optim as optim
from torch.autograd import Variable


device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
# https://pytorch.org/tutorials/intermediate/reinforcement_q_learning.html
Transition = namedtuple('Transition', ('state', 'action', 'next_state', 'reward'))

class ReplayMemory(object):

    def __init__(self, capacity):
        self.capacity = capacity
        self.memory = []
        self.position = 0

    def push(self, *args):
        if len(self.memory) < self.capacity:
            self.memory.append(None)
        self.memory[self.position] = Transition(*args)
        self.position = (self.position + 1) % self.capacity

    def sample(self, batch_size):
        return random.sample(self.memory, batch_size)

    def __len__(self):
        return len(self.memory)


class NN(nn.Module):
    def __init__(self):
        super(NN, self).__init__()
        self.block1 = nn.Sequential(
            nn.Conv2d(in_channels=6, out_channels=32, kernel_size=3, padding=1),
            nn.ReLU(inplace=True),
            nn.Conv2d(in_channels=32, out_channels=32, kernel_size=3, padding=1),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(kernel_size=2, stride=1),
            nn.BatchNorm2d(32)
        )

        self.block2 = nn.Sequential(
            nn.Conv2d(in_channels=32, out_channels=64, kernel_size=3, padding=1),
            nn.ReLU(inplace=True),
            nn.Conv2d(in_channels=64, out_channels=64, kernel_size=3, padding=1),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(kernel_size=2, stride=1),
            nn.BatchNorm2d(64)
        )

        self.full_connection = nn.Sequential(
            nn.Linear(in_features=64*18*18+1, out_features=1024),
            nn.ReLU(),
            nn.Dropout(),
            nn.Linear(in_features=1024, out_features=17, bias=False)
        )

    def forward(self, in_observ, in_turns):
        x = self.block1(in_observ)
        x = self.block2(x)
        x = x.view(x.size(0), 64 * 18 * 18)
        x = torch.cat([x, in_turns], dim=1)
        y = self.full_connection(x)
        return y

BATCH_SIZE = 128
GAMMA = 0.999
EPS_START = 0.9
EPS_END = 0.03
STEP_MAX = 60

env = gym.make('procon_env-v0')

# 確認用
print('observation space:', env.observation_space)
print('action space:', env.action_space)
obs_size = env.obsercation_space.shape[0] * env.observation_space.shape[1] * env.observation_space.shape[2]
print('obsercation size:', obs_size)
print('n actions:', env.action_space.n)
# 確認終わり

def optimize_model():
    if len(memory) < BATCH_SIZE:    # バッチサイズ分たまってるか見る
        return

    transitions = memory.sample(BATCH_SIZE)

    batch = Transitions(*zip(*transitions))

    non_final_mask = torch.tensor(tuple(map(lambda s: s is not None, batch.next_state)), device=device, dtype=torch.uint8)
    non_final_next_states = torch.cat([s for s in batch.next_state if s is not None])

    state_batch = torch.cat(batch.state)
    action_batch = torch.cat(batch.action)
    reward_batch = torch.cat(batch.reward)

    state_action_values = policy_net(state_batch).gather(1, action_batch)

    next_state_values = torch.zeros(BATCH_SIZE, device=device)
    next_state_values[non_final_mask] = target_net(non_final_next_states).max(1)[0].detach()

    expected_state_action_values = (next_state_values * GAMMA) + reward_batch

policy_net = NN().to(device) # ネットワーク
target_net = NN().to(device)
optimizer = optim.Adam(policy_net.parameters())

total_step = 0
memory = ReplayMemory(100000)
total_rewards = []

print("start train")
start = time.time()

NUM_EPISODES = 100  # エポック数　このゲームの場合試合数
for epoch in range(NUM_EPISODES):
    env.reset() # 環境初期化
    step = 0
    done = False
    total_reward = 0

    while not done:
        action = env.action_space.sample()
        _, reward, done, _ = env.step(action.item())

        rward = torch.tensor([reward], device=device)

        # Observe new state
        #ここでステップ移行

        memory.push(state, action, next_state, reward)
>>>>>>> e440c8c44c9d7d8e1dcabe301dba4933b3b5ea28
