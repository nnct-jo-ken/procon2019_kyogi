#include "Sysloop.h"

void game_loop(Game& game, Renderer& renderer, std::queue<ACT_STATE>& queue, std::mutex& mtx, bool& restart)
{
	while (System::Update())
	{
		mtx.lock();
		if (restart)
		{
			restart = false;
			mtx.unlock();

			game.init();
			renderer.init(game.getBoardState());
		}
		else {
			mtx.unlock();
		}

		if (System::GetPreviousEvent() == WindowEvent::CloseButton)
		{
			System::Exit();
		}
		game.load_queue(queue, mtx);
		renderer.update(game.getAgentVector());

		if (KeyEnter.down()) {
			game.updateTurn();
			renderer.updateTurn(game.getBoardState());
		}
		Sleep(50);
	}
}

void server_loop(Server& server, Game& game, std::queue<ACT_STATE>& queue, std::mutex& mtx, bool& restart)
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
				server.wait_cmd(game.getBoardState(), i, mtx, restart);
				break;
			case WAIT:
				server.wait_act(i, queue, mtx);
				break;
			}
		}
		Sleep(20);
	}
	server.close();
}