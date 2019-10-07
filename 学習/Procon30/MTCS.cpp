#include "MTCS.h"


MTCS::MTCS(BOARD_STATE board)
{
	mt = std::mt19937(rd());
	MTCS::N = 0;
	root_node = std::make_unique<Node>();
	root_node->n = 0;
	root_node->w = 0;
	root_node->board = board;
	root_node->parent = nullptr;
}

void MTCS::search()
{
	
}

void MTCS::visit_node(mtcs_node& node)
{
	node.n++;
}

void MTCS::expension_node(mtcs_node& node, int team)
{
	vector<vector<ACT_STATE>>act = valid_act(node.board, team);
	// ランダムで手を選ぶ
	vector<int> act_list;
	for (int n = 0; n < EXPENSION_LIMIT * 2; n++) {
		int act_digit = 0;
		for (int i = 0; i < node.board.agents_count; i++)
		{
			int len = act[i].size();
			act_digit *= 10;
			act_digit += mt() % len;
		}
		act_list.push_back(act_digit);
	}
	std::sort(act_list.begin(), act_list.end());
	act_list.erase(std::unique(act_list.begin(), act_list.end()), act_list.end());

	// 木を拡張
	for (int n = 0; n < EXPENSION_LIMIT && n < act_list.size(); n++) {
		int act_num = act_list[n];
		for (int i = 0; i < node.board.agents_count; i++)
		{
			act[i][act_num % 10];
			act_num /= 10;
		}
	}
}

vector<vector<ACT_STATE>> MTCS::valid_act(BOARD_STATE& board, int team)
{
	int start_i;
	if (team == 1) 
	{ start_i = 0; }
	else 
	{ start_i = board.agents_count; }

	vector<vector<ACT_STATE>> act_list;
	for (int i = start_i; i < start_i + board.agents_count; i++)
	{
		vector<ACT_STATE> tmp;
		Vector2 pos = board.agents[i].pos;
		// 8方向探索
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (x == 0 && y == 0) { continue; }
				if (pos.x + x < 0 || pos.x + x >= board.width 
					|| pos.y + y < 0 || pos.y + y >= board.height) 
				{ continue; }

				int color = board.tile_color[(pos.y + y) * board.width + (pos.x + x)];	// 移動先のタイルの色
				if (color == 0 || color == team)	// 移動
				{
					tmp.push_back(ACT_STATE{ 0, x, y, 1 });
				}
				// 除去
				if (color != 0 && color != team)
				{
					tmp.push_back(ACT_STATE{ 0, x, y, 2 });
				}
				if (color == team)
				{
					if (board.tile_points[(pos.y + y) * board.width + (pos.x + x)] < 0)
					{
						tmp.push_back(ACT_STATE{ 0, x, y, 2 });
					}
				}
			}
		}
		act_list.push_back(tmp);
	}

	return act_list;
}
