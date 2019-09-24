import numpy as np
from multiagent.core import World, Agent, Landmark
from multiagent.scenario import BaseScenario

from board import Board
from learn_test import Connector

class Scenario(BaseScenario):

    def __init__(self, connector):
        self.board = connector.GET
        self.connector = connector

    def make_world(self):
        world = World()

        # world setup
        world.dim_c = 4
        world.collaborative = True
        num_agents = self.board.agents_count
        world.num_agents = num_agents * 2
        num_adversaries = num_agents
        

        # add agents
        world.agents = [Agent() for i in range(num_agents * 2)]
        for i, agent in enumerate(world.agents):
            agent.name = str(self.board.agents_list[i / num_agents][i % num_agents])
            agent.collide = False
            agent.silent = True
            agent.adversary = True if i < num_adversaries else False
            agent.size = 1

        world.landmarks = [[0 for i in range(20)] for j in range(20)]
        for i, line in enumerate(world.landmarks):
            for j, landmark in enumerate(line):
                landmark.name = 'x = ' + j + ' : y =' + i
                landmark.collide = False
                landmark.movable = False
                landmark.size = 1

        self.reset_world
        return world
     
    def reset_world(self, world):
        self.connector.RESET()
        self.board = self.connector.GET()
        for i in range(0, world.num_agents / 2):
            world.agents[i].color = np.array([0.23, 0.9, 0.9])
        for i in range(world.num_agents / 2, world.num_agents):
            world.agents[i].color = np.array([1.0, 0.5, 0.14])

        for y, line in enumerate(world.landmarks):
            for x, landmark in enumerate(line):
                landmark.color = np.array([0, 0, 0])

        goal = np.random.choice(world.landmark.reshape(-1))
        for agent in world.agents:
            agent.goal_a = goal

        for i, agent in enumerate(world.agents):
            agent.state.p_pos = np.random.uniform(-1, +1, world.dim_p)
            agent.state.p_vel = np.zeros(world.dim_p)
            agent.state.c = np.zeros(world.dim_c)
        
        for line in world.landmarks:
            for i, landmark in enumerate(line):
                landmark.state.p_pos = np.random.uniform(-1, +1, world.dim_p)
                landmark.state.p_vel = np.zeros(world.dim_p)

    def reward(self, agent, world):
        main_reward = self.board.my_tile_score + self.board.my_area_score
        return main_reward

    def observation(self, agent, world):
        pass
