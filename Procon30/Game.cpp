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

int Game::getAgentQuentity()
{
	return board.agent_quentity;
}

std::vector<Agent>& Game::getAgentVector() {
	return board.agents;
}

void Game::init() {
	// 本来はhttpでサーバーと通信して情報を得る
	board.width = 6;
	board.height = 6;
	board.turn = 50;
	board.now_turn = 1;
	board.agent_quentity = 2;

	// エージェントの初期化
	/*try {
		for (int i = 0; i < board.agent_quentity * 2; i++) {
			if (i < board.agent_quentity) {
				board.agents.push_back(Agent(Vector2(i, 0), 1));
			}
			else {
				board.agents.push_back(Agent(Vector2(i, 0), 2));
			}
		}
	}
	catch (std::invalid_argument e) {
		throw std::invalid_argument("game agent init exception.");
	}*/
	board.agents.push_back(Agent(Vector2(0, 0), 1));
	board.agents.push_back(Agent(Vector2(5, 5), 1));

	board.agents.push_back(Agent(Vector2(5, 0), 2));
	board.agents.push_back(Agent(Vector2(0, 5), 2));

	// タイルのスコアの設定
	for (int y = 0; y < board.height; y++) {
		for (int x = 0; x < board.width; x++) {
		//	board.tile_score.push_back(y * board.width + x);
			board.tile_score.push_back(-1);
		}
	}

	// タイルのチーム情報の初期化
	for (int y = 0; y < board.height; y++) {
		for (int x = 0; x < board.width; x++) {
			board.tile_state.push_back(0);
		}
	}

	// エージェントの初期位置のタイルのチーム情報の設定
	for (int i = 0; i < board.agent_quentity * 2; i++) {
		setTileState(board.agents[i].getPos(), board.agents[i].getTeam());
	}

	board.game_score = getGameScore();
}

BOARD_STATE Game::getBoardState() {
	return board;
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

	if (num < 0 || num <= board.agent_quentity) {
		throw std::invalid_argument("Game::setStep() exception.");
	}

	if (abs(step.x) > 1 || abs(step.y) > 1) {
		throw std::invalid_argument("Game::setStep() exception.");
	}

	if (act != 0 && act != 1) {
		throw std::invalid_argument("Game::setStep() exception.");
	}

	Agent& tmp = board.agents[(team - 1) * board.agent_quentity + num];
	tmp.setActState(act);
	tmp.setTarget(tmp.getPos() + step);
}

void Game::setTileState(Vector2 pos, int _team) {
	if (_team != 1 && _team != 2) {
		throw std::invalid_argument("Game::setTileState() exception.");
	}
	board.tile_state[pos.y * board.width + pos.x] = _team;
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
		if (a.getActState() == 0) {
			a.resetAct();
		}
		if (a.getActState() == 1) {
			if (getTileState(a.getTarget()) != 0 && getTileState(a.getTarget()) != a.getTeam()) {
				a.resetAct();
			}
		}
		if (a.getActState() == 2) {
			if (a.getTeam() == 0) {
				a.resetAct();
			}
			if (getTileState(a.getTarget()) == 0 || getTileState(a.getTarget()) == a.getTeam()) {
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
		for (int i = 0; i < board.agent_quentity * 2 - 1; i++) {
			for (int i2 = i + 1; i2 < board.agent_quentity * 2; i2++) {
				Vector2 checkingPos1 = board.agents[i].getTarget();
				Vector2 checkingPos2 = board.agents[i2].getTarget();
				if (board.agents[i].getTarget() == board.agents[i2].getTarget()) {
					buttingTable[checkingPos1.y * board.width + checkingPos1.x] = true;
					board.agents[i].resetAct();
					board.agents[i2].resetAct();
					inCheck = true;
				}
				if (board.agents[i].getActState() == 2) {
					if (board.agents[i].getPos() == board.agents[i2].getTarget()) {
						buttingTable[checkingPos2.y * board.width + checkingPos2.x] = true;
						board.agents[i2].resetAct();
						inCheck = true;
					}
				}
				if (board.agents[i2].getActState() == 2) {
					if (board.agents[i2].getPos() == board.agents[i].getTarget()) {
						buttingTable[checkingPos1.y * board.width + checkingPos1.x] = true;
						board.agents[i].resetAct();
						inCheck = true;
					}
				}

				if (board.agents[i2].getActState() == 1) {

				}
				if (board.agents[i].getActState() != 0) {
					if (buttingTable[checkingPos1.y * board.width + checkingPos1.x]) {
						board.agents[i].resetAct();
					}
				}

				if (board.agents[i2].getActState() != 0) {
					if (buttingTable[checkingPos2.y * board.width + checkingPos2.x]) {
						board.agents[i2].resetAct();
					}
				}
			}
		}
	}

	// 実際のデータを更新してリセットする
	for (Agent& a : board.agents) {
		if (a.getActState() == 2) {
			board.tile_state[a.getTarget().y * board.width + a.getTarget().x] = 0;
		} 
		else {
			a.setPos(a.getTarget());
		}
		setTileState(a.getPos(), a.getTeam());
		a.resetAct();
	}

	board.game_score = getGameScore();

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

GAME_SCORE Game::getGameScore()
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
