#include "GUI_input.h"

void GUI_input::clicked_agent(std::vector<RAgent>& ragents, std::vector<RTile>& tiles, std::vector<Agent>& agents) {
	if (MouseL.down()) {
		if (!is_clicked_selector) {
			bool is_agent_clicked = false;
			int a_index = 0;
			for (RAgent a : ragents) {
				if (a.getCircle().leftClicked()) {
					is_agent_clicked = true;
					if (a_index == agent_index) {
						if (in_operation == NONE) {
							in_operation = MOVE;
							agents[a_index].resetAct();
						}
						else if (in_operation == MOVE) {
							in_operation = REMOVE;
						}
						else if (in_operation == REMOVE) {
							in_operation = NONE;
							break;
						}
					}
					else {
						in_operation = MOVE;
						agents[a_index].resetAct();
					}
					select_agent = a.getPos();
					agent_index = a.getAgentIndex();
					// 黄色い枠の生成
					int i = 0;
					for (int y = -1; y <= 1; y++) {
						for (int x = -1; x <= 1; x++) {
							if (x == 0 && y == 0) { continue; }
							int tile_index = (y + select_agent.y) * width + (x + select_agent.x);
							if (tile_index < 0 || tile_index >= width * height) {
								i++;
								continue;
							}
							selector[i].pos = tiles[tile_index].getRectPos();
							i++;
						}
					}

					// 端の処理
					i = 0;
					for (int y = 0; y < 3; y++) {
						for (int x = 0; x < 3; x++) {
							if (x == 1 && y == 1) { continue; }
							is_enable[i] = true;
							if (select_agent.x == 0 && x == 0) {
								is_enable[i] = false;
							}
							if (select_agent.y == 0 && y == 0) {
								is_enable[i] = false;
							}
							if (select_agent.x == width - 1 && x == 2) {
								is_enable[i] = false;
							}
							if (select_agent.y == height - 1 && y == 2) {
								is_enable[i] = false;
							}
							i++;
						}
					}
					break;
				}
				a_index++;
			}
			if (!is_agent_clicked) {
				in_operation = NONE;
			}
		}
	}
}

void GUI_input::clear()
{
	select_agent = Point();
	agent_index = 0;
	selector.clear();
	is_enable.clear();
	in_operation = NONE;
	is_clicked_selector = false;
	width = 0;
	height = 0;
}

void GUI_input::clicked_selector(std::vector<Agent>& agents) {
	if (in_operation != NONE) {
		if (MouseL.down()) {
			int i = 0;
			for (Rect r : selector) {
				if (is_enable[i]) {
					if (r.leftClicked()) {
						is_clicked_selector = true;
						// エージェントに移動先を設定する
						int tmp = i;
						if (i > 3) { tmp++; }
						Vector2 point = Vector2(tmp % 3 - 1, tmp / 3 - 1);
						agents[agent_index].setDeltaMove(point);
						if (in_operation == MOVE) {
							agents[agent_index].setActType(1);
						}
						if (in_operation == REMOVE) {
							agents[agent_index].setActType(2);
						}
						in_operation = NONE;
					}
				}
				i++;
			}
		}
	}
}

void GUI_input::init(RInfo& rInfo, BOARD_STATE& bState) {
	in_operation = NONE;
	select_agent = Point(0, 0);
	width = bState.width;
	height = bState.height;
	for (int i = 0; i < 8; i++) {
		selector.push_back(RectF(0, 0, rInfo.tile_side));
		is_enable.push_back(false);
	}
}

void GUI_input::update(std::vector<RAgent>& ragents, std::vector<RTile>& tiles, std::vector<Agent>& agents) {
	is_clicked_selector = false;
	clicked_selector(agents);
	clicked_agent(ragents, tiles, agents);
	drawSelector();
}

void GUI_input::drawSelector() {
	if (in_operation != NONE) {
		int i = 0;
		for (RectF r : selector) {
			if (is_enable[i]) {
				if (in_operation == MOVE) {
					r.draw(Color(255, 255, 0, 80));
					r.drawFrame(3, Palette::Yellow);
				}
				else {
					r.draw(Color(255, 0, 0, 80));
					r.drawFrame(3, Palette::Red);
				}
			}
			i++;
		}
	}
}

