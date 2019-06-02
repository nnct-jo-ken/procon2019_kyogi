#pragma once
#include <iostream>
#include <thread>
#include "Game.h"
#include "Renderer.h"
#include "Server.h"

std::mutex mtx;

void game_loop(Game& game, Renderer& renderer, std::queue<ACT_STATE>& queue)
{
	while (System::Update())
	{
		if (System::GetPreviousEvent() == WindowEvent::CloseButton)
		{
			System::Exit();
		}
		game.load_queue(queue, mtx);
		renderer.update(game.getAgentVector());

		if (KeyEnter.down()) {
			game.updateTurn();
			renderer.updateTurn(game.getBoardState());
		}
		Sleep(50);
	}
}

void server_loop(Server& server, Game& game, std::queue<ACT_STATE>& queue)
{
	server.open();
	while (1)
	{
		if (System::GetPreviousEvent() == WindowEvent::CloseButton)
		{
			break;
		}

		for (int i = 0; i < 2; i++)
		{
			switch (server.server_state[i])
			{
			case NC:
				server.accept(i);
				break;
			case CONNECT:
				server.wait_cmd(game.getBoardState(), i);
				break;
			case WAIT:
				server.wait_act(i, queue, mtx);
				break;
			}
		}
		Sleep(20);
	}
	server.close();
}

void Main() {
	Game game1;
	Renderer renderer1;
	Server server1 = Server(7755, 7756);
	System::SetExitEvent(WindowEvent::CloseButton);
	std::queue<ACT_STATE> input_q;


	// 初期化
	game1.init();
	renderer1.init(game1.getBoardState());

	std::thread gameThread1(game_loop, std::ref(game1), std::ref(renderer1), std::ref(input_q));
	std::thread serverThread1(server_loop, std::ref(server1), std::ref(game1), std::ref(input_q));

	gameThread1.join();
	serverThread1.join();
}
