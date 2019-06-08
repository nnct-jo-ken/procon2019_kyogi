#include "Sysloop.h"

void game_loop(share_obj& share, Renderer& renderer)
{
	while (System::Update())
	{
		if (share.restart.load())
		{
			share.restart.store(false, std::memory_order_seq_cst);

			share.mtx.lock();
			share.game.clear();
			renderer.clear();

			share.game.init();
			renderer.init(share.game.getBoardState());
			share.mtx.unlock();
		}
	
		if (System::GetPreviousEvent() == WindowEvent::CloseButton)
		{
			System::Exit();
		}

		if (KeyEnter.down()) 
		{
			share.update_turn.store(true, std::memory_order_seq_cst);
		}

		if (share.update_turn.load())
		{
			share.update_turn.store(false, std::memory_order_seq_cst);
			share.mtx.lock();
			share.game.updateTurn();
			share.mtx.unlock();
			renderer.updateTurn(share.game.getBoardState());
		}

		share.game.load_queue(share.queue, share.mtx);
		share.mtx.lock();
		renderer.update(share.game.getAgentVector());
		share.mtx.unlock();
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
				server.wait_cmd(share.game.getBoardState(), i, share.restart, share.update_turn);
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