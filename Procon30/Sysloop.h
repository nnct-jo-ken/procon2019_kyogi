#pragma once
#include <iostream>
#include "share_obj.h"
#include "Game.h"
#include "Renderer.h"
#include "Server.h"

void game_loop(share_obj& share, Renderer& renderer);
void server_loop(share_obj& share, Server& server);