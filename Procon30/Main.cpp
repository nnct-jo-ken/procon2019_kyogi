#pragma once
#include "Sysloop.h"

std::mutex mtx;

void Main() {
	Game game1;
	Renderer renderer1;
	Server server1 = Server(7755, 7756);
	System::SetExitEvent(WindowEvent::CloseButton);
	std::queue<ACT_STATE> input_q;
	bool restart = false;

	// 初期化
	game1.init();
	renderer1.init(game1.getBoardState());

	std::thread gameThread1(game_loop, std::ref(game1), std::ref(renderer1), std::ref(input_q), std::ref(mtx), std::ref(restart));
	std::thread serverThread1(server_loop, std::ref(server1), std::ref(game1), std::ref(input_q), std::ref(mtx), std::ref(restart));

	gameThread1.join();
	serverThread1.join();
}
