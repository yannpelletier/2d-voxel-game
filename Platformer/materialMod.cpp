#include "materialMod.h"
#include "tile.h"


materialMod::materialMod()
{
	_definitionType = definitionType::MATERIAL_MOD;
}


materialMod::~materialMod()
{
}

bool materialMod::hasAutoTiling()
{
	return _autoTiling;
}

void materialMod::init(std::string texture, glm::vec2 spritesheetSize, bool autoTile, int hp) {
	_texture.init(gengine::ressourceManager::getTexture(texture), spritesheetSize);
	_autoTiling = autoTile;
	_hp = hp;
}

void materialMod::addAnimation(std::string animationName, int beginIndex, int endIndex, int speedBetweenFrames) {
	_texture.addAnimation(animationName, beginIndex, endIndex, speedBetweenFrames);
}

gengine::spriteSheet materialMod::getTexture()
{
	return _texture;
}

void materialMod::LUA_onPlaced(materialModInstance* modInstance, tile* onTile) {
	lua_State* S = luaHandler::getLuaState();
	lua_getglobal(S, _nameID.c_str());
	lua_pushstring(S, "onPlaced");
	lua_gettable(S, -2);
	lua_pushvalue(S, -2);
	lua_pushlightuserdata(S, modInstance);
	luaL_getmetatable(S, "materialMod_m");
	lua_setmetatable(S, -2);

	tile** addedLightUserdata = (tile**)lua_newuserdata(S, sizeof(tile*));
	*addedLightUserdata = onTile;
	luaL_getmetatable(S, "tile_m");
	lua_setmetatable(S, -2);

	lua_pcall(S, 3, 0, 0);
	lua_pop(S, 1);
}
void materialMod::LUA_update(materialModInstance* modInstance, tile* onTile) {
	lua_State* S = luaHandler::getLuaState();
	lua_getglobal(S, _nameID.c_str());
	lua_pushstring(S, "update");
	lua_gettable(S, -2);
	lua_pushvalue(S, -2);
	lua_pushlightuserdata(S, modInstance);
	luaL_getmetatable(S, "materialMod_m");
	lua_setmetatable(S, -2);

	tile** addedLightUserdata = (tile**)lua_newuserdata(S, sizeof(tile*));
	*addedLightUserdata = onTile;
	luaL_getmetatable(S, "tile_m");
	lua_setmetatable(S, -2);

	lua_call(S, 3, 0, 0);
	lua_pop(S, 1);
}
void materialMod::LUA_onDestroy(materialModInstance* modInstance, tile* onTile) {
	lua_State* S = luaHandler::getLuaState();
	lua_getglobal(S, _nameID.c_str());
	lua_pushstring(S, "onDestroy");
	lua_gettable(S, -2);
	lua_pushvalue(S, -2);
	lua_pushlightuserdata(S, modInstance);
	luaL_getmetatable(S, "materialMod_m");
	lua_setmetatable(S, -2);

	tile** addedLightUserdata = (tile**)lua_newuserdata(S, sizeof(tile*));
	*addedLightUserdata = onTile;
	luaL_getmetatable(S, "tile_m");
	lua_setmetatable(S, -2);

	lua_pcall(S, 3, 0, 0);
	lua_pop(S, 1);
}

