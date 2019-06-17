/*
-	クライアントに送信するjsonを作成するクラス
-	サーバーから提供されるjsonに近い形のものを作成するが、以下の点について異なる
-		1. チームIDは 1 又は 2
-		2. エージェントの座標は左上を(0, 0)として記述する
-		3. json中の"turn"は残りターン数を返す
-		4. "agentCount"として1チームのエージェント数を渡す
*/

#pragma once
#include <string>
#include <utility>
#include "Game.h"

class Buffers
{
public:
	// 盤面の情報の通信用のJson文字列を生成する
	static std::string createJson(BOARD_STATE board, int team);
};

