#pragma once
#include <cmath>
#include <memory>
#include <vector>
#include <random>
#include <algorithm>
#include "Game.h"

using std::unique_ptr;
using std::vector;

class Node {
public:
	BOARD_STATE board;
	Node* parent;
	vector<Node*> child;
	vector<ACT_STATE> act_list;
	int n;
	double w;
	int team;
	Node(BOARD_STATE& _board, Node* _parent, int& nodes_count, int team);
};

class MTCS
{
public:
	static constexpr int VISIT_LIMIT = 2;
	static constexpr int EXPENSION_LIMIT = 10000;
	static constexpr int NODES_LIMIT = 100000;
	const double c = std::sqrt(2);

	static double calc_utc(MTCS& mtcs, Node* node);
	
	int nodes_count;
	Node* root_node;
	int N;
	std::random_device rd;
	std::mt19937 mt;

	MTCS(BOARD_STATE board, int team);
	void search(vector<ACT_STATE>& act_list);
	bool visit_node(Node* node);
	void expension_node(Node* node);
	void action(Node* node, vector<double>& utc_list);
	void backpropagation(Node* node);
	vector<vector<ACT_STATE>> valid_act(BOARD_STATE& board, int team);
	void clean();
	void delete_ptr(Node* node);
};
