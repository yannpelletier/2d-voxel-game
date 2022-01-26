#include "effect.h"
#include "world.h"

int luaHandler::LUA_EFFECT_newEffect(lua_State *S) {
	std::string effectName = lua_tostring(S, 1);

	if (!_verifyLuaName(effectName)) {
		return 0;
	}

	lua_getglobal(S, effectName.c_str());
	lua_pushstring(S, "setProperties");
	lua_gettable(S, -2);
	lua_pushvalue(S, -2);
	lua_pcall(S, 1, 0, 0);
	lua_pop(S, 1);

	return 0;
}

effect::effect(std::string nameID, glm::vec2 position, float angle){
	_nameID = nameID;

	lua_State *S = luaHandler::getLuaState();

	gengine::GLTexture textureID = gengine::ressourceManager::getTexture(luaHandler::getPath(luaHandler::getTableString(_nameID, "texture")));
	_texture.init(textureID, luaHandler::getTableRange(_nameID, "spritesheetSize"));

	//Add animations to texture
	lua_getglobal(S, nameID.c_str());		// tableName
	lua_pushstring(S, "animations");			// "animations", tableName
	lua_gettable(S, -2);						// table animations, tableName
	if (lua_istable(S, -1)) {
		lua_pushnil(S);							// nil, table animations, tableName
		while (lua_next(S, -2) != 0) {			// table, key, table animations, tableName
			lua_pushnumber(S, 1);				// 1, table, key, table animations, tableName
			lua_gettable(S, -2);				// string, table, key, table animations, tableName
			std::string animName = lua_tostring(S, -1);

			lua_pushnumber(S, 2);
			lua_gettable(S, -3);				// int, string, table, key, table animations, tableName
			int beginIndex = lua_tointeger(S, -1);

			lua_pushnumber(S, 3);
			lua_gettable(S, -4);				// int, int, string, table, key, table animations, tableName
			int endIndex = lua_tointeger(S, -1);

			lua_pushnumber(S, 4);
			lua_gettable(S, -5);				// int, int, int, string, table, key, table animations, tableName
			int rate = lua_tointeger(S, -1);

			_texture.addAnimation(animName, beginIndex, endIndex, rate);
			lua_pop(S, 5);						// key, table animations, tableName
		}
		lua_pop(S, 3);
	}

	_body = new body();

	b2BodyDef bodyDef;
	bodyDef.fixedRotation = true;
	bodyDef.type = b2_dynamicBody;
	bodyDef.gravityScale = 0;
	bodyDef.position.Set(position.x, position.y);
	bodyDef.angle = angle;

	glm::vec2  effectSize = luaHandler::getTableRange(_nameID, "size");
	b2PolygonShape box = _body->makeBox(glm::vec2(globals::pixToMet(effectSize.x), globals::pixToMet(effectSize.y)));

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &box;
	fixtureDef.isSensor = true;
	fixtureDef.userData = new userData(this, gameObjectType::PROJECTILE);
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.0001f;
	fixtureDef.filter.categoryBits = globals::bitData::PROJECTILE_BIT;
	fixtureDef.filter.maskBits = globals::bitData::NPC_BIT | globals::bitData::TILE_BIT | globals::bitData::PLAYER_BIT | globals::bitData::SOLID_BODY_BIT | globals::bitData::PLATFORM_BIT;

	_body->createBody(luaHandler::getWorld(), bodyDef, fixtureDef, b2Vec2(0,0));
	luaHandler::getWorld()->addEntity(this);
}

void effect::update() {
	if (_justCreated == false) {
		LUA_EFFECT_onCreate();
		_justCreated = true;
	}

	_texture.update();
	LUA_EFFECT_update();
}

void effect::destroy(world* world) {
	if (_body != nullptr) {
		_body->destroy(world);
		_body = nullptr;
		world->deleteEntity(this);
	}
}

