<<<<<<< HEAD
import numpy as np

import sys
sys.path.append('../')
from board import Board

# 行列を20x20に直す
def resizeArray(array):
    r = np.zeros((20, 20), dtype=int)
    for y in range(array.shape[0]):
        for x in range(array.shape[1]):
            r[y, x] = array[y, x]
    
    return r

# (タイルポイント、タイル状況、エージェント座標リスト)のタプルを返す
def convertArray(board):
    # 1次元リストを２次元のndarrayに変換
    points = np.array(board.tile_points)
    colors = np.array(board.tile_color)
    points = np.reshape(points, (board.height, board.width))
    colors = np.reshape(colors, (board.height, board.width))
    # エージェントの座標をndarrayに変換
    agents = [[] for i in range(2)]
    for (team, l) in zip(board.agents_list, agents):
        for e in team:
            l.append([e.x, e.y])
    agents = np.array(agents)
    
    my_agents = np.zeros((20, 20), dtype=int)
    enemy_agents = np.zeros((20, 20), dtype = int)

    for i, j in zip(agents[0], agents[1]):
        my_agents[i[1], i[0]] = 1
        enemy_agents[j[1], j[0]] = 1

    points = resizeArray(points)
    colors = resizeArray(colors)
    my_tiled = 1 * (colors == 1)
    enemy_tiled = 1 * (colors == 2)
=======
import numpy as np

import sys
sys.path.append('../')
from board import Board

# 行列を20x20に直す
def resizeArray(array):
    r = np.zeros((20, 20), dtype=int)
    for y in range(array.shape[0]):
        for x in range(array.shape[1]):
            r[y, x] = array[y, x]
    
    return r

# (タイルポイント、タイル状況、エージェント座標リスト)のタプルを返す
def convertArray(board):
    # 1次元リストを２次元のndarrayに変換
    points = np.array(board.tile_points)
    colors = np.array(board.tile_color)
    points = np.reshape(points, (board.height, board.width))
    colors = np.reshape(colors, (board.height, board.width))
    # エージェントの座標をndarrayに変換
    agents = [[] for i in range(2)]
    for (team, l) in zip(board.agents_list, agents):
        for e in team:
            l.append([e.x, e.y])
    agents = np.array(agents)
    
    my_agents = np.zeros((20, 20), dtype=int)
    enemy_agents = np.zeros((20, 20), dtype = int)

    for i, j in zip(agents[0], agents[1]):
        my_agents[i[1], i[0]] = 1
        enemy_agents[j[1], j[0]] = 1

    points = resizeArray(points)
    colors = resizeArray(colors)
    my_tiled = 1 * (colors == 1)
    enemy_tiled = 1 * (colors == 2)
>>>>>>> e440c8c44c9d7d8e1dcabe301dba4933b3b5ea28
    return (points, my_tiled, enemy_tiled, my_agents, enemy_agents)