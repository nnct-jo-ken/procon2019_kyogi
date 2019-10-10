#include "Sysloop.h"

int game_count = 1;

void solver_loop(share_obj& share)
{
	while (true)
	{
		
		if (share.done_update.load()) 
		{
			vector<vector<ACT_STATE>> act_list;
			for (int i = 1; i <= 2; i++)
			{
				vector<ACT_STATE> tmp;
				MTCS ai(share.game.board, i);
				ai.search(tmp);
				ai.clean();
				// std::cout << ai.nodes_count << std::endl;
				act_list.push_back(tmp);
			}

			share.mtx.lock();
			for (int j = 0; j < 2; j++) {
				for (int i = 0; i < share.game.board.agents_count; i++)
				{
					share.game.board.agents[i + j * share.game.board.agents_count].act_type = act_list[j][i].type;
					share.game.board.agents[i + j * share.game.board.agents_count].delta_pos = Vector2(act_list[j][i].dx, act_list[j][i].dy);
				}
			}

			share.mtx.unlock();
			share.done_update.store(false, std::memory_order_seq_cst);
			share.update_turn[0].store(true, std::memory_order_seq_cst);
		}
	}
}

void game_loop(share_obj& share)
{
	while (1)
	{
		share.mtx.lock();
		
		share.mtx.unlock();

		if (share.restart[0].load() || share.restart[1].load())
		{
			game_count++;
			std::cout << "game count : " << game_count << std::endl;
			// gui
			share.restart_gui.store(true, std::memory_order_seq_cst);

			for (int i = 0; i < 2; i++)
			{
				share.restart[i].store(false, std::memory_order_seq_cst);
			}
			share.mtx.lock();
			share.game = Game();
			share.game.init();
			share.mtx.unlock();

			share.done_update.store(true, std::memory_order_seq_cst);
		}
	
		if (System::GetPreviousEvent() == WindowEvent::CloseButton)
		{
			break;
		}

		if (share.update_turn[0].load() || share.update_turn[1].load())
		{
			for (int i = 0; i < 2; i++)
			{
				share.update_turn[i].store(false, std::memory_order_seq_cst);
			}
			share.mtx.lock();
			share.game.updateTurn(share.game.board);
			for (Agent &a : share.game.board.agents)
			{
				if (a.done_bad_act)
				{
					std::cout << "team : " << a.team << ", " << "warning : done but action" << std::endl;
				}
			}
			share.mtx.unlock();

			// gui
			share.update_gui.store(true, std::memory_order_seq_cst);
			if (share.game.board.turn <= 0)
			{
				share.restart[0].store(true, std::memory_order_seq_cst);
			}
			else {
				share.done_update.store(true, std::memory_order_seq_cst);
			}
		}

		share.game.load_queue(share.queue, share.mtx);
		Sleep(10);
	}
}

void server_loop(share_obj& share, Server& server)
{
	server.open();
	while (1)
	{
		if (System::GetPreviousEvent() == WindowEvent::CloseButton)
		{
			server.close();
			break;
		}

		for (int i = 0; i < 2; i++)
		{
			switch (server.server_state[i])
			{
			case NC:
				server.accept(i);
				break;
			case CONNECT:
				share.mtx.lock();
				server.wait_cmd(share.game.board, i, share.restart[i], share.update_turn[i]);
				share.mtx.unlock();
				break;
			case WAIT:
				server.wait_act(i, share.queue, share.mtx);
				break;
			}
		}
		Sleep(20);
	}
	server.close();
}

void render_loop(share_obj& share, Renderer& renderer)
{
	while (System::Update())
	{
		// ×ボタンが押された処理
		if (System::GetPreviousEvent() == WindowEvent::CloseButton)
		{
			System::Exit();
		}

		if (share.restart_gui.load())
		{
			share.restart_gui.store(false, std::memory_order_seq_cst);
			share.mtx.lock();
			renderer.clear();
			renderer.init(share.game.board);
			share.mtx.unlock();
		}

		if (share.update_gui.load())
		{
			share.update_gui.store(false, std::memory_order_seq_cst);
			share.mtx.lock();
			renderer.updateTurn(share.game.board);
			share.mtx.unlock();
		}

		if (KeyEnter.down())
		{
			for (int i = 0; i < 2; i++)
			{
				share.update_turn[i].store(true, std::memory_order_seq_cst);
			}
		}

		share.mtx.lock();
		renderer.update(share.game.board.agents);
		share.mtx.unlock();
	}
}