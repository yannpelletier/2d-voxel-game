#pragma once
#include "luaHandler.h"
#include "timer.h"

class event
{
public:
	friend class luaHandler;

	event(std::string nameID, int duration);

	int getOccuranceTime(); //Returns for how long the event has been active
	int getDuration();

	void update();

	void LUA_EVENT_whenActivated();
	void LUA_EVENT_whileActive();
	void LUA_EVENT_whenDisactivated();

	std::string getNameID() { return _nameID; }
private:
	std::string _nameID;
	int _duration = 0;
	timer _durationTimer;

	std::map<std::string, std::pair<void*, luaVariableType>> LUA_cppvars;
};

