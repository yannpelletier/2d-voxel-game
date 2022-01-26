#include "event.h"
#include "world.h"

event::event(std::string nameID, int duration) {
	_nameID = nameID;
	_duration = duration;

	LUA_EVENT_whenActivated();
}

int event::getOccuranceTime() {
	return _durationTimer.getTime();
}

int event::getDuration() {
	return _duration;
}

void event::update() {
	int occuranceTime = getOccuranceTime();
	
	if (occuranceTime > _duration && _duration != 0) { //Is occuring regularly
		LUA_EVENT_whenDisactivated();

		if (!LUA_cppvars.empty()) {
			for (auto it : LUA_cppvars) {
				free(it.second.first);
			}
			LUA_cppvars.clear();
		}

		luaHandler::getWorld()->deleteEvent(getNameID());
	}
	else { //Ended
		LUA_EVENT_whileActive();
	}
}

int luaHandler::LUA_EVENT_newEvent(lua_State *S) {
	std::string eventName = lua_tostring(S, 1);

	if (!_verifyLuaName(eventName)) {
		return 0;
	}

	lua_getglobal(S, eventName.c_str());
	lua_pushstring(S, "setProperties");
	lua_gettable(S, -2);
	lua_pushvalue(S, -2);
	lua_pcall(S, 1, 0, 0);
	lua_pop(S, 1);

	return 0;
}

void event::LUA_EVENT_whenActivated() {
	lua_State* S = luaHandler::getLuaState();
	lua_getglobal(S, _nameID.c_str());		//"_nameID"
	lua_pushstring(S, "whenActivated");		//"whileOccuring", "_nameID"
	lua_gettable(S, -2);					//function update, "_nameID"
	lua_pushvalue(S, -2);					// "self", function update, "_nameID"
	lua_pushlightuserdata(S, this);
	luaL_getmetatable(S, "event_m");
	lua_setmetatable(S, -2);
	lua_pcall(S, 2, 0, 0);
	lua_pop(S, 1);
}

void event::LUA_EVENT_whileActive() {
	int initDuration = luaHandler::getTableInt(_nameID, "initDuration");

	lua_State* S = luaHandler::getLuaState();
	lua_getglobal(S, _nameID.c_str());		//"_nameID"
	lua_pushstring(S, "whileActive");		//"whileOccuring", "_nameID"
	lua_gettable(S, -2);					//function update, "_nameID"
	lua_pushvalue(S, -2);					// "self", function update, "_nameID"
	lua_pushlightuserdata(S, this);
	luaL_getmetatable(S, "event_m");
	lua_setmetatable(S, -2);
	lua_pcall(S, 2, 0, 0);
	lua_pop(S, 1);
}

void event::LUA_EVENT_whenDisactivated() {
	lua_State* S = luaHandler::getLuaState();
	lua_getglobal(S, _nameID.c_str());		//"_nameID"
	lua_pushstring(S, "whenDisactivated");		//"whileOccuring", "_nameID"
	lua_gettable(S, -2);					//function update, "_nameID"
	lua_pushvalue(S, -2);					// "self", function update, "_nameID"
	lua_pushlightuserdata(S, this);
	luaL_getmetatable(S, "event_m");
	lua_setmetatable(S, -2);
	lua_pcall(S, 2, 0, 0);
	lua_pop(S, 1);
}

int luaHandler::LUA_EVENT_getDuration(lua_State *S) {
	event* p = (event*)lua_touserdata(S, 1);

	lua_pushinteger(S, p->getDuration());

	return 1;
}

int luaHandler::LUA_EVENT_getOccuranceTime(lua_State *S) {
	event* p = (event*)lua_touserdata(S, 1);

	lua_pushinteger(S, p->getOccuranceTime());
	
	return 1;
}

int luaHandler::LUA_EVENT_addInt(lua_State *S) {
	event* p = (event*)lua_touserdata(S, 1);

	if (lua_isinteger(S, 3)) {
		auto iterator = p->LUA_cppvars.find(lua_tostring(S, 2));

		if (iterator == p->LUA_cppvars.end()) {
			void* newInt = malloc(sizeof(lua_tointeger(S, 3)));
			*(int*)newInt = lua_tointeger(S, 3);

			p->LUA_cppvars.insert(std::make_pair(lua_tostring(S, 2), std::make_pair((void*)newInt, luaVariableType::INTL)));
		}
	}

	return 0;
}
int luaHandler::LUA_EVENT_addFloat(lua_State *S) {
	event* p = (event*)lua_touserdata(S, 1);

	if (lua_isnumber(S, 3)) {
		auto iterator = p->LUA_cppvars.find(lua_tostring(S, 2));

		if (iterator == p->LUA_cppvars.end()) {
			void* newFloat = malloc(sizeof(lua_tonumber(S, 3)));
			*(float*)newFloat = lua_tonumber(S, 3);

			p->LUA_cppvars.insert(std::make_pair(lua_tostring(S, 2), std::make_pair((void*)newFloat, luaVariableType::FLOATL)));
		}
	}

	return 0;
}
int luaHandler::LUA_EVENT_addBool(lua_State *S) {
	event* p = (event*)lua_touserdata(S, 1);

	auto iterator = p->LUA_cppvars.find(lua_tostring(S, 2));

	if (lua_isboolean(S, 3)) {
		if (iterator == p->LUA_cppvars.end()) {
			void* newBool = malloc(sizeof(lua_toboolean(S, 3)));
			*(bool*)newBool = lua_toboolean(S, 3);

			p->LUA_cppvars.insert(std::make_pair(lua_tostring(S, 2), std::make_pair((void*)newBool, luaVariableType::BOOLL)));
		}
	}

	return 0;
}

