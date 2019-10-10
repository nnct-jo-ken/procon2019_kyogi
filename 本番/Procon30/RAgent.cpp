#include "RAgent.h"

Vec2 RAgent::getUIPos(const RInfo& _rInfo)
{
	return Vec2(_rInfo.margin_left + (_rInfo.tile_side * pos.x) + (_rInfo.tile_side / 2),
		_rInfo.margin_top + (_rInfo.tile_side * pos.y) + (_rInfo.tile_side / 2))
		+ _rInfo.field_origin;
}

Vec2 RAgent::getUIPos(const RInfo& _rInfo, Point _pos)
{
	return Vec2(_rInfo.margin_left + (_rInfo.tile_side * _pos.x) + (_rInfo.tile_side / 2),
		_rInfo.margin_top + (_rInfo.tile_side * _pos.y) + (_rInfo.tile_side / 2))
		+ _rInfo.field_origin;
}

RAgent::RAgent(int _team, int _id, int _agent_index, Point _pos, const RInfo& _rInfo) {
	if (_team != 1 && _team != 2) {
		throw std::invalid_argument("RAgent constructor exception.");
	}
	if (_pos.x < 0 || _pos.y < 0) {
		throw std::invalid_argument("RAgent constructor exception.");
	}
	
	if (_team == 1) {
		surface_color = _rInfo.team1_color;
	}
	else {
		surface_color = _rInfo.team2_color;
	}
	agent_id = _id;
	pos = _pos;
	UI_pos = getUIPos(_rInfo);
	circle = Circle(UI_pos, _rInfo.agent_radius);
	agent_index = _agent_index;
	ostate = NONE;
	target_pos = pos;
}

void RAgent::update(Point _pos, const RInfo& _rInfo) {
	if (_pos.x < 0 || _pos.y < 0) {
		throw std::invalid_argument("RAgent constructor exception.");
	}

	pos = _pos;
	UI_pos = getUIPos(_rInfo);
	circle = Circle(UI_pos, _rInfo.agent_radius);
}

void RAgent::draw(const RInfo& _rInfo) {
	circle.draw(surface_color);
	circle.drawFrame(2, _rInfo.line_color);
	_rInfo.id_font(agent_id).draw(Arg::center = UI_pos, Palette::Black);
}

void RAgent::drawArrow(const RInfo& _rInfo) {
	Color arrow_color;
	if (ostate == MOVE) {
		arrow_color = Palette::Yellow;
	}
	else if (ostate == REMOVE) {
		arrow_color = Palette::Red;
	}

	if (ostate != NONE) {
		Vec2 target_UI_pos = getUIPos(_rInfo, target_pos);
		Line(UI_pos, target_UI_pos).drawArrow(2, Vec2(7, 7), arrow_color);
	}
}

const Circle& RAgent::getCircle() {
	return circle;
}

Vec2 RAgent::getUI_pos() {
	return UI_pos;
}

Point RAgent::getPos() {
	return pos;
}

void RAgent::update_targetPos(Point _target_pos, OPERATION_STATE _ostate)
{
	target_pos = _target_pos;
	ostate = _ostate;
}

int RAgent::getAgentIndex() {
	return agent_index;
}