#pragma once
#include <stdexcept>
#include <math.h>
#include "Vector2.h"

class Agent {

private:
	Vector2		pos;	// エージェントの位置
	Vector2		target_pos; // 次のターンの移動先
	int			act_type;		// 行動の種類 0:動かない 1:移動 2:タイル除去
	int			team;		// チーム. 1 or 2
	int			ID;

public:
	Agent();
	Agent(Vector2 _pos, int _team, int _ID);

	void setPos(Vector2 _pos);	// x, yともに整数
	void setDeltaMove(Vector2 _step); // x, y ともに-1 ~ 1の間で指定
	void setActType(int _state);
	void setTeam(int _team);

	Vector2 getPos();
	Vector2 getTarget(); // 絶対座標を返す
	int getActType();
	int getTeam();
	int getID();

	void resetAct();	// エージェントの行動をリセット
};