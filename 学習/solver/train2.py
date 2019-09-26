import chainer
import chainer.functions as F
import chainer.links as L
import chainerrl
import gym
import numpy as np
import procon_env
import time

class QFunction(chainer.Chain):
    def __init__(self, obs_size, n_actions, n_hidden_channels=50):
        super().__init__()
        with self.init_scope():
            self.l1 = L.Convolution2D(7, 32, ksize=3, stride=1,pad=1)
            self.l2 = L.Convolution2D(32, 64, ksize=3, stride=1, pad=1)
            self.l3 = L.Convolution2D(64, 64, ksize=3, stride=1, pad=1)
            self.l4 = L.Linear(64*20*20, 1024)
            self.out = L.Linear(1024, 17)
    
    def __call__(self, x, test=False):
        h = F.relu(self.l1(x))
        h = F.relu(self.l2(h))
        h = F.relu(self.l3(h))
        h = F.relu(self.l4(h))
        h = self.out(h)

        return chainerrl.action_value.DiscreteActionValue(h)

env = gym.make('procon_env-v0')
print("observation space:", env.observation_space)
print("action space:", env.action_space)

obs_size = env.observation_space.shape[0] * env.observation_space.shape[1] * env.observation_space.shape[2]
n_actions = env.action_space.n
q_func = QFunction(obs_size, n_actions)
q_func.to_gpu(0)
optimizer = chainer.optimizers.Adam(eps=1e-2)
optimizer.setup(q_func)

explorer = chainerrl.explorers.ConstantEpsilonGreedy(
    epsilon = 0.3, random_action_func=env.action_space.sample)

replay_buffer = chainerrl.replay_buffer.ReplayBuffer(capacity=10 ** 6)

gamma = 0.97

phi = lambda x: x.astype(np.float32, copy=False)
agent = chainerrl.agents.DQN(
    q_func, optimizer, replay_buffer, gamma, explorer,
    replay_start_size=100, update_interval=1,
    target_update_interval=50, phi=phi)

log_path = "./loglog.log"

n_episodes = 1000
count = 0
for i in range(n_episodes):
    obs = env.reset()
    reward = 0
    done = False
    R = 0
    t = 0
    while not done:
        count += 1
        print(count)
        print(i)
        action = agent.act_and_train(obs, reward)
        obs, reward, done, _ = env.step(action)
        R += reward
        t += 1

    agent.stop_episode_and_train(obs, reward, done)
    #agent.save("agent")
    
print('finishied')