#include "Sysloop.h"

int game_count = 1;

void game_loop(share_obj& share)
{
	while (1)
	{
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
			share.game.clear();
			share.game.init();
			share.mtx.unlock();			
		}
	
		if (System::GetPreviousEvent() == WindowEvent::CloseButton)
		{
			break;
		}

		if (share.update_turn[0].load() || share.update_turn[1].load())
		{
			// MCTS
			share.mtx.lock();
			{
				auto f_search = [share](int i, vector<ACT_STATE>& act_list) {
					MTCS ai(share.game.board, i);
					ai.search(act_list);
					std::cout << ai.nodes_count << std::endl;
					ai.clean();
				};

				vector<ACT_STATE> act_list1;
				vector<ACT_STATE> act_list2;

				/*std::thread mtcs1(f_search, 1, act_list1);
				std::thread mtcs2(f_search, 2, act_list2);
				mtcs1.join();
				mtcs2.join();*/

				f_search(1, act_list1);
				f_search(2, act_list2);

				for (int i = 0; i < share.game.board.agents_count; i++)
				{
					share.game.board.agents[i].act_type = act_list1[i].type;
					share.game.board.agents[i].delta_pos = Vector2(act_list1[i].dx, act_list1[i].dy);
					share.game.board.agents[i + share.game.board.agents_count].act_type = act_list2[i].type;
					share.game.board.agents[i + share.game.board.agents_count].delta_pos = Vector2(act_list2[i].dx, act_list2[i].dy);
				}
			}
			share.mtx.unlock();

			for (int i = 0; i < 2; i++)
			{
				share.update_turn[i].store(false, std::memory_order_seq_cst);
			}
			share.mtx.lock();
			share.game.updateTurn(share.game.board);
			share.mtx.unlock();

			// gui
			share.update_gui.store(true, std::memory_order_seq_cst);
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