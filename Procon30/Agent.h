#pragma once
#include <stdexcept>
#include <math.h>
#include "Vector2.h"

class Agent {
private:
	Vector2 pos;	// エージェントの位置
	Vector2 target_pos; // 次のターンの移動先
	int act_state;		// 行動の種類 0:動かない 1:移動 2:タイル除去
	int team;		// チーム. 1 or 2
public:
	Agent();
	Agent(Vector2 _pos, int _team);

	void setPos(Vector2 _pos);	// x, yともに整数
	void setTarget(Vector2 _step);
	void setActState(int _state);
	void setTeam(int _team);

	Vector2 getPos();
	Vector2 getTarget();
	int getActState();
	int getTeam();

	void resetAct();	// エージェントの行動をリセット
};