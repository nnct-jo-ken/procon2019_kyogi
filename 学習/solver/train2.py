import chainer
import chainer.functions as F
import chainer.links as L
import chainerrl
import gym
import numpy as np
import procon_env
import time

class QFunction(chainer.Chain):
    def __init__(self, obs_size, n_actions):
        super().__init__()
        with self.init_scope():
            self.c1 = L.Convolution2D(6, 16, ksize=3, stride=1,pad=1)
            self.c2 = L.Convolution2D(16, 16, ksize=3, stride=1, pad=1)
            self.c3 = L.Convolution2D(22, 32, ksize=3, stride=1, pad=1)
            self.c4 = L.Convolution2D(32, 32, ksize=3, stride=1, pad=1)
            self.b16 = L.BatchNormalization(16)
            self.b32 = L.BatchNormalization(32)
            self.l1 = L.Linear(32*10*10 + 1, 4096)
            self.l2 = L.Linear(4096, 4096)
            self.l3 = L.Linear(4096, 4096)
            # ここから個々のエージェント
            self.c7 = L.Convolution2D(4, 16, ksize=3, stride=1, pad=1)
            self.c8 = L.Convolution2D(16, 16, ksize=3, stride=1, pad=1)
            self.c9 = L.Convolution2D(16, 16, ksize=3, stride=1, pad=1)
            self.l4 = L.Linear(4096 + 1600, 4096)
            self.l5 = L.Linear(4096, 4096)
            self.l6 = L.Linear(4096, 4096)
            self.out = L.Linear(4096, n_actions)
            
    def __call__(self, x, test=False):
        h = F.relu(self.b16(self.c1(x[0])))
        h = F.relu(self.b16(self.c2(h)))
        h = F.concat([h, x[0]])
        h = F.relu(self.b32(self.c3(h)))
        h = F.max_pooling_2d(h, 2)
        h = F.relu(self.b32(self.c4(h)))
        h = F.reshape(h, (h.shape[0], 32 * 10 * 10))
        h = F.concat([h, x[2]])
        h = F.relu(self.l1(h))
        h = F.relu(self.l2(h))
        h = F.relu(self.l3(h))

        y = F.relu(self.b16(self.c7(x[1])))
        y = F.relu(self.b16(self.c8(y)))
        y = F.max_pooling_2d(y, 2)
        y = F.relu(self.b16(self.c9(y)))

        y = F.reshape(y, (y.shape[0], 16 * 10 * 10))
        y = F.concat([y, h])
        y = F.relu(self.l4(y))
        y = F.relu(self.l5(y))
        y = F.relu(self.l6(y))
        y = self.out(y)

        return chainerrl.action_value.DiscreteActionValue(y)

env = gym.make('procon_env-v0')
print("observation space:", env.observation_space)
print("action space:", env.action_space)

obs_size = env.observation_space.shape[0] * env.observation_space.shape[1] * env.observation_space.shape[2]
n_actions = env.action_space.n
q_func = QFunction(obs_size, n_actions)
q_func.to_gpu(0)
optimizer = chainer.optimizers.Adam(eps=0.008)
optimizer.setup(q_func)

n_episodes = 10000
explorer = chainerrl.explorers.LinearDecayEpsilonGreedy(
    start_epsilon = 0.7 ,end_epsilon = 0.1, decay_steps=n_episodes / 10, random_action_func=env.action_space.sample)

replay_buffer = chainerrl.replay_buffer.ReplayBuffer(capacity=10 ** 6)

gamma = 0.97

phi = lambda x: (x[0].astype(np.float32, copy=False), x[1].astype(np.float32, copy=False), np.array([x[2]], dtype=np.float32), x[3])

agent = []

agent.append(chainerrl.agents.DQN(
    q_func, optimizer, replay_buffer, gamma, explorer,
    replay_start_size=128, minibatch_size=64, update_interval=1,
    target_update_interval=50, phi=phi))

agent.append(chainerrl.agents.DQN(
    q_func, optimizer, replay_buffer, gamma, explorer,
    replay_start_size=128, minibatch_size=64, update_interval=1,
    target_update_interval=50, phi=phi))

for i in range(1, n_episodes + 1):
    obs = env.reset()
    reward = np.zeros((2, 2), dtype=int)
    done = False
    t = 0
    print("episode num : ", i)

    while not done:
        action = []
        for i in range(2):
            team_act = []
            for a in range(obs[3]):
                team_act.append(agent[i].act_and_train([obs[0][i], obs[1][i][a], obs[2], obs[3]], reward[i][0]))
            action.append(team_act)
        obs, reward, done, _ = env.step(action)
        t += 1

    if i % 10 == 0:
        for i in range(2):
            agent[i].stop_episode_and_train([obs[0][i], obs[1][i][a], obs[2], obs[3]], reward[i][1], done=True)
    if i % 50 == 0:
        agent[0].save("agent")
    
print('finishied')