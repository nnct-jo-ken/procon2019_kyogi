#include "Renderer.h"

// もはやこれは定数
void Renderer::initrInfo() {
	rInfo.margin_top = 30;
	rInfo.margin_right = 30;
	rInfo.margin_left = 30;
	rInfo.margin_bottom = 30;
	rInfo.field_origin = Vec2(0, 0);
	rInfo.tile_side = 30;
	rInfo.agent_radius = 14;
	rInfo.background_color = Color(255, 255, 255);
	rInfo.line_color = Color(0, 0, 0);
	rInfo.team1_color = Color(10, 82, 255);
	rInfo.team2_color = Color(255, 183, 10);
	rInfo.tile_clolor = Color(255, 255, 255);
	rInfo.id_font = Font(20);
	rInfo.score_font = Font(15);
}

void Renderer::init(const BOARD_STATE& board) {
	font = Font(20);
	initrInfo();
	render_board = board;
	for (int i = 0; i < board.agents_count * 2; i++) {
		Vector2 pos = board.agents[i].pos;
		ragents.push_back(RAgent(board.agents[i].team, board.agents[i].ID, i, Point(pos.x, pos.y), rInfo));
	}

	for (int i = 0; i < board.width * board.height; i++) {
		Point pos(i % board.width, i / board.width);
		tiles.push_back(RTile(board.tile_color[i], board.tile_points[i], pos, rInfo));
	}
	input.init(rInfo, board);
	Graphics::SetBackground(rInfo.background_color);
	Window::Resize(rInfo.margin_left + rInfo.tile_side * board.width + rInfo.margin_right + 200,
		rInfo.margin_right + rInfo.tile_side * board.height + rInfo.margin_bottom);

	std::cout << "board agent : " << board.agents_count << std::endl;
	std::cout << "gui agent   : " << ragents.size() / 2 << std::endl;
}

void Renderer::clear()
{
	render_board = { 0 };
	ragents.clear();
	tiles.clear();
	input.clear();
}

void Renderer::update(std::vector<Agent>& agents){

	for (RTile t : tiles) {
		t.draw(rInfo);
	}
	
	int i = 0;
	for (RAgent &a : ragents) {
		a.draw(rInfo);
		OPERATION_STATE ostate;
		switch (agents[i].act_type)
		{
		case 0:
			ostate = NONE;
			break;
		case 1:
			ostate = MOVE;
			break;
		case 2:
			ostate = REMOVE;
			break;
		default:
			ostate = NONE;
			break;
		}
		a.update_targetPos(Point(agents[i].pos.x + agents[i].delta_pos.x, agents[i].pos.y + agents[i].delta_pos.y), ostate);
		i++;
	}

	for (RAgent a : ragents) {
		a.drawArrow(rInfo);
	}
	input.update(ragents, tiles, agents);

	int dseux = rInfo.margin_left + rInfo.tile_side * render_board.width + rInfo.margin_right;
	font(render_board.game_score.tile_team1).draw(Arg::topLeft = Vec2(dseux, 40), rInfo.team1_color);
	font(render_board.game_score.area_team1).draw(Arg::topLeft = Vec2(dseux, 65), rInfo.team1_color);
	font(render_board.game_score.tile_team2).draw(Arg::topLeft = Vec2(dseux, 90), rInfo.team2_color);
	font(render_board.game_score.area_team2).draw(Arg::topLeft = Vec2(dseux, 115), rInfo.team2_color);
	font(U"残り", render_board.turn, U"ターン").draw(Arg::topLeft = Vec2(dseux, 150), Palette::Black);
}

void Renderer::updateTurn(const BOARD_STATE& board) {
	int i = 0;
	for (RTile& t : tiles) {
		if (board.tile_color[i] == 0) {
			t.setColor(rInfo.tile_clolor);
		}
		if (board.tile_color[i] == 1) {
			t.setColor(rInfo.team1_color);
		}
		if (board.tile_color[i] == 2) {
			t.setColor(rInfo.team2_color);
		}
		i++;
	}

	i = 0;
	for (RAgent& a : ragents) {
		Vector2 posMyVec2 = board.agents[i].pos;
		Point pos(posMyVec2.x, posMyVec2.y);
		a.update(pos, rInfo);
		i++;
	}

	render_board = board;
}