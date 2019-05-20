#pragma once
#include "Rendering_INFO.h"
#include "RAgent.h"
#include "RTile.h"
#include "GUI_input.h"

class Renderer {
private:
	BOARD_STATE render_board;
	RInfo rInfo;
	void initrInfo();
	std::vector<RAgent> ragents;
	std::vector<RTile> tiles;
	GUI_input input;
	Font font;
public:
	void init(BOARD_STATE board);
	void update(std::vector<Agent>& agents);
	void updateTurn(BOARD_STATE board);
};

