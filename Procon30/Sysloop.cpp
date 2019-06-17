#include "Sysloop.h"

void game_loop(share_obj& share)
{
	while (1)
	{
		if (share.restart[0].load() && share.restart[1].load())
		{
			for (int i = 0; i < 2; i++)
			{
				share.restart[i].store(false, std::memory_order_seq_cst);
			}
			share.mtx.lock();
			share.game.clear();
			share.game.init();
			share.mtx.unlock();
			
			// gui
			share.restart_gui.store(true, std::memory_order_seq_cst);
		}
	
		if (System::GetPreviousEvent() == WindowEvent::CloseButton)
		{
			break;
		}

		if (share.update_turn[0].load() && share.update_turn[1].load())
		{
			for (int i = 0; i < 2; i++)
			{
				share.update_turn[i].store(false, std::memory_order_seq_cst);
			}
			share.mtx.lock();
			share.game.updateTurn();
			std::cout << Buffers::createJson(share.game.getBoardState(), 1);
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
				server.wait_cmd(share.game.getBoardState(), i, share.restart[i], share.update_turn[i]);
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
			renderer.init(share.game.getBoardState());
			share.mtx.unlock();
		}

		if (share.update_gui.load())
		{
			share.update_gui.store(false, std::memory_order_seq_cst);
			share.mtx.lock();
			renderer.updateTurn(share.game.getBoardState());
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
		renderer.update(share.game.getAgentVector());
		share.mtx.unlock();
	}
}