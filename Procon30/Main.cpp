#pragma once
#include "Game.h"
#include "Renderer.h"

void Main() {
	Game game1;
	Renderer renderer1;

	game1.init();
	renderer1.init(game1.getBoardState());
}