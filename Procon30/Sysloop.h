#pragma once
#include <iostream>
#include <thread>
#include "Game.h"
#include "Renderer.h"
#include "Server.h"

void game_loop(Game& game, Renderer& renderer, std::queue<ACT_STATE>& queue, std::mutex& mtx, bool& restart);
void server_loop(Server& server, Game& game, std::queue<ACT_STATE>& queue, std::mutex& mtx, bool& restart);