int luaHandler::LUA_MATERIAL_MOD_newMaterialMod(lua_State *S) {
	std::string materialName = lua_tostring(S, 1); //Get name of the table

	if (!_verifyLuaName(materialName)) {
		return 0;
	}

	lua_getglobal(S, materialName.c_str()); // "ground"
	lua_pushstring(S, "setProperties");		// "setProperties", "ground"
	lua_gettable(S, -2);					//	function setProperties, "ground"
	lua_pushvalue(S, -2);					//  "ground", function setProperties, "ground"
	lua_call(S, 1, 0);
	lua_pop(S, 1);

	materialMod* newMaterial = new materialMod();
	newMaterial->_nameID = materialName;
	newMaterial->init(getPath(getTableString(materialName, "texture")), getTableRange(materialName, "spritesheetSize"), getTableBool(materialName, "autoTile"), getTableInt(materialName, "hitpoints"));
	_definitions.emplace(std::make_pair(materialName, newMaterial));

	//Add animations to texture
	lua_getglobal(S, materialName.c_str());		// tableName
	lua_pushstring(S, "animations");			// "animations", tableName
	lua_gettable(S, -2);						// table animations, tableName
	if (lua_istable(S, -1)) {
		lua_pushnil(S);							// nil, table animations, tableName
		while (lua_next(S, -2) != 0) {			// table, key, table animations, tableName
			lua_pushnumber(S, 1);				// 1, table, key, table animations, tableName
			lua_gettable(S, -2);				// string, table, key, table animations, tableName
			std::string animName = lua_tostring(_S, -1);

			lua_pushnumber(S, 2);
			lua_gettable(S, -3);				// int, string, table, key, table animations, tableName
			int beginIndex = lua_tointeger(_S, -1);

			lua_pushnumber(S, 3);
			lua_gettable(S, -4);				// int, int, string, table, key, table animations, tableName
			int endIndex = lua_tointeger(_S, -1);

			lua_pushnumber(S, 4);
			lua_gettable(S, -5);				// int, int, int, string, table, key, table animations, tableName
			int rate = lua_tointeger(_S, -1);

			newMaterial->addAnimation(animName, beginIndex, endIndex, rate);
			lua_pop(S, 5);						// key, table animations, tableName
		}
		lua_pop(S, 3);
	}

	lua_pop(S, 1);

	return 0;
}

int luaHandler::LUA_MATERIAL_MOD_setAnimation(lua_State *S) {
	materialModInstance* p = (materialModInstance*)lua_touserdata(S, 1);

	p->setAnimation(lua_tostring(S, 2));

	return 0;
}

int luaHandler::LUA_MATERIAL_MOD_addInt(lua_State *S) {
	materialModInstance* p = (materialModInstance*)lua_touserdata(S, 1);

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
int luaHandler::LUA_MATERIAL_MOD_addFloat(lua_State *S) {
	materialModInstance* p = (materialModInstance*)lua_touserdata(S, 1);

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
int luaHandler::LUA_MATERIAL_MOD_addBool(lua_State *S) {
	materialModInstance* p = (materialModInstance*)lua_touserdata(S, 1);

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

int luaHandler::LUA_MATERIAL_MOD_addTimer(lua_State *S) {
	materialModInstance* p = (materialModInstance*)lua_touserdata(S, 1);

	auto iterator = p->LUA_cppvars.find(lua_tostring(S, 2));

	if (iterator == p->LUA_cppvars.end()) {
		void* newTimerPtr = new timer;

		p->LUA_cppvars.insert(std::make_pair(lua_tostring(S, 2), std::make_pair((void*)newTimerPtr, luaVariableType::TIMERL)));
	}

	return 0;
}

int luaHandler::LUA_MATERIAL_MOD_setValue(lua_State *S) {
	materialModInstance* p = (materialModInstance*)lua_touserdata(S, 1);
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

int luaHandler::LUA_MATERIAL_MOD_getValue(lua_State *S) {
	materialModInstance* p = (materialModInstance*)lua_touserdata(S, 1);
	std::string name = lua_tostring(S, 2);

	auto iterator = p->LUA_cppvars.find(name);

	//Checks if the value exists
	if (iterator != p->LUA_cppvars.end()) {
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
	else {
		lua_pushnumber(S, 0);
	}



}


void luaHandler::_registerMaterialModFunction() {
	luaL_Reg particlelib_m[] = {
		{ "setAnimation", LUA_MATERIAL_MOD_setAnimation },
		{ "addInt", LUA_MATERIAL_MOD_addInt },
		{ "addNum", LUA_MATERIAL_MOD_addFloat },
		{ "addBool", LUA_MATERIAL_MOD_addBool },
		{ "addTimer", LUA_MATERIAL_MOD_addTimer },
		{ "getValue", LUA_MATERIAL_MOD_getValue },
		{ "setValue", LUA_MATERIAL_MOD_setValue },
		{ NULL, NULL }
	};

	luaL_newmetatable(_S, "materialMod_m");

	lua_pushstring(_S, "__index");
	lua_pushvalue(_S, -2);  /* pushes the metatable*/
	lua_settable(_S, -3);  /* metatable.__index = metatable */

	luaL_setfuncs(_S, particlelib_m, 0);
}
