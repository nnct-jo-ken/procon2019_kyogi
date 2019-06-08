#pragma once
#include "share_obj.h"
#include "Sysloop.h"

void Main() {
	Game game1;
	Renderer renderer1;
	Server server1 = Server(7755, 7756);
	System::SetExitEvent(WindowEvent::CloseButton);

	std::mutex mtx;
	std::queue<ACT_STATE> queue;
	std::atomic<bool> restart(false);
	std::atomic<bool> update_turn(false);
	std::atomic<bool> restart_gui(false);
	std::atomic<bool> update_gui(false);

	share_obj share = {
		game1,
		mtx,
		queue,
		restart,
		update_turn,
		restart_gui,
		update_gui
	};

	// 初期化
	game1.init();
	renderer1.init(game1.getBoardState());

	// スレッド開始
	std::thread gameThread1(game_loop, std::ref(share), std::ref(renderer1));
	std::thread serverThread1(server_loop, std::ref(share), std::ref(server1));

	gameThread1.join();
	serverThread1.join();
}