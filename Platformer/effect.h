#pragma once
#include "timer.h"
#include "body.h"
#include "luaHandler.h"
#include "globals.h"
#include "entity.h"

class effect: public entity
{
public:
	friend class luaHandler;

	effect(std::string nameID, glm::vec2 position, float angle);

	void update();
	void draw(gengine::spriteBatch* spriteBatch);

	void destroy(world* world);

	void LUA_EFFECT_onCreate();
	void LUA_EFFECT_update();
private:
	std::map<std::string, std::pair<void*, luaVariableType>> LUA_cppvars;
	std::string _nameID;
	gengine::spriteSheet _texture;
	bool _justCreated = false;
};

