#include "Agent.h"

Agent::Agent()
{
	pos = Vector2(0, 0);
	target_pos = Vector2(0, 0);
	act_state = 0;
	team = 0;
}

Agent::Agent(Vector2 _pos, int _team)
{
	pos = _pos;
	target_pos = Vector2(0, 0);
	act_state = 0;
	if (_team != 1 && _team != 2) {
		throw std::invalid_argument("Agent constructor exception.");
	}
	team = _team;
}

void Agent::setPos(Vector2 _pos) {
	if (_pos.x < 0 || _pos.y < 0) {
		throw std::invalid_argument("Agent::setPos() exception.");
	}

	pos = _pos;
}

void Agent::setTarget(Vector2 _step) {
	if (abs(_step.x) > 1 || abs(_step.y) > 1) {
		throw std::invalid_argument("Agent::setTarget() exception.");
	}

	target_pos = pos + _step;
}

void Agent::setActState(int _state) {
	if (_state == 0 || _state == 1 || _state == 2) {
		act_state = _state;
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

Vector2 Agent::getPos() {
	return pos;
}

Vector2 Agent::getTarget() {
	return target_pos;
}

int Agent::getActState() {
	return act_state;
}

int Agent::getTeam() {
	return team;
}

void Agent::resetAct() {
	target_pos = pos;
	act_state = 0;
}
