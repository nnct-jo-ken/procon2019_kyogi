#パッケージのインポート
import numpy as np
import random
import json
import socket
import sys
import datetime

def make_board():
	#盤面のパターン決定
	BOARD_MODE = random.randrange(1,3)											 # 1番:上下左右対称 2:左右のみ対称
	BOARD_X = random.randrange(10,21)											  #盤の横のマス数(10～20の間のランダムな整数値)
	BOARD_Y = random.randrange(10,21)											  #盤の縦のマス数(10～20の間のランダムな整数値)
	BOARD_SX = BOARD_X // 2 + BOARD_X % 2
	BOARD_SY = BOARD_Y // 2 + BOARD_Y % 2

	#盤面の得点リスト生成
	BOARD = np.array([-1])
	while BOARD.sum() <= 0:
		if BOARD_MODE == 1: #上下左右対称の時
			BOARD = np.random.randint(-16, 17, [BOARD_SY, BOARD_SX])			   #1/4作成
			BOARD = np.concatenate([BOARD, np.flip(BOARD, axis = 1)], axis = 1)	#1/2作成
			if BOARD_X % 2 == 1:
				BOARD = np.delete(BOARD, BOARD_X // 2, axis = 1)				   #重複削除
			BOARD = np.concatenate([BOARD, np.flip(BOARD, axis = 0)], axis = 0)	#盤面作成
			if BOARD_Y % 2 == 1:
					BOARD = np.delete(BOARD, BOARD_Y // 2, axis = 0)			   #重複削除
		
		else: #左右のみ対称の時
			BOARD = np.random.randint(-16, 17, [BOARD_Y, BOARD_SX])				#1/2作成
			BOARD = np.concatenate([BOARD, np.flip(BOARD, axis = 1)], axis = 1)	#盤面作成
			if BOARD_X % 2 == 1:
				BOARD = np.delete(BOARD, BOARD_X // 2, axis = 1)				   #重複削除

	#エージェントの座標リスト生成
	if BOARD_MODE == 1: #上下左右対称の時
		SMALL_AGENTS = random.randrange(1,3)									   #基エージェントの数

		POS_X = np.random.randint(0, BOARD_SX, [100, 1])						   #1/4x座標サンプル作成
		POS_Y = np.random.randint(0, BOARD_SY, [100, 1])						   #1/4y座標サンプル作成
		POS_SAMPLE = np.concatenate([POS_X, POS_Y], axis = 1)					  #1/4座標サンプル作成
		POS_SAMPLE = np.unique(POS_SAMPLE, axis = 0)							   #サンプルの重複削除
		POS_SAMPLE = np.random.permutation(POS_SAMPLE)							 #サンプルのシャッフル

		MY_Q = np.full((BOARD_SY, BOARD_SX), 0)									#1/4自座標(初期化)
		ENEMY_Q = np.full((BOARD_SY, BOARD_SX), 0)								 #1/4敵座標(初期化)
		MY_AGENTS = POS_SAMPLE[0:SMALL_AGENTS]									 #1/4自座標リスト作成
		ENEMY_AGENTS = POS_SAMPLE[SMALL_AGENTS:2 * SMALL_AGENTS]				   #1/4敵座標リスト作成
		for i, j in zip(MY_AGENTS, ENEMY_AGENTS):
			MY_Q[i[1], i[0]] = 1											 #1/4自座標完成
			ENEMY_Q[j[1], j[0]] = 1										  #1/4敵座標完成

		MY_H1 = np.concatenate([MY_Q, np.flip(ENEMY_Q, axis = 1)], axis = 1)
		ENEMY_H1 = np.concatenate([ENEMY_Q, np.flip(MY_Q, axis = 1)], axis = 1)
		if BOARD_X % 2 == 1:
			MY_H1 = np.delete(MY_H1, BOARD_X // 2, axis = 1)
			ENEMY_H1 = np.delete(ENEMY_H1, BOARD_X // 2, axis = 1)

		MY = np.concatenate([MY_H1, np.flip(ENEMY_H1, axis = 0)], axis = 0)
		ENEMY = np.concatenate([ENEMY_H1, np.flip(MY_H1, axis = 0)], axis = 0)  

		if BOARD_Y % 2 == 1:
			MY = np.delete(MY, BOARD_Y // 2, axis = 0)
			ENEMY = np.delete(ENEMY, BOARD_Y // 2, axis = 0)

	else: #左右のみ対称の時
		SMALL_AGENTS = random.randrange(1,5)									   #基エージェントの数

		POS_X = np.random.randint(0, BOARD_X // 2, [100, 1])					   #1/2x座標リストサンプル作成
		POS_Y = np.random.randint(0, BOARD_Y, [100, 1])							#1/2y座標リストサンプル作成
		POS_SAMPLE = np.concatenate([POS_X, POS_Y], axis = 1)					  #1/2座標リストサンプル作成
		POS_SAMPLE = np.unique(POS_SAMPLE, axis = 0)							   #座標リストサンプルの重複削除
		POS_SAMPLE = np.random.permutation(POS_SAMPLE)							 #座標リストのシャッフル

		MY_H = np.full((BOARD_Y, BOARD_SX), 0)									 #1/2自座標(初期化)
		ENEMY_H = np.full((BOARD_Y, BOARD_SX), 0)								  #1/2敵座標(初期化)
		MY_AGENTS = POS_SAMPLE[0:SMALL_AGENTS]									 #1/4自座標リスト作成
		ENEMY_AGENTS = POS_SAMPLE[SMALL_AGENTS:2 * SMALL_AGENTS]				   #1/4敵座標リスト作成
		for i, j in zip(MY_AGENTS, ENEMY_AGENTS):
			MY_H[i[1], i[0]] = 1
			ENEMY_H[j[1], j[0]] = 1
		MY = np.concatenate([MY_H, np.flip(ENEMY_H, axis = 1)], axis = 1)
		ENEMY = np.concatenate([ENEMY_H, np.flip(MY_H, axis = 1)], axis = 1)
		if BOARD_X % 2 == 1:
			MY = np.delete(MY, BOARD_X // 2, axis = 1)
			ENEMY = np.delete(ENEMY, BOARD_X // 2, axis = 1)

	for i in range(MY.shape[1]):
		for j in range(MY.shape[0]):
			if MY[j, i] == 1:
				MY_AGENTS = np.append(MY_AGENTS, [[i, j]], axis = 0)
			if ENEMY[j, i] == 1:
				ENEMY_AGENTS = np.append(ENEMY_AGENTS, [[i, j]], axis = 0)

	MY_AGENTS = np.unique(MY_AGENTS, axis = 0)
	ENEMY_AGENTS = np.unique(ENEMY_AGENTS, axis = 0)
	
	return BOARD, MY_AGENTS, ENEMY_AGENTS, MY, ENEMY


if __name__=='__main__':

	# 重複回避
	while True:
		BOARD, MY_AGENTS, ENEMY_AGENTS, MY, ENEMY = make_board()
		# 重複確認
		test = np.vstack((MY_AGENTS, ENEMY_AGENTS))
		b_len = len(test)
		test = np.unique(test, axis=0)
		a_len = len(test)

		if len(MY_AGENTS) != len(ENEMY_AGENTS):
			continue

		if b_len == a_len:
			break

	tiled_array = MY + ENEMY * 2
	MY_AGENTS += 1
	ENEMY_AGENTS += 1

	log_dump = '-------------------------------------------------------------\n'
	log_dump += str(datetime.datetime.now())
	log_dump += '\n'
	log_dump += str(BOARD)
	log_dump += '\n\n'
	log_dump += str(MY)
	log_dump += '\n\n'
	log_dump += str(ENEMY)
	log_dump += '\n\n'
	log_dump += str(MY_AGENTS)
	log_dump += '\n\n'
	log_dump += str(ENEMY_AGENTS)
	log_dump += '\n\n'


	'''

		f.write('--------------------------------------------------------------\n')
		f.write(str(datetime.datetime.now()))
		f.write('\n')
		f.write(str(BOARD))
		f.write('\n\n')
		f.write(str(MY))
		f.write('\n\n')
		f.write(str(ENEMY))
		f.write('\n\n')
		f.write(str(MY_AGENTS))
		f.write('\n\n')
		f.write(str(ENEMY_AGENTS))'''
		
	BOARD = BOARD.tolist()
	tiled_array = tiled_array.tolist()
	MY_AGENTS = MY_AGENTS.tolist()
	ENEMY_AGENTS = ENEMY_AGENTS.tolist()
	# print(BOARD)
	my_dict = []
	enemy_dict = []
	i = 1
	for e in MY_AGENTS:
		my_dict.append(
			{
				"agentID" : i,
				"x" : e[0],
				"y" : e[1]
			}
		)
		i += 1
	
	for e in ENEMY_AGENTS:
		enemy_dict.append(
			{
				"agentID" : i,
				"x" : e[0],
				"y" : e[1]
			}
		)
		i += 1

	teams = [
		{
			"teamID" : 1,
			"agents" : my_dict,
			"tilePoint" : 0,
			"areaPoint" : 0
		},
		{
			"teamID" : 2,
			"agents" : enemy_dict,
			"tilePoint" : 0,
			"areaPoint" : 0
		}
	]

	json_dict = {
		"width" : len(BOARD[0]), 
		"height" : len(BOARD),
		"points" : BOARD,
		"allTurn" : random.randint(30, 60),
		"startedAtUnixTime": 0,
		"turn": 0,
		"tiled": tiled_array,
		"teams" : teams
	}

	json_str = json.dumps(json_dict)
	"""
	with open(log_file_path, mode='a') as f:
		f.write('\n\n')
		f.write(json_str)
	"""
	log_dump += json_str

	log_file_path = 'py_server.log'
	# log出力
	try:
		with open(log_file_path, mode='x') as f:
			f.write('')
	except FileExistsError:
		pass

	with open(log_file_path, mode='a') as f:
		f.write(log_dump)

	with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
		s.bind(('127.0.0.1', 5678))
		s.listen(1)

		while True:
			connect, addr = s.accept()
			with connect:
				while True:
					connect.sendall(json_str.encode('utf-8'))
					sys.exit()