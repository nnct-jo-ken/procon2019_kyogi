#pragma once
#include <iostream>
#include "Renderer.h"
#include "Game.h"
#include "Server.h"
#include "MTCS.h"

BOARD_STATE solver(const BOARD_STATE &board)
{
	BOARD_STATE result = board;
	vector<vector<ACT_STATE>> act_list;
	for (int i = 1; i <= 2; i++)
	{
		vector<ACT_STATE> tmp;
		MTCS ai(result, i);
		ai.search(tmp);
		ai.clean();
		act_list.push_back(tmp);
	}

	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < result.agents_count; i++)
		{
			result.agents[i + j * result.agents_count].act_type = act_list[j][i].type;
			result.agents[i + j * result.agents_count].delta_pos = Vector2(act_list[j][i].dx, act_list[j][i].dy);
		}
	}
	return result;
}

void Main() {
	Sleep(1000);
	Game game1;
	Renderer renderer1;
	
	Console.open();

	// 初期化
	game1.init();
	renderer1.init(game1.board);

	BOARD_STATE result;
	while (System::Update())
	{
		renderer1.update(game1.board.agents);

		if (System::FrameCount() % 2 == 0)
		{
			game1.board = solver(game1.board);
		}
		else {
			game1.updateTurn(game1.board);
			renderer1.updateTurn(game1.board);
			
			if (game1.board.turn <= 0)
			{
				game1 = Game();
				renderer1 = Renderer();
				game1.init();
				renderer1.init(game1.board);
			}
		}
	}
}