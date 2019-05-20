#pragma once
#include <iostream>
#include "Game.h"
#include "Renderer.h"
#include "Buffers.h"

void Main() {
	Game game1;
	Renderer renderer1;
	game1.init();
	renderer1.init(game1.getBoardState());
	std::string strs;
	strs = Buffers::createBoardBuffer(game1.getBoardState());
	Console.open();

	std::cout << strs << std::endl;
	std::cout << strs.length() << u8"文字" << std::endl;

	while (System::Update()) {
		renderer1.update(game1.getAgentVector());

		if (KeyEnter.down()) {
			game1.updateTurn();
			renderer1.updateTurn(game1.getBoardState());
		}
	}
}
