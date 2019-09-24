#pragma once
#include <Siv3D.hpp>
#include "Game.h"

enum OPERATION_STATE {
	NONE, MOVE, REMOVE
};

struct RInfo {
	int		margin_top, margin_left, margin_right, margin_bottom;
	Vec2	field_origin;
	int		tile_side;
	int		agent_radius;
	Color	background_color;
	Color	line_color;
	Color	team1_color;
	Color	team2_color;
	Color	tile_clolor;
	Font	id_font;
	Font	score_font;
}typedef RInfo;