#pragma once
#include "Rendering_INFO.h"
#include "RAgent.h"
#include "RTile.h"

class GUI_input {
private:
	Point				select_agent;
	int					agent_index;
	std::vector<RectF>	selector;
	std::vector<bool>	is_enable;
	OPERATION_STATE		in_operation;
	bool				is_clicked_selector;
	int					width, height;

	void clicked_agent(std::vector<RAgent>& ragents, std::vector<RTile>& tiles, std::vector<Agent>& agents);
	void clicked_selector(std::vector<Agent>& agents);
	void drawSelector();

public:
	void init(RInfo& rInfo, BOARD_STATE& bState);
	void update(std::vector<RAgent>& ragents, std::vector<RTile>& tiles, std::vector<Agent>& agents);
	
};

