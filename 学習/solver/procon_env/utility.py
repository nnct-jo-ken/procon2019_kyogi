import numpy as np

import sys
sys.path.append('../')
from board import Board

# 行列を20x20に直す
def resizeArray(np_array):
    size = np_array.shape
    return np.pad(np_array, [(0, 20 - size[0]), (0, 20 - size[1])], 'constant')

# (タイルポイント、タイル状況、エージェント座標リスト)のタプルを返す
def convertArray(board):
    # エージェントの座標をndarrayに変換
    pos_tmp = np.zeros((2, 20, 20), dtype=int)
    for i in range(2):
        pos_tmp[i][board.agents_pos[i][1], board.agents_pos[i][0]] = 1

    points = resizeArray(board.tile_points)
    colors = resizeArray(board.tile_color)
    my_tiled = 1 * (colors == 1)
    enemy_tiled = 1 * (colors == 2)
    return (points, my_tiled, enemy_tiled, pos_tmp[0], pos_tmp[1])