#include "Buffers.h"

std::string Buffers::createJson(BOARD_STATE board, int team)
{
	// タイルの得点と色のvectorを2次元に直す
	std::vector<std::vector<int>> tile_score;
	std::vector<std::vector<int>> tile_color;

	for (int y = 0; y < board.height; y++)
	{
		std::vector<int> row_points;
		std::vector<int> row_color;
		for (int x = 0; x < board.width; x++)
		{
			row_points.push_back(board.tile_points[y * board.width + x]);
			row_color.push_back(board.tile_color[y * board.width + x]);
		}
		tile_score.push_back(row_points);
		tile_color.push_back(row_color);
	}

	// 敵を味方とする場合反転
	if (team == 2)
	{
		for (std::vector<int>& vec : tile_color)
		{
			for (int& e : vec)
			{
				switch (e)
				{
				case 1:
					e = 2;
					break;
				case 2:
					e = 1;
					break;
				}
			}
		}
	}

	// エージェントのリスト作成
	std::vector<json11::Json> agent_list[2];
	for (int i = 0; i < board.agents_count; i++)
	{
		agent_list[0].push_back(json11::Json::object({
				{"agentID", board.agents[i].ID},		// int
				{"x", board.agents[i].pos.x},			// int
				{"y", board.agents[i].pos.y},			// int
				{"butting", board.agents[i].done_butting},	// bool
				{"badAct", board.agents[i].done_bad_act} }) // bool
		);
	}
	for (int i = board.agents_count; i < board.agents_count * 2; i++)
	{
		agent_list[1].push_back(json11::Json::object({
				{"agentID", board.agents[i].ID},
				{"x", board.agents[i].pos.x},
				{"y", board.agents[i].pos.y},
				{"butting", board.agents[i].done_butting},
				{"badAct", board.agents[i].done_bad_act} })
		);
	}

	// チームが２ならエージェントのリストを入れ替える
	if (team == 2)
	{
		std::swap(agent_list[0], agent_list[1]);
	}

	int tile_point[2];
	int area_point[2];

	tile_point[0] = board.game_score.tile_team1;
	tile_point[1] = board.game_score.tile_team2;
	area_point[0] = board.game_score.area_team1;
	area_point[1] = board.game_score.area_team2;

	if (team == 2)
	{
		std::swap(tile_point[0], tile_point[1]);
		std::swap(area_point[0], area_point[1]);
	}

	// チームのjson作成
	std::vector<json11::Json> team_list;
	for (int i = 0; i < 2; i++)
	{
		team_list.push_back(json11::Json::object({
			{"teamID", i + 1},
			{"agents", agent_list[i]},
			{"tilePoint", tile_point[i]},
			{"areaPoint", area_point[i]}
			})
		);
	}

	std::vector<json11::Json> teams;
	for (json11::Json e : team_list)
	{
		teams.push_back(e);
	}

	// 送信するjson作成
	json11::Json game_json;
	game_json = json11::Json::object({
		{"width", board.width},
		{"height", board.height},
		{"agentCount", board.agents_count},
		{"points", tile_score},
		{"turn", board.turn},
		{"tiled", tile_color},
		{"teams", teams}
		}
	);

	return game_json.dump();
}