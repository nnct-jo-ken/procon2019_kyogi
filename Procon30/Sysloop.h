#pragma once
#include <iostream>
#include "Renderer.h"
#include "share_obj.h"
#include "Game.h"
#include "Server.h"

void game_loop(share_obj& share);
void server_loop(share_obj& share, Server& server);
void render_loop(share_obj& share, Renderer& renderer);