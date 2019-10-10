#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma once
#include <vector>
#include <iostream>
#include <math.h>
#include <fstream>
#include <iterator>
#include <mutex>
#include <queue>
#include <WinSock2.h>
#include "json11/json11.hpp"
#include "Agent.h"

// 得点を保持する構造体
struct GAME_SCORE {
	int		tile_team1;
	int		area_team1;

	int		tile_team2;
	int		area_team2;
}typedef GAME_SCORE;

// フィールドの情報を保持する構造体
struct BOARD_STATE {
	int					team_ID;		// 本番では変更する
	int					width;			// フィールドの横のマス数 10 ～ 20
	int					height;			// フィールドの縦のマス数 10 ～ 20
	int					turn;			// ターン数
	int					agents_count;	// 各チームのエージェントの数 2 ～ 8
	std::vector<int>	tile_points;		// 盤面の得点情報を記憶する配列　サイズは width * height
	std::vector<int>	tile_color;		// タイルの色 0 or 1 or 2
	std::vector<Agent>	agents;			// エージェントたち。1つの配列に無理やり2チーム詰め込んでる
	GAME_SCORE			game_score;
}typedef BOARD_STATE;

// 次取る行動を保存する
struct ACT_STATE {
	int agent_id;
	int dx;
	int dy;
	int type; // 0：停留、 1：移動、2：除去
}typedef ACT_STATE;

class Game {
private:
	void parse_json(std::string str_json);
	std::string getJsonFromServer();

public:
	BOARD_STATE board;

	void init();
	void load_queue(std::queue<ACT_STATE>& queue, std::mutex& mtx);

	static bool updateTurn(BOARD_STATE& _board);				// ターンを移す 試合が終わったらtrueを返す
	static GAME_SCORE countGameScore(BOARD_STATE& _board);	// 得点計算

	static int getAreaScore(int team, BOARD_STATE& _board);
	static void areaScoreRecursion(int team, int x, int y, std::vector<bool>& table, int& tmp, bool& reach_end, BOARD_STATE& _board);

};
