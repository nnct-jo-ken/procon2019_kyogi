#include "MTCS.h"

Node::Node(BOARD_STATE& _board, Node* _parent, int& nodes_count, int team)
{
	this->parent = _parent;
	this->board = _board;
	this->n = 0;
	this->w = 0;
	this->team = team;
	nodes_count++;
}

double MTCS::calc_utc(MTCS& mtcs, Node* node)
{
	if (node->n == 0)
		{ return 10000000; }

	return node->w / (double)node->n + mtcs.c * std::sqrt(std::log((double)mtcs.N) / (double)node->n);
}

MTCS::MTCS(BOARD_STATE board, int team)
{
	mt = std::mt19937(rd());
	MTCS::N = 0;
	nodes_count = 0;
	int pre_team = team == 1 ? 2 : 1;
	root_node = new Node(board, nullptr, nodes_count, pre_team);
	root_node->n = VISIT_LIMIT;
}

void MTCS::search(vector<ACT_STATE>& act_list)
{
	//　再帰
	while (!visit_node(root_node));
	
	// wが最大の手を返す
	std::vector<double> w_list;
	for (Node* child : root_node->child) 
	{ 
		w_list.push_back(child->w); 
	}

	std::vector<double>::iterator max = std::max_element(w_list.begin(), w_list.end());
	size_t select_index = std::distance(w_list.begin(), max);

	act_list = root_node->child[select_index]->act_list;
}

bool MTCS::visit_node(Node* node)
{
	node->n++;
	MTCS::N++;
	if (nodes_count > NODES_LIMIT)
	{
		return true;
	}
	// 葉に到達した
	if (node->child.empty())
	{
		if (node->n >= MTCS::VISIT_LIMIT && node->board.turn >= 0)	// nが閾値以上
		{
			// ノードを展開
			expension_node(node);
		}
		else
		{		// ロールアウト
			// この辺でディープ
			GAME_SCORE scores = node->board.game_score;
			if (node->team == 1)
			{
				node->w += (scores.area_team1 + scores.tile_team1) - (scores.area_team2 + scores.tile_team2);
			}
			else
			{
				node->w += (scores.area_team2 + scores.tile_team2) - (scores.area_team1 + scores.tile_team1);
			}
			// backpropagation
			backpropagation(node);
		}
	}
	else	// 葉ではない
	{	
		// 各ノードのutcを計算
		vector<double> utc_list;
		for (Node* c_node : node->child)
		{
			utc_list.push_back(MTCS::calc_utc(*this, c_node));
		}

		action(node, utc_list);
	}
	return false;
}

void MTCS::backpropagation(Node* node)
{
	// rootまでやる
	for (Node* target = node; target->parent != nullptr; target = target->parent)
	{
		target->parent->w += target->w / (double)target->parent->child.size();
	}
}

void MTCS::expension_node(Node* node)
{
	vector<vector<ACT_STATE>> act = valid_act(node->board, node->team == 1 ? 2 : 1);
	// ランダムで手を選ぶ
	vector<std::string> act_list;
	for (int n = 0; n < EXPENSION_LIMIT; n++) {
		std::string act_key(node->board.agents_count, '0');
		for (int i = 0; i < node->board.agents_count; i++)
		{
			act_key[i] = (char)('a' + mt() % act[i].size());
		}
		act_list.push_back(act_key);
	}
	std::sort(act_list.begin(), act_list.end());
	act_list.erase(std::unique(act_list.begin(), act_list.end()), act_list.end());
	std::shuffle(act_list.begin(), act_list.end(), mt);

	// 木を拡張
	for (int n = 0; n < EXPENSION_LIMIT && n < act_list.size(); n++) {
		// 新規ノード作成
		int _team = node->team == 1 ? 2 : 1;
		Node* new_node = new Node(node->board, node, nodes_count, _team);

		int agent_start_i = _team == 1 ? 0 : node->board.agents_count;	// team == 1
		for (int i = 0; i < node->board.agents_count; i++)
		{
			new_node->board.agents[agent_start_i + i].act_type = act[i][(int)act_list[n][i] - (int)'a'].type; // ここやばい
			new_node->board.agents[agent_start_i + i].delta_pos= Vector2(act[i][(int)act_list[n][i] - (int)'a'].dx, act[i][(int)act_list[n][i] - (int)'a'].dy); // ここやばい
		}

		agent_start_i = node->team == 1 ? node->board.agents_count : 0;
		for (int i = agent_start_i; i < node->board.agents_count + agent_start_i; i++)
		{
			Vector2 d_pos = new_node->board.agents[i].delta_pos;
			int type = new_node->board.agents[i].act_type;
			new_node->act_list.push_back(ACT_STATE{ 0, d_pos.x, d_pos.y, type });
		}
		
		Game::updateTurn(new_node->board);
		if (node->team == root_node->team)
		{
			new_node->board.turn++;
		}
		node->child.push_back(new_node);
	}
}

void MTCS::action(Node* node, vector<double>& utc_list)
{
	// 最大値のイテレータ取得
	std::vector<double>::iterator max = std::max_element(utc_list.begin(), utc_list.end());
	// 最大値のインデックス取得
	size_t select_index = std::distance(utc_list.begin(), max);
	
	// 選択
	visit_node(node->child[select_index]);
}

vector<vector<ACT_STATE>> MTCS::valid_act(BOARD_STATE& board, int team)
{
	int start_i = team == 1 ? 0 : board.agents_count;
	
	vector<vector<ACT_STATE>> act_list;
	for (int i = 0; i < board.agents_count; i++)
	{
		vector<ACT_STATE> tmp;
		Vector2 pos = board.agents[i + start_i].pos;
		// 8方向探索
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				if (x == 0 && y == 0) { continue; }
				if (pos.x + x < 0 || pos.x + x >= board.width 
					|| pos.y + y < 0 || pos.y + y >= board.height) 
				{ continue; }

				int color = board.tile_color[(pos.y + y) * board.width + (pos.x + x)];	// 移動先のタイルの色
				if (color == team || color == 0)	// 移動
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

void MTCS::clean()
{
	delete_ptr(root_node);
}

void MTCS::delete_ptr(Node* node)
{
	if (node->child.empty())
	{
		node->parent = nullptr;
		delete node;
		return;
	}
	else
	{
		for (Node* target : node->child)
		{
			delete_ptr(target);
		}
	}
}