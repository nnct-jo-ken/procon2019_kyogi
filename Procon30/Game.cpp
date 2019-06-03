#include "Game.h"

int Game::getTileScore(int index)
{
	return board.tile_score[index];
}

int Game::getTileScore(Vector2 pos)
{
	return board.tile_score[pos.y * board.width + pos.x];
}

int Game::getTileState(int index)
{
	return board.tile_state[index];
}

int Game::getTileState(Vector2 pos) {
	return board.tile_state[pos.y * board.width + pos.x];
}

int Game::getTurn() {
	return board.turn;
}

int Game::getNowTurn() {
	return board.now_turn;
}

int Game::getAgentQant()
{
	return board.agent_count;
}

std::vector<Agent>& Game::getAgentVector() {
	return board.agents;
}

void Game::init() {
	// ./resource/field.jsonを読み込み、反映する
	// ファイル読み込み
	std::ifstream ifs("../resource/field.json");
	if (ifs.fail()) {
		exit(-1);
	}

	// なんかやばい方法
	std::istreambuf_iterator<char> it(ifs);
	std::istreambuf_iterator<char> last;
	std::string str(it, last);
	// 仮チームID
	board.team_ID = 5;
	parse_json(str);

	board.game_score = countGameScore();
}

void Game::parse_json(std::string json_str)
{
	std::string err;
	const json11::Json json = json11::Json::parse(json_str, err);
	
	board.width = json["width"].int_value();
	board.height = json["height"].int_value();
	board.turn = json["turn"].int_value();
	board.now_turn = json["turn"].int_value();

	// タイルポイント
	for (auto& col : json["points"].array_items())
	{
		for (int i = 0; i < board.width; i++)
		{
			board.tile_score.push_back(col[i].int_value());
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
				board.tile_state.push_back(tile_coler);
			} 
			else if (tile_coler == board.team_ID)
			{
				board.tile_state.push_back(1);
			}
			else
			{
				board.tile_state.push_back(2);
			}
		}
	}

	// エージェント格納
	for (int i = 0; i < 2; i++)
	{
		board.agent_count = 0;
		auto team_obj = json["teams"].array_items()[i];
		for (auto& agent_obj : team_obj["agents"].array_items())
		{
			board.agent_count++;
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
		for (int i = 0; i < board.agent_count; i++)
		{
			std::iter_swap(&board.agents[i], &board.agents[i + board.agent_count]);
		}
	}
}

BOARD_STATE Game::getBoardState() {
	return board;
}

GAME_SCORE Game::getGameScore()
{
	return board.game_score;
}

int Game::getWidth()
{
	return board.width;
}

int Game::getHeight()
{
	return board.height;
}

int Game::getVectorSize()
{
	return (int)board.tile_score.size();
}

void Game::setAct(int team, int num, int act, Vector2 step) {
	if (team != 1 && team != 2) {
		throw std::invalid_argument("Game::setStep() exception.");
	}

	if (num < 0 || num <= board.agent_count) {
		throw std::invalid_argument("Game::setStep() exception.");
	}

	if (abs(step.x) > 1 || abs(step.y) > 1) {
		throw std::invalid_argument("Game::setStep() exception.");
	}

	if (act != 0 && act != 1) {
		throw std::invalid_argument("Game::setStep() exception.");
	}

	Agent& tmp = board.agents[(team - 1) * board.agent_count + num];
	tmp.setActType(act);
	tmp.setDeltaMove(tmp.getPos() + step);
}

void Game::setTileState(Vector2 pos, int _team) {
	if (_team != 1 && _team != 2) {
		throw std::invalid_argument("Game::setTileState() exception.");
	}
	board.tile_state[pos.y * board.width + pos.x] = _team;
}

void Game::load_queue(std::queue<ACT_STATE>& queue, std::mutex& mtx)
{
	mtx.lock();
	while (!queue.empty()) 
	{
		ACT_STATE tmp = queue.front();
		for (Agent& a : board.agents)
		{
			if (a.getID() == tmp.agent_id)
			{
				a.setDeltaMove(Vector2(tmp.dx, tmp.dy));
				a.setActType(tmp.type);
			}
		}
		queue.pop();
	}

	mtx.unlock();
}

