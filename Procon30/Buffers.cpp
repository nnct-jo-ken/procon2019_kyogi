#include "Buffers.h"

std::string Buffers::createBoardBuffer(BOARD_STATE board)
{
	std::string buf = "";
	// 幅、高さ、残りターン、1チーム当たりのエージェント数を書き込む
	buf += std::to_string(board.width);
	buf += ',';
	buf += std::to_string(board.height);
	buf += ',';
	buf += std::to_string(board.turn);
	buf += ',';
	buf += std::to_string(board.agent_quentity);
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
			buf += std::to_string(board.tile_state[y * board.width + x]);
			buf += ',';
		}
	}

	// エージェントの座標を書き込む
	for (int i = 0; i < board.agent_quentity * 2; i++)
	{
		buf += std::to_string(board.agents[i].getPos().x);
		buf += ',';
		buf += std::to_string(board.agents[i].getPos().y);
		buf += ',';
	}

	// 仕上げ
	buf[buf.length() - 1] = '\0';

	return buf;
}