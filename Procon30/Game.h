#pragma once
#include <vector>
#include <iostream>
#include <math.h>
#include <fstream>
#include <iterator>
#include <mutex>
#include <queue>
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
	int					turn;			// 試合の全ターン数
	int					now_turn;		// 現在のターン数
	int					agent_count;	// 各チームのエージェントの数 2 ～ 8
	std::vector<int>	tile_score;		// 盤面の得点情報を記憶する配列　サイズは width * height
	std::vector<int>	tile_state;		// タイルの色 0 or 1 or 2
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
	BOARD_STATE board;
	int getAreaScore(int team);
	void areaScoreRecursion(int team, int x, int y, std::vector<bool>& table, int& tmp, bool& reach_end);
	void parse_json(std::string str_json);
	
public:
	void init();
	void clear();

	void setAct(int team, int num, int act, Vector2 delta_pos);
	void setTileState(Vector2 pos, int _team);
	void load_queue(std::queue<ACT_STATE>& queue, std::mutex& mtx);

	BOARD_STATE getBoardState();
	GAME_SCORE getGameScore();
	int getWidth();
	int getHeight();
	int getVectorSize();
	int getTileScore(int index);
	int getTileScore(Vector2 pos);
	int getTileState(int index);
	int getTileState(Vector2 pos);
	int getTurn();
	int getNowTurn();
	int getAgentQant();

	std::vector<Agent>& getAgentVector();

	bool updateTurn();				// ターンを移す 試合が終わったらtrueを返す
	GAME_SCORE countGameScore();	// 得点計算
};
