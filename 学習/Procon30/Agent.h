#pragma once
#include <stdexcept>
#include <math.h>
#include "Vector2.h"

class Agent {

public:
	Vector2		pos;			// エージェントの位置
	Vector2		delta_pos;		// 次のターンの移動先 相対
	int			act_type;		// 行動の種類 0:動かない 1:移動 2:タイル除去
	int			team;			// チーム. 1 or 2
	int			ID;
	bool		done_butting;	// 前のターンにバッティングしたかどうか
	bool		done_bad_act;	// 手が有効だったか
	Agent();
	Agent(Vector2 _pos, int _team, int _ID);

	void resetAct();	// エージェントの行動をリセット
};