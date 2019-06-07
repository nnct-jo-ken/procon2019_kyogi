#pragma once
#include <iostream>
#include <thread>
#include <atomic>
#include "Game.h"
#include "Renderer.h"
#include "Server.h"

void game_loop(
	Game& game, 
	Renderer& renderer, 
	std::queue<ACT_STATE>& queue, 
	std::mutex& mtx, 
	std::atomic<bool>& restart, 
	std::atomic<bool>& update_turn);

void server_loop(
	Server& server, 
	Game& game, 
	std::queue<ACT_STATE>& queue, 
	std::mutex& mtx, 
	std::atomic<bool>& restart, 
	std::atomic<bool>& update_turn);

void render_loop(
	Game& game,
	Renderer& renderer,
	std::mutex& mtx,
	std::atomic<bool>& restart,
	std::atomic<bool>& update_turn);