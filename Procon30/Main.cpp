#pragma once
#include "Sysloop.h"
#include "share_obj.h"

#define GUI


void Main() {
	constexpr bool NO_GUI = true;
	Game game1;
	Renderer renderer1;
	Server server1 = Server(7755, 7756);
	System::SetExitEvent(WindowEvent::CloseButton);

	Console.open();

	std::mutex mtx;
	std::queue<ACT_STATE> queue;
	std::array<std::atomic<bool>, 2> restart{ {false, false} };
	std::array<std::atomic<bool>, 2> update_turn{ {false, false} };
	std::atomic<bool> restart_gui(false);
	std::atomic<bool> update_gui(false);
	std::atomic<bool> close_window(false);

	share_obj share = {
		game1,
		mtx,
		queue,
		restart,
		update_turn,
		restart_gui,
		update_gui,
		close_window
	};

	// 初期化
	game1.init();
	renderer1.init(game1.getBoardState());
	// スレッド開始
	std::thread gameThread1(game_loop, std::ref(share));
	std::thread serverThread1(server_loop, std::ref(share), std::ref(server1));
	std::thread renderThread1(render_loop, std::ref(share), std::ref(renderer1));

	gameThread1.join();
	serverThread1.join();
	renderThread1.join();
}