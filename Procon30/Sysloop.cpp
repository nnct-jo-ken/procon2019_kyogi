#include "Sysloop.h"

void game_loop(
	Game& game, 
	Renderer& renderer, 
	std::queue<ACT_STATE>& queue,
	std::mutex& mtx, 
	std::atomic<bool>& restart, 
	std::atomic<bool>& update_turn)
{
	while (System::Update())
	{
		if (restart.load())
		{
			restart.store(false, std::memory_order_seq_cst);

			mtx.lock();
			game.clear();
			renderer.clear();

			game.init();
			renderer.init(game.getBoardState());
			mtx.unlock();
		}
	
		if (System::GetPreviousEvent() == WindowEvent::CloseButton)
		{
			System::Exit();
		}

		if (KeyEnter.down()) 
		{
			update_turn.store(true, std::memory_order_seq_cst);
		}

		if (update_turn.load())
		{
			update_turn.store(false, std::memory_order_seq_cst);
			mtx.lock();
			game.updateTurn();
			mtx.unlock();
			renderer.updateTurn(game.getBoardState());
		}

		game.load_queue(queue, mtx);
		mtx.lock();
		renderer.update(game.getAgentVector());
		mtx.unlock();
		Sleep(10);
	}
}

void server_loop(
	Server& server, 
	Game& game, 
	std::queue<ACT_STATE>& queue, 
	std::mutex& mtx, 
	std::atomic<bool>& restart, 
	std::atomic<bool>& update_turn)
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
				mtx.lock();
				server.wait_cmd(game.getBoardState(), i, restart, update_turn);
				mtx.unlock();
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

void render_loop(
	Game& game,
	Renderer& renderer,
	std::mutex& mtx,
	std::atomic<bool>& restart,
	std::atomic<bool>& update_turn)
{

}