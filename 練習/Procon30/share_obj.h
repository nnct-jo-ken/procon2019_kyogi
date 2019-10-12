#pragma once
#include <thread>
#include <atomic>
#include "Game.h"

struct share_obj {
	Game& game;
	std::mutex& mtx;
	std::queue<ACT_STATE>& queue;
	std::array<std::atomic<bool>, 2>& restart;
	std::array<std::atomic<bool>, 2>& update_turn;
	std::atomic<bool>& done_update;
	std::atomic<bool>& restart_gui;
	std::atomic<bool>& update_gui;
	std::atomic<bool>& close_window;
}typedef share_obj;