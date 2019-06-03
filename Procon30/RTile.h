#pragma once
#include "Rendering_INFO.h"

class RTile {
private:
	Color	color;
	int		score;
	Point	pos;
	Vec2	UI_pos;
	RectF	rect;

	Vec2 getUIPos(const RInfo& _rInfo);
public:
	RTile(int _team, int _score, Point _pos, const RInfo& _rInfo);
	void update(int _team, const RInfo& _rInfo);
	void draw(const RInfo& _rInfo);
	Vec2 getRectPos();
	void setColor(Color c);
};

