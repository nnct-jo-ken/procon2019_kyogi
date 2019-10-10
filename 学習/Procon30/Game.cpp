#include "Game.h"

void Game::init() 
{
	std::thread py_server([] {system("python ../Board_Generator.py"); });
	py_server.detach();

	std::string strs;
	strs = getJsonFromServer();

	// 仮チームID
	board.team_ID = 1;
	parse_json(strs);

	board.game_score = countGameScore(board);
}

std::string Game::getJsonFromServer() 
{
	WSADATA wsaData;
	struct sockaddr_in server;
	SOCKET sock;
	char buf[8192] = {};

	WSAStartup(MAKEWORD(2, 0), &wsaData);

	sock = socket(AF_INET, SOCK_STREAM, 0);

	server.sin_family = AF_INET;
	server.sin_port = htons(5678);
	server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	connect(sock, (struct sockaddr*) & server, sizeof(server));

	recv(sock, buf, sizeof(buf), 0);

	WSACleanup();

	return std::string(buf);
}

void Game::parse_json(std::string json_str)
{
	std::string err;
	const json11::Json json = json11::Json::parse(json_str, err);
	
	board.width = json["width"].int_value();
	board.height = json["height"].int_value();
	board.turn = json["allTurn"].int_value() - json["turn"].int_value();

	// タイルポイント
	for (auto& col : json["points"].array_items())
	{
		for (int i = 0; i < board.width; i++)
		{
			board.tile_points.push_back(col[i].int_value());
		}
	}

	// タイルの状態
	for (auto& col : json["tiled"].array_items())
	{
		for (int i = 0; i < board.width; i++)
		{
			int tile_coler = col[i].int_value();
			if (tile_coler == 0)
			{
				board.tile_color.push_back(tile_coler);
			} 
			else if (tile_coler == board.team_ID)
			{
				board.tile_color.push_back(1);
			}
			else
			{
				board.tile_color.push_back(2);
			}
		}
	}

	// エージェント格納
	for (int i = 0; i < 2; i++)
	{
		board.agents_count = 0;
		auto team_obj = json["teams"].array_items()[i];
		for (auto& agent_obj : team_obj["agents"].array_items())
		{
			board.agents_count++;
			board.agents.push_back(
				Agent(
					Vector2(
						agent_obj["x"].int_value() - 1,
						agent_obj["y"].int_value() - 1
					),
					team_obj["teamID"].int_value() == board.team_ID ? 1 : 2,
					agent_obj["agentID"].int_value()
				)
			);
		}
	}

	// 自分のチームがコンテナの前半に来るように入れ替え
	if (board.team_ID != json["teams"].array_items()[0]["teamID"].int_value())
	{
		for (int i = 0; i < board.agents_count; i++)
		{
			std::iter_swap(&board.agents[i], &board.agents[i + board.agents_count]);
		}
	}
}

void Game::load_queue(std::queue<ACT_STATE>& queue, std::mutex& mtx)
{
	mtx.lock();
	while (!queue.empty()) 
	{
		ACT_STATE tmp = queue.front();
		for (Agent& a : board.agents)
		{
			if (a.ID == tmp.agent_id)
			{
				a.delta_pos = Vector2(tmp.dx, tmp.dy);
				a.act_type = tmp.type;
			}
		}
		queue.pop();
	}

	mtx.unlock();
}

