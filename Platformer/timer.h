#pragma once
#include "luaHandler.h"

//Will be used by lua objects
class timer
{
public:
	timer();

	void start();
	void stop();

	void reset();

	int getTime();

	bool isStopped();
private:
	bool _stopped;

	int _lastUpdate;
	int _time;
};

