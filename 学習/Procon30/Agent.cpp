#include "Agent.h"

Agent::Agent()
{
	pos = Vector2(0, 0);
	target_pos = Vector2(0, 0);
	act_type = 0;
	team = 0;
	done_butting = false;
	done_bad_act = false;
}

Agent::Agent(Vector2 _pos, int _team, int _ID)
{
	pos = _pos;
	target_pos = Vector2(0, 0);
	act_type = 0;
	if (_team != 1 && _team != 2) {
		throw std::invalid_argument("Agent constructor exception.");
	}
	team = _team;
	ID = _ID;
	done_butting = false;
	done_bad_act = false;
}

void Agent::setPos(Vector2 _pos) {
	if (_pos.x < 0 || _pos.y < 0) {
		throw std::invalid_argument("Agent::setPos() exception.");
	}

	pos = _pos;
}

void Agent::setDeltaMove(Vector2 _step) {
	if (abs(_step.x) > 1 || abs(_step.y) > 1) {
		throw std::invalid_argument("Agent::setTarget() exception.");
	}

	target_pos = pos + _step;
}

void Agent::setActType(int _state) {
	if (_state == 0 || _state == 1 || _state == 2) {
		act_type = _state;
	}
	else {
		throw "Agent::setActState() error. Value must be 0 or 1 or 2.";
	}
}

void Agent::setTeam(int _team) {
	if (_team != 1 && _team != 2) {
		throw std::invalid_argument("Agent::setTeam() exception.");
	}
	team = _team;
}

void Agent::setButting(bool is_butting) { done_butting = is_butting; }

void Agent::setBadAct(bool bad_act) { done_bad_act = bad_act; }

Vector2 Agent::getPos() { return pos; }

Vector2 Agent::getTarget() { return target_pos; }

int Agent::getActType() { return act_type; }

int Agent::getTeam() { return team; }

int Agent::getID() { return ID; }

bool Agent::getButting() { return done_butting; }

bool Agent::getDoneBadAct() { return done_bad_act; }

void Agent::resetAct() {
	target_pos = pos;
	act_type = 0;
}
