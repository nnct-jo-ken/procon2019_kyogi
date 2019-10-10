#include "Agent.h"

Agent::Agent()
{
	pos = Vector2(0, 0);
	delta_pos = Vector2(0, 0);
	act_type = 0;
	team = 0;
	done_butting = false;
	done_bad_act = false;
}

Agent::Agent(Vector2 _pos, int _team, int _ID)
{
	pos = _pos;
	delta_pos = Vector2(0, 0);
	act_type = 0;
	if (_team != 1 && _team != 2) {
		throw std::invalid_argument("Agent constructor exception.");
	}
	team = _team;
	ID = _ID;
	done_butting = false;
	done_bad_act = false;
}

void Agent::resetAct() {
	delta_pos = Vector2(0, 0);
	act_type = 0;
}