bool Game::updateTurn(BOARD_STATE& _board)
{

	// 最初から停留だったエージェントを調べておく
	// この関数の最後にこれらのエージェントはバッティングを起こさなかったことにする
	std::vector<bool> was_stay;
	for (Agent a : _board.agents)
	{
		if (a.act_type == 0)
		{
			was_stay.push_back(true);
		}
		else
		{
			was_stay.push_back(false);
		}
	}

	// バッティングと不正の記録のリセット
	for (Agent& a : _board.agents)
	{
		a.done_butting = false;
		a.done_bad_act = false;
	}

	// エージェントの移動先が盤面の範囲内かを調べる
	for (Agent& a : _board.agents) 
	{
		Vector2 t_pos = a.pos + a.delta_pos;

		if (t_pos.x < 0 || t_pos.y < 0 || t_pos.x >= _board.width || t_pos.y >= _board.height) {
			a.resetAct();
			// 不正として処理
			a.done_bad_act = true;
		}
	}

	// 移動先のタイルと自身の色を比較して移動か削除かを決める（不正チェック）
	for (Agent& a : _board.agents) {
		int _pos_x = a.pos.x + a.delta_pos.x;
		int _pos_y = a.pos.y + a.delta_pos.y;
		switch (a.act_type)
		{
		case 0:
			a.resetAct();
			a.done_bad_act = true;
			break;
		case 1:
			if (_board.tile_color[_pos_y * _board.width + _pos_x] != 0 && _board.tile_color[_pos_y * _board.width + _pos_x] != a.team) {
				a.done_bad_act = true;
				a.resetAct();
			}
			break;
		case 2:
			if (_board.tile_color[_pos_y * _board.width + _pos_x] == 0) {
				a.done_bad_act = true;
				a.resetAct();
			}
			break;
		}
	}

	// バッティングを調べる
	std::vector<bool> buttingTable; // 過去にバッティングが起こったかを記録しておく
	for (int i = 0; i < _board.width * _board.height; i++) {
		buttingTable.push_back(false);
	}
	bool inCheck = true;
	while (inCheck) {
		// エージェントの行動の変化がなくなるまで調べる
		inCheck = false;
		for (int i = 0; i < _board.agents_count * 2 - 1; i++) {
			for (int i2 = i + 1; i2 < _board.agents_count * 2; i2++) {
				Vector2 checkingPos1 = _board.agents[i].pos + _board.agents[i].delta_pos;
				Vector2 checkingPos2 = _board.agents[i2].pos + _board.agents[i2].delta_pos;
				if (checkingPos1 == checkingPos2) {
					buttingTable[checkingPos1.y * _board.width + checkingPos1.x] = true;
					_board.agents[i].resetAct();
					_board.agents[i2].resetAct();
					_board.agents[i].done_butting = true;
					_board.agents[i2].done_butting = true;
					inCheck = true;
				}
				if (_board.agents[i].act_type == 2) {
					if (_board.agents[i].pos == checkingPos2) {
						buttingTable[checkingPos2.y * _board.width + checkingPos2.x] = true;
						_board.agents[i2].resetAct();
						_board.agents[i2].done_butting = true;
						inCheck = true;
					}
				}
				if (_board.agents[i2].act_type == 2) {
					if (_board.agents[i2].pos == checkingPos1) {
						buttingTable[checkingPos1.y * _board.width + checkingPos1.x] = true;
						_board.agents[i].resetAct();
						_board.agents[i].done_butting = true;
						inCheck = true;
					}
				}

				if (_board.agents[i2].act_type == 1) {

				}
				if (_board.agents[i].act_type != 0) {
					if (buttingTable[checkingPos1.y * _board.width + checkingPos1.x]) {
						_board.agents[i].done_butting = true;
						_board.agents[i].resetAct();
					}
				}

				if (_board.agents[i2].act_type != 0) {
					if (buttingTable[checkingPos2.y * _board.width + checkingPos2.x]) {
						_board.agents[i2].done_butting = true;
						_board.agents[i2].resetAct();
					}
				}
			}
		}
	}

	// 停留を選んだエージェントと不正をしたエージェントはバッティングを起こさなかったことにする
	int i = 0;
	for (Agent& a : _board.agents)
	{
		if (was_stay[i])
		{
			a.done_butting = false;
			a.done_bad_act = false;
		}

		if (a.done_bad_act)
		{
			a.done_butting = false;
		}
		i++;
	}

	// 実際のデータを更新してリセットする
	for (Agent& a : _board.agents) 
	{
		if (a.act_type == 2) {
			_board.tile_color[(a.pos + a.delta_pos).y * _board.width + (a.pos + a.delta_pos).x] = 0;
		} 
		else {
			a.pos = Vector2(a.pos + a.delta_pos);
		}
		_board.tile_color[a.pos.y * _board.width + a.pos.x] = a.team;
		a.resetAct();
	}

	_board.game_score = countGameScore(_board);

	// 残りターン数を更新する
	_board.turn--;

	return false;
}

int Game::getAreaScore(int team, BOARD_STATE& _board) 
{
	int area_score = 0;
	int tmp;
	bool reach_end;
	// 探索済みのタイルの配列
	std::vector<bool> is_searched(_board.width * _board.height, false);
	for (int y = 0; y < _board.height; y++)
	{
		for (int x = 0; x < _board.width; x++)
		{
			tmp = 0;
			reach_end = false;

			if (!is_searched[y * _board.width + x])
			{
				areaScoreRecursion(team, x, y, is_searched, tmp, reach_end, _board);
			}

			if (!reach_end)
			{
				area_score += tmp;
			}
		}
	}
	return area_score;
}

void Game::areaScoreRecursion(int team, int x, int y, std::vector<bool>& table, int& tmp, bool& reach_end, BOARD_STATE& _board)
{
	// 範囲外判定
	if (x < 0 || x >= _board.width || y < 0 || y >= _board.height) { return; }
	// 自分のチームのタイルの場合
	if (_board.tile_color[y * _board.width + x] == team) { return; }
	// 既に探索済みの場合
	if (table[y * _board.width + x]) { return; }
	// 筋判定
	if (x == 0 || x == _board.width - 1 || y == 0 || y == _board.height - 1) { reach_end = true;}

	tmp += abs(_board.tile_points[y * _board.width + x]);
	table[y * _board.width + x] = true;

	areaScoreRecursion(team, x + 1, y, table, tmp, reach_end, _board);
	areaScoreRecursion(team, x - 1, y, table, tmp, reach_end, _board);
	areaScoreRecursion(team, x, y + 1, table, tmp, reach_end, _board);
	areaScoreRecursion(team, x, y - 1, table, tmp, reach_end, _board);
}

GAME_SCORE Game::countGameScore(BOARD_STATE& _board)
{
	GAME_SCORE score_tmp = {};
	for (int y = 0; y < _board.height; y++)  {
		for (int x = 0; x < _board.width; x++) {
			if (_board.tile_color[y * _board.width + x] == 1) {
				score_tmp.tile_team1 += _board.tile_points[y * _board.width + x];
			}
			else if (_board.tile_color[y * _board.width + x] == 2) {
				score_tmp.tile_team2 += _board.tile_points[y * _board.width + x];
			}
		}
	}

	score_tmp.area_team1 = getAreaScore(1, _board);
	score_tmp.area_team2 = getAreaScore(2, _board);

	return score_tmp;
}
