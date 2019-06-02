#include "Buffers.h"

std::string Buffers::createBoardBuffer(BOARD_STATE board, int team)
{
	std::string buf = "";
	// 幅、高さ、残りターン、1チーム当たりのエージェント数を書き込む
	buf += std::to_string(board.width);
	buf += ',';
	buf += std::to_string(board.height);
	buf += ',';
	buf += std::to_string(board.turn);
	buf += ',';
	buf += std::to_string(board.agent_qant);
	buf += ',';

	// タイルポイントとエリアポイントを書き込む
	if (team == 1) {
		buf += std::to_string(board.game_score.tile_team1);
		buf += ',';
		buf += std::to_string(board.game_score.area_team1);
		buf += ',';
		buf += std::to_string(board.game_score.tile_team2);
		buf += ',';
		buf += std::to_string(board.game_score.area_team2);
	}
	if (team == 2) {
		buf += std::to_string(board.game_score.tile_team2);
		buf += ',';
		buf += std::to_string(board.game_score.area_team2);
		buf += ',';
		buf += std::to_string(board.game_score.tile_team1);
		buf += ',';
		buf += std::to_string(board.game_score.area_team1);
	}
	buf += ',';

	// 盤面の得点を書き込む
	for (int y = 0; y < board.height; y++) 
	{
		for (int x = 0; x < board.width; x++) 
		{
			buf += std::to_string(board.tile_score[y * board.width + x]);
			buf += ',';
		}
	}

	// 盤面の塗り具合を書き込む
	for (int y = 0; y < board.height; y++)
	{
		for (int x = 0; x < board.width; x++)
		{
			switch (board.tile_state[y * board.width + x])
			{
			case 0:
				buf += '0';
				break;
			case 1:
				if (team == 1) {
					buf += '1';
				}
				else
				{
					buf += '2';
				}
				break;
			case 2:
				if (team == 1) {
					buf += '2';
				}
				else
				{
					buf += '1';
				}
				break;
			}
			buf += ',';
		}
	}

	// エージェントのIDと座標を書き込む
	if (team == 1)
	{
		for (int i = 0; i < board.agent_qant * 2; i++)
		{
			buf += std::to_string(board.agents[i].getID());
			buf += ',';
			buf += std::to_string(board.agents[i].getPos().x);
			buf += ',';
			buf += std::to_string(board.agents[i].getPos().y);
			buf += ',';
		}
	}
	if (team == 2)
	{
		for (int i = board.agent_qant; i < board.agent_qant * 2; i++)
		{
			buf += std::to_string(board.agents[i].getID());
			buf += ',';
			buf += std::to_string(board.agents[i].getPos().x);
			buf += ',';
			buf += std::to_string(board.agents[i].getPos().y);
			buf += ',';
		}

		for (int i = 0; i < board.agent_qant; i++)
		{
			buf += std::to_string(board.agents[i].getID());
			buf += ',';
			buf += std::to_string(board.agents[i].getPos().x);
			buf += ',';
			buf += std::to_string(board.agents[i].getPos().y);
			buf += ',';
		}
	}

	// 仕上げ
	buf[buf.length() - 1] = '\0';

	return buf;
}