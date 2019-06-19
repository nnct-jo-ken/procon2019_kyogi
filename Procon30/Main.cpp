#pragma once
#include <string>

#include "Game.h"
#include "Renderer.h"
#include "TcpClient.h"

void Main() {
	Game game1;
	Renderer renderer1;
	TcpClient client;

	game1.init();
	renderer1.init(game1.getBoardState());

	Console.open();

	client.tcp_connect();
	client.tcp_send("INFO");
	std::string json_str = client.tcp_recv();
	std::cout << json_str << std::endl;

	client.tcp_close();
}