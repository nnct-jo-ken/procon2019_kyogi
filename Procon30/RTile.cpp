#include "RTile.h"

RTile::RTile(int _team, int _score, Point _pos, const RInfo& _rInfo) {
	if (_team != 0 && _team != 1 && _team != 2) {
		throw std::invalid_argument("RTile constructor exception.");
	}
	if (_pos.x < 0 || _pos.y < 0) {
		throw std::invalid_argument("RTile constructor exception.");
	}

	switch (_team)
	{
	case 1:
		color = _rInfo.team1_color;
		break;
	case 2:
		color = _rInfo.team2_color;
		break;
	default:
		color = _rInfo.tile_clolor;
		break;
	}

	score = _score;
	pos = _pos;
	UI_pos = getUIPos(_rInfo);
	rect = RectF(UI_pos, _rInfo.tile_side);
}

Vec2 RTile::getUIPos(const RInfo& _rInfo) {
	return Vec2(pos.x * _rInfo.tile_side + _rInfo.margin_left, pos.y * _rInfo.tile_side + _rInfo.margin_top) + _rInfo.field_origin;
}

void RTile::update(int _team, const RInfo& _rInfo) {
	if (_team != 0 && _team != 1 && _team != 2) {
		throw std::invalid_argument("RTile::update() exception,");
	}

	switch (_team)
	{
	case 1:
		color = _rInfo.team1_color;
		break;
	case 2:
		color = _rInfo.team2_color;
		break;
	default:
		color = _rInfo.tile_clolor;
		break;
	}

	UI_pos = getUIPos(_rInfo);
	rect = RectF(UI_pos, _rInfo.tile_side);
}

void RTile::draw(const RInfo& _rInfo) {
	rect.draw(color);
	rect.drawFrame(1, _rInfo.line_color);
	_rInfo.score_font(score).draw(Arg::bottomRight = UI_pos + Vec2(_rInfo.tile_side, _rInfo.tile_side) - Vec2(2, -2), Palette::Lightgrey);
}

Vec2 RTile::getRectPos()
{
	return rect.pos;
}

void RTile::setColor(Color c) {
	color = c;
}