int luaHandler::LUA_EVENT_addTimer(lua_State *S) {
	event* p = (event*)lua_touserdata(S, 1);

	auto iterator = p->LUA_cppvars.find(lua_tostring(S, 2));

	if (iterator == p->LUA_cppvars.end()) {
		void* newTimerPtr = new timer;

		p->LUA_cppvars.insert(std::make_pair(lua_tostring(S, 2), std::make_pair((void*)newTimerPtr, luaVariableType::TIMERL)));
	}

	return 0;
}

int luaHandler::LUA_EVENT_setValue(lua_State *S) {
	event* p = (event*)lua_touserdata(S, 1);
	std::string name = lua_tostring(S, 2);

	auto iterator = p->LUA_cppvars.find(name);

	//Checks if the value exists
	if (iterator != p->LUA_cppvars.end()) {
		//Checks the type of the value, and if the value pasted as the 2nd parameter fits the type of the value
		if (p->LUA_cppvars[name].second == luaVariableType::INTL) {
			if (lua_isinteger(S, 3)) {
				*((int*)(p->LUA_cppvars[name].first)) = lua_tointeger(S, 3);
			}
		}
		else if (p->LUA_cppvars[name].second == luaVariableType::FLOATL) {
			if (lua_isnumber(S, 3)) {
				*((float*)(p->LUA_cppvars[name].first)) = lua_tonumber(S, 3);
			}
		}
		else if (p->LUA_cppvars[name].second == luaVariableType::BOOLL) {
			if (lua_isboolean(S, 3)) {
				*((bool*)(p->LUA_cppvars[name].first)) = lua_toboolean(S, 3);
			}
		}
	}

	return 0;
}

int luaHandler::LUA_EVENT_getValue(lua_State *S) {
	event* p = (event*)lua_touserdata(S, 1);
	std::string name = lua_tostring(S, 2);
	if (p->LUA_cppvars[name].second == luaVariableType::INTL) {
		lua_pushinteger(S, *(int*)(p->LUA_cppvars[lua_tostring(S, 2)].first));
		return 1;
	}
	else if (p->LUA_cppvars[name].second == luaVariableType::FLOATL) {
		lua_pushnumber(S, *(float*)(p->LUA_cppvars[lua_tostring(S, 2)].first));
		return 1;
	}
	else if (p->LUA_cppvars[name].second == luaVariableType::BOOLL) {
		lua_pushboolean(S, *(bool*)(p->LUA_cppvars[lua_tostring(S, 2)].first));
		return 1;
	}
	else if (p->LUA_cppvars[name].second == luaVariableType::TIMERL) {
		timer* pe = (timer*)(p->LUA_cppvars[lua_tostring(S, 2)].first);
		timer** addedLightUserdata = (timer**)lua_newuserdata(S, sizeof(timer*));
		*addedLightUserdata = pe;
		luaL_getmetatable(S, "timer_m");
		lua_setmetatable(S, -2);
		return 1;
	}

}

void luaHandler::_registerEventFunction() {
	luaL_Reg eventlib_m[] = {
		{ "getDuration", LUA_EVENT_getDuration },
		{ "getOccuranceTime", LUA_EVENT_getOccuranceTime},
		{ "addInt", LUA_EVENT_addInt },
		{ "addNum", LUA_EVENT_addFloat },
		{ "addBool", LUA_EVENT_addBool },
		{ "addTimer", LUA_EVENT_addTimer },
		{ "setValue", LUA_EVENT_setValue },
		{ "getValue", LUA_EVENT_getValue },
		{ NULL, NULL }
	};

	luaL_newmetatable(_S, "event_m");

	lua_pushstring(_S, "__index");
	lua_pushvalue(_S, -2);  /* pushes the metatable*/
	lua_settable(_S, -3);  /* metatable.__index = metatable */

	luaL_setfuncs(_S, eventlib_m, 0);
}
