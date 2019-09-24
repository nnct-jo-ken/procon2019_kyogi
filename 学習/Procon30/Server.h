#pragma once
#include <memory>
#include <string>
#include "share_obj.h"
#include "TCP_Server.h"
#include "json11/json11.hpp"
#include "Game.h"
#include "Buffers.h"

enum SERVER_STATE
{
	// NC : 接続なし
	// CONNECT : 接続中
	// WAIT : 行動のデータ待機
	NC, CONNECT, WAIT
}typedef SERVER_STATE;

class Server
{
private:
	std::vector<std::unique_ptr<TCP_Server>> tcp_server;

	void set_action(std::string tcp_json, std::queue<ACT_STATE>& queue, std::mutex& mtx);
public:
	std::vector<SERVER_STATE> server_state;
	Server(u_short port1, u_short port2);
	void open();
	void accept(int index);
	void wait_cmd(BOARD_STATE board, int index, std::atomic<bool>& restart, std::atomic<bool>& update_turn);
	void wait_act(int index, std::queue<ACT_STATE>& queue, std::mutex& mtx);
	void close();
};