void effect::draw(gengine::spriteBatch* spriteBatch) {
	if (_body != nullptr) {
		glm::vec4 destRect;
		destRect.x = _body->getPosition().x - _body->getDimensions().x / 2.0f;
		destRect.y = _body->getPosition().y - _body->getDimensions().y / 2.0f;
		destRect.z = _body->getDimensions().x;
		destRect.w = _body->getDimensions().y;
		spriteBatch->draw(destRect, _texture.getAnimUV(), _texture.getTexture()->id, 8, gengine::color(255, 255, 255, 255), _body->getAngleRad());
	}
}

void effect::LUA_EFFECT_onCreate() {
	lua_State* S = luaHandler::getLuaState();
	lua_getglobal(S, _nameID.c_str());		//"_nameID"
	lua_pushstring(S, "onCreate");		//"whileOccuring", "_nameID"
	lua_gettable(S, -2);					//function update, "_nameID"
	lua_pushvalue(S, -2);					// "self", function update, "_nameID"
	lua_pushlightuserdata(S, this);
	luaL_getmetatable(S, "effect_m");
	lua_setmetatable(S, -2);
	lua_pcall(S, 2, 0, 0);
	lua_pop(S, 1);
}

void effect::LUA_EFFECT_update() {
	lua_State* S = luaHandler::getLuaState();
	lua_getglobal(S, _nameID.c_str());		//"_nameID"
	lua_pushstring(S, "update");		//"whileOccuring", "_nameID"
	lua_gettable(S, -2);					//function update, "_nameID"
	lua_pushvalue(S, -2);					// "self", function update, "_nameID"
	lua_pushlightuserdata(S, this);
	luaL_getmetatable(S, "effect_m");
	lua_setmetatable(S, -2);
	lua_pcall(S, 2, 0, 0);
	lua_pop(S, 1);
}

int luaHandler::LUA_EFFECT_addInt(lua_State *S) {
	effect* p = (effect*)lua_touserdata(S, 1);

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
int luaHandler::LUA_EFFECT_addFloat(lua_State *S) {
	effect* p = (effect*)lua_touserdata(S, 1);

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
int luaHandler::LUA_EFFECT_addBool(lua_State *S) {
	effect* p = (effect*)lua_touserdata(S, 1);

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

int luaHandler::LUA_EFFECT_addTimer(lua_State *S) {
	effect* p = (effect*)lua_touserdata(S, 1);

	auto iterator = p->LUA_cppvars.find(lua_tostring(S, 2));

	if (iterator == p->LUA_cppvars.end()) {
		void* newTimerPtr = new timer;

		p->LUA_cppvars.insert(std::make_pair(lua_tostring(S, 2), std::make_pair((void*)newTimerPtr, luaVariableType::TIMERL)));
	}

	return 0;
}

int luaHandler::LUA_EFFECT_setValue(lua_State *S) {
	effect* p = (effect*)lua_touserdata(S, 1);
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

int luaHandler::LUA_EFFECT_getValue(lua_State *S) {
	effect* p = (effect*)lua_touserdata(S, 1);
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

int luaHandler::LUA_EFFECT_destroy(lua_State *S) {
	effect* p = (effect*)lua_touserdata(S, 1);

	p->destroy(luaHandler::getWorld());

	return 0;
}

void luaHandler::_registerEffectFunction() {
	luaL_Reg eventlib_m[] = {
		{ "addInt", LUA_EFFECT_addInt },
		{ "addNum", LUA_EFFECT_addFloat },
		{ "addBool", LUA_EFFECT_addBool },
		{ "addTimer", LUA_EFFECT_addTimer },
		{ "setValue", LUA_EFFECT_setValue },
		{ "getValue", LUA_EFFECT_getValue },
		{ "destroy", LUA_EFFECT_destroy },
		{ NULL, NULL }
	};

	luaL_newmetatable(_S, "effect_m");

	lua_pushstring(_S, "__index");
	lua_pushvalue(_S, -2);  /* pushes the metatable*/
	lua_settable(_S, -3);  /* metatable.__index = metatable */

	luaL_setfuncs(_S, eventlib_m, 0);
}
