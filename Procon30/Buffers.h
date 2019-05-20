#pragma once
#include <string>
#include "Game.h"

class Buffers
{
public:
	// 盤面の情報の通信用の文字列を生成する
	static std::string createBoardBuffer(BOARD_STATE board);
};

