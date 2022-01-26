#include "timer.h"

timer::timer() {
	_stopped = false;

	_lastUpdate = SDL_GetTicks();
	_time = 0;
}

void timer::start() {
	_stopped = false;
	_lastUpdate = SDL_GetTicks();
}
void timer::stop() {
	_stopped = true;
	_time += SDL_GetTicks() - _lastUpdate;
	_lastUpdate = SDL_GetTicks();
}

void timer::reset() {
	_time = 0;
	_lastUpdate = SDL_GetTicks();
}

bool timer::isStopped() {
	return _stopped;
}

int timer::getTime() {
	if (!_stopped) {
		_time += (SDL_GetTicks() - _lastUpdate);
		_lastUpdate = SDL_GetTicks();
	}
	return _time;
}

int luaHandler::LUA_TIMER_getTime(lua_State *S) {
	timer* p = *((timer**)lua_touserdata(S, 1));
	lua_pushinteger(S, p->getTime());
	return 1;
}
int luaHandler::LUA_TIMER_stop(lua_State *S) {
	timer* p = *((timer**)lua_touserdata(S, 1));
	p->stop();
	return 0;
}
int luaHandler::LUA_TIMER_start(lua_State *S) {
	timer* p = *((timer**)lua_touserdata(S, 1));
	p->start();
	return 0;
}
int luaHandler::LUA_TIMER_reset(lua_State *S) {
	timer* p = *((timer**)lua_touserdata(S, 1));
	p->reset();
	return 0;
}
int luaHandler::LUA_TIMER_isStopped(lua_State *S) {
	timer* p = *((timer**)lua_touserdata(S, 1));
	lua_pushboolean(S, p->isStopped());
	return 1;
}

void luaHandler::_registerTimerFunction() {
	luaL_Reg timerlib_m[] = {
		{ "getTime", LUA_TIMER_getTime },
		{ "stop", LUA_TIMER_stop },
		{ "start", LUA_TIMER_start },
		{ "reset", LUA_TIMER_reset },
		{ "isStopped", LUA_TIMER_isStopped },
		{ NULL, NULL }
	};

	luaL_newmetatable(_S, "timer_m");

	lua_pushstring(_S, "__index");
	lua_pushvalue(_S, -2);  /* pushes the metatable*/
	lua_settable(_S, -3);  /* metatable.__index = metatable */

	luaL_setfuncs(_S, timerlib_m, 0);
}