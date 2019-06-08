#pragma once
#include <thread>
#include <atomic>
#include "Game.h"

struct share_obj {
	Game& game;
	std::mutex& mtx;
	std::queue<ACT_STATE>& queue;
	std::atomic<bool>& restart;
	std::atomic<bool>& update_turn;
	std::atomic<bool>& restart_gui;
	std::atomic<bool>& update_gui;
}typedef share_obj;