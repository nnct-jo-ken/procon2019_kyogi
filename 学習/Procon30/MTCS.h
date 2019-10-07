#pragma once
#include <cmath>
#include <memory>
#include <vector>
#include "Game.h"

using Node = struct mtcs_node;

struct agents_pos {

};

class mtcs_node {
	std::vector<std::vector<int>> tile_points;
	std::vector<std::vector<int>> tiled;

	int n;
	int w;

	mtcs_node(BOARD_STATE board);
};

class MTCS
{
	std::unique_ptr<Node> top_node;
	int N;
	const double c = std::sqrt(2);

	MTCS(BOARD_STATE board);
	void search();
	void action();
};

