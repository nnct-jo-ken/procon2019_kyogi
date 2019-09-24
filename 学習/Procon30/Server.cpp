#include "Server.h"

Server::Server(u_short port1, u_short port2)
{
	for (int i = 0; i < 2; i++)
	{
		tcp_server.push_back(std::make_unique<TCP_Server>());
		server_state.push_back(NC);
	}


	tcp_server[0]->init(port1);
	tcp_server[1]->init(port2);
}

void Server::open()
{
	for (auto& sock : tcp_server)
	{
		sock->tcp_open();
	}
}

void Server::close()
{
	for (auto& sock : tcp_server)
	{
		sock->tcp_close();
		sock.reset();
	}
}

void Server::accept(int index)
{
	if (tcp_server[index]->tcp_accept())
	{
		server_state[index] = CONNECT;
	}
}

void Server::wait_cmd(BOARD_STATE board, int index, std::atomic<bool>& restart, std::atomic<bool>& update_turn)
{
	// クライアントからの要求を受け取る
	// データの要求 : INFO
	// 行動の送信 : ACT
	char recv_buf[128];
	int n = tcp_server[index]->tcp_recv(recv_buf, 128);

	// クライアントが切断
	if (n == 0)
	{
		server_state[index] = NC;
	}

	// ノンブロッキングソケットだから毎回あれする
	if (n >= 1) {
		std::string cmd(recv_buf);
		if (cmd == "INFO")
		{
			std::string board_str = Buffers::createJson(board, index + 1);
			char board_buf[8192] = { 0 };
			// string char* 変換
			int i;
			for (i = 0; i < board_str.length(); i++)
			{
				board_buf[i] = board_str[i];
			}
			board_buf[i] = 0;

			tcp_server[index]->tcp_send(board_buf, 8192);
		}

		if (cmd == "ACT")
		{
			tcp_server[index]->tcp_send("ok", 3);
			server_state[index] = WAIT;
		}


		if (cmd == "NEW")
		{
			restart.store(true, std::memory_order_seq_cst);
			tcp_server[index]->tcp_send("ok", 3);
		}

		if (cmd == "UPDATE")
		{
			update_turn.store(true, std::memory_order_seq_cst);
			tcp_server[index]->tcp_send("ok", 3);
		}
	}
}

void Server::wait_act(int index, std::queue<ACT_STATE>& queue, std::mutex& mtx)
{
	// クライアントからのデータを受け取る
	char recv_buf[1024];
	int n = tcp_server[index]->tcp_recv(recv_buf, 1024);
	tcp_server[index]->tcp_send("ok", 3);

	// クライアントが切断
	if (n == 0)
	{
		server_state[index] = NC;
	}

	// ノンブロッキングソケットだから毎回あれする
	if (n >= 1) {
		std::string act_str(recv_buf);
		set_action(act_str, queue, mtx);
		server_state[index] = CONNECT;
	}
}

void Server::set_action(std::string tcp_json, std::queue<ACT_STATE>& queue, std::mutex& mtx)
{
	std::string err;
	json11::Json json = json11::Json::parse(tcp_json, err);
	if (json.dump() == "null")
	{
		return;
	}

	for (auto& act_obj : json["actions"].array_items())
	{
		ACT_STATE tmp;
		tmp.agent_id = act_obj["agentID"].int_value();
		tmp.dx = act_obj["dx"].int_value();
		tmp.dy = act_obj["dy"].int_value();
		std::string type_str = act_obj["type"].string_value();
		if (type_str == "stay")
		{
			tmp.type = 0;
		}
		if (type_str == "move")
		{
			tmp.type = 1;
		}
		if (type_str == "remove")
		{
			tmp.type = 2;
		}

		mtx.lock();
		queue.push(tmp);
		mtx.unlock();
	}
}