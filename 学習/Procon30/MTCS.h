#pragma once
#include <cmath>
#include <memory>
#include <vector>
#include <random>
#include "Game.h"

using Node = struct mtcs_node;
using std::unique_ptr;
using std::vector;

struct mtcs_node {
	BOARD_STATE board;
	unique_ptr<Node> parent;
	vector<unique_ptr<Node>> child;
	int n;
	int w;
};

class MTCS
{
public:
	static constexpr int VISIT_LIMIT = 1;
	static constexpr int EXPENSION_LIMIT = 100;
	static std::random_device rd;
	static std::mt19937 mt;

	unique_ptr<Node> root_node;
	int N;
	const double c = std::sqrt(2);

	MTCS(BOARD_STATE board);
	void search();
	void visit_node(mtcs_node& node);
	void expension_node(mtcs_node& node, int team);
	void action();

	vector<vector<ACT_STATE>> valid_act(BOARD_STATE& board, int team);

	int getTileState(int index, const BOARD_STATE& board) { return board.tile_color[index]; }
	int getTileState(Vector2 pos, const BOARD_STATE& board) { return board.tile_color[pos.y * board.width + pos.x]; }
	void setTileState(Vector2 pos, int _team, BOARD_STATE& board) {
		if (_team != 1 && _team != 2) {
			throw std::invalid_argument("Game::setTileState() exception.");
		}
		board.tile_color[pos.y * board.width + pos.x] = _team;
	}
};

