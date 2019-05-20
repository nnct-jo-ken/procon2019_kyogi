#pragma once
#include "Rendering_INFO.h"

class RAgent {
private:
	Color color;
	int id;
	int agent_index;
	Point pos;
	OPERATION_STATE ostate;
	Point target_pos;
	Vec2 UI_pos;
	Circle circle;

	Vec2 getUIPos(const RInfo& _rInfo);
	Vec2 getUIPos(const RInfo& _rInfo, Point pos);
public:
	RAgent(int _team, int _id, int _agent_index, Point _pos, const RInfo& _rInfo);
	void update(Point _pos, const RInfo& _rInfo);
	void draw(const RInfo& _rInfo);
	void drawArrow(const RInfo& _rInfo);
	const Circle& getCircle();
	Vec2 getUI_pos();
	Point getPos();
	void update_targetPos(Point _target_pos, OPERATION_STATE _ostate);
	int getAgentIndex();
};