bool Game::updateTurn() {
	// エージェントの移動先が盤面の範囲内かを調べる
	for (Agent& a : board.agents) {
		Vector2 t_pos = a.getTarget();
		if (t_pos.x < 0 || t_pos.y < 0 || t_pos.x >= board.width || t_pos.y >= board.height) {
			a.resetAct();
		}
	}

	// 移動先のタイルと自身の色を比較して移動か削除かを決める
	for (Agent& a : board.agents) {
		if (a.getActType() == 0) {
			a.resetAct();
		}
		if (a.getActType() == 1) {
			if (getTileState(a.getTarget()) != 0 && getTileState(a.getTarget()) != a.getTeam()) {
				a.resetAct();
			}
		}
		if (a.getActType() == 2) {
			if (getTileState(a.getTarget()) == 0) {
				a.resetAct();
			}
		}
	}

	// バッティングを調べる
	std::vector<bool> buttingTable; // 過去にバッティングが起こったかを記録しておく
	for (int i = 0; i < board.width * board.height; i++) {
		buttingTable.push_back(false);
	}
	bool inCheck = true;
	while (inCheck) {
		// エージェントの行動の変化がなくなるまで調べる
		inCheck = false;
		for (int i = 0; i < board.agent_count * 2 - 1; i++) {
			for (int i2 = i + 1; i2 < board.agent_count * 2; i2++) {
				Vector2 checkingPos1 = board.agents[i].getTarget();
				Vector2 checkingPos2 = board.agents[i2].getTarget();
				if (board.agents[i].getTarget() == board.agents[i2].getTarget()) {
					buttingTable[checkingPos1.y * board.width + checkingPos1.x] = true;
					board.agents[i].resetAct();
					board.agents[i2].resetAct();
					inCheck = true;
				}
				if (board.agents[i].getActType() == 2) {
					if (board.agents[i].getPos() == board.agents[i2].getTarget()) {
						buttingTable[checkingPos2.y * board.width + checkingPos2.x] = true;
						board.agents[i2].resetAct();
						inCheck = true;
					}
				}
				if (board.agents[i2].getActType() == 2) {
					if (board.agents[i2].getPos() == board.agents[i].getTarget()) {
						buttingTable[checkingPos1.y * board.width + checkingPos1.x] = true;
						board.agents[i].resetAct();
						inCheck = true;
					}
				}

				if (board.agents[i2].getActType() == 1) {

				}
				if (board.agents[i].getActType() != 0) {
					if (buttingTable[checkingPos1.y * board.width + checkingPos1.x]) {
						board.agents[i].resetAct();
					}
				}

				if (board.agents[i2].getActType() != 0) {
					if (buttingTable[checkingPos2.y * board.width + checkingPos2.x]) {
						board.agents[i2].resetAct();
					}
				}
			}
		}
	}

	// 実際のデータを更新してリセットする
	for (Agent& a : board.agents) {
		if (a.getActType() == 2) {
			board.tile_state[a.getTarget().y * board.width + a.getTarget().x] = 0;
		} 
		else {
			a.setPos(a.getTarget());
		}
		setTileState(a.getPos(), a.getTeam());
		a.resetAct();
	}

	board.game_score = countGameScore();

	// 残りターン数を更新する
	board.turn--;

	return false;
}

int Game::getAreaScore(int team) 
{
	int area_score = 0;
	int tmp;
	bool reach_end;
	// 探索済みのタイルの配列
	std::vector<bool> is_searched(board.width * board.height, false);
	for (int y = 0; y < board.height; y++)
	{
		for (int x = 0; x < board.width; x++)
		{
			tmp = 0;
			reach_end = false;

			if (!is_searched[y * board.width + x])
			{
				areaScoreRecursion(team, x, y, is_searched, tmp, reach_end);
			}

			if (!reach_end)
			{
				area_score += tmp;
			}
		}
	}
	return area_score;
}

void Game::areaScoreRecursion(int team, int x, int y, std::vector<bool>& table, int& tmp, bool& reach_end)
{
	// 範囲外判定
	if (x < 0 || x >= board.width || y < 0 || y >= board.height) { return; }
	// 自分のチームのタイルの場合
	if (board.tile_state[y * board.width + x] == team) { return; }
	// 既に探索済みの場合
	if (table[y * board.width + x]) { return; }
	// 筋判定
	if (x == 0 || x == board.width - 1 || y == 0 || y == board.height - 1) { reach_end = true;}

	tmp += abs(board.tile_score[y * board.width + x]);
	table[y * board.width + x] = true;

	areaScoreRecursion(team, x + 1, y, table, tmp, reach_end);
	areaScoreRecursion(team, x - 1, y, table, tmp, reach_end);
	areaScoreRecursion(team, x, y + 1, table, tmp, reach_end);
	areaScoreRecursion(team, x, y - 1, table, tmp, reach_end);
}

GAME_SCORE Game::countGameScore()
{
	GAME_SCORE score_tmp = {};
	for (int y = 0; y < board.height; y++)  {
		for (int x = 0; x < board.width; x++) {
			if (board.tile_state[y * board.width + x] == 1) {
				score_tmp.tile_team1 += board.tile_score[y * board.width + x];
			}
			else if (board.tile_state[y * board.width + x] == 2) {
				score_tmp.tile_team2 += board.tile_score[y * board.width + x];
			}
		}
	}

	score_tmp.area_team1 = getAreaScore(1);
	score_tmp.area_team2 = getAreaScore(2);

	return score_tmp;
}
