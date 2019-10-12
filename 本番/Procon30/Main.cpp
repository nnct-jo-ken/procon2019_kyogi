#pragma once
#include <iostream>
#include <Siv3D.hpp>
#include "Renderer.h"
#include "Game.h"
#include "TCP_Server.h"
#include "MTCS.h"

BOARD_STATE solver(const BOARD_STATE &board)
{
	BOARD_STATE result = board;
	vector<vector<ACT_STATE>> act_list;
	for (int i = 1; i <= 2; i++)
	{
		vector<ACT_STATE> tmp;
		MTCS ai(result, i);
		ai.search(tmp);
		ai.clean();
		act_list.push_back(tmp);
	}

	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < result.agents_count; i++)
		{
			result.agents[i + j * result.agents_count].act_type = act_list[j][i].type;
			result.agents[i + j * result.agents_count].delta_pos = Vector2(act_list[j][i].dx, act_list[j][i].dy);
		}
	}
	return result;
}

std::string make_send_json(BOARD_STATE board)
{
	std::vector<json11::Json> act_list;

	for (int i = 0; i < board.agents_count; i++)
	{
		std::string type;
		switch (board.agents[i].act_type)
		{
		case 0: type = std::string("stay"); break;
		case 1: type = std::string("move"); break;
		case 2: type = std::string("remove"); break;
		}

		act_list.push_back(json11::Json::object({
			{"agentID", board.agents[i].ID},
			{"dx", board.agents[i].delta_pos.x},
			{"dy", board.agents[i].delta_pos.y},
			{"type", type}}));
	}

	json11::Json json_obj = json11::Json::object({ { "actions", act_list } });
	return json_obj.dump();
}

std::string receive_json(u_short port)
{
	char buf[8192];
	TCP_Server server;
	server.init(port);	
	server.tcp_open();
	server.tcp_accept();
	server.tcp_recv(buf, 8192);
	server.tcp_close();
	return std::string(buf);
}

void send_json(u_short port,BOARD_STATE& board)
{
	std::string json_str = make_send_json(board);
	
	WSADATA wsaData;
	struct sockaddr_in server;
	SOCKET clientsock;
	char buf[4096] = { 0 };

	WSAStartup(MAKEWORD(2, 0), &wsaData);
	clientsock = socket(AF_INET, SOCK_STREAM, 0);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	int i = 0;
	for (; json_str[i] != 0; i++)
	{
		buf[i] = json_str[i];
	}
	buf[i] = '\0';

	connect(clientsock, (struct sockaddr*) & server, sizeof(server));
	send(clientsock, buf, 4096, 0);
	WSACleanup();
}

void get_my_data(int& all_turns, int& real_team_id, int& team_index)
{
	char buf[128] = { 0 };
	TCP_Server server;
	server.init(7777);
	server.tcp_open();
	server.tcp_accept();
	server.tcp_recv(buf, 128);
	server.tcp_close();

	std::cout << std::string(buf) << std::endl;

	std::string err;
	auto json = json11::Json::parse(std::string(buf), err);
	all_turns = json["all_turns"].int_value();
	real_team_id = json["real_team_id"].int_value();
	team_index = json["team_index"].int_value();
}

void Main() {
	Console.open();
	std::cout << "start up" << std::endl;
	int all_turns;
	int real_team_id;
	int team_index;
	get_my_data(all_turns, real_team_id, team_index);
	std::cout << "get first data : " << all_turns << std::endl;
	std::cout << "real : " << team_index << std::endl;
	std::cout << "team : " << real_team_id << std::endl;

	std::string json_str = receive_json((u_short)7755 + (u_short)team_index);
	std::cout << "get json" << std::endl;
	std::cout << json_str << std::endl;
	BOARD_STATE board = Game::parse_json(json_str, all_turns, real_team_id);
	Sleep(1000);
	Renderer renderer1;
	renderer1.init(board);
	Sleep(1000);
	while (System::Update())
	{
		renderer1.update(board.agents);

		board = solver(board);
		send_json((u_short)5577 + (u_short)team_index, board);

		json_str = receive_json((u_short)7755 + (u_short)team_index);
		std::cout << "get json2" << std::endl;
		std::cout << json_str << std::endl;
		board = Game::parse_json(json_str, all_turns, real_team_id);
	}
}