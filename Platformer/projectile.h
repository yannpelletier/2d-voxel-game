#pragma once
#include "userData.h"
#include "itemDefinition.h"
#include "boost/function.hpp"
#include "entity.h"
#include "luaHandler.h"
#include <Gengine/spriteBatch.h>
#include <Gengine/spriteSheet.h>
#include <Gengine/timing.h>

class world;

class projectile: public entity
{
	friend class luaHandler;
public:
	void create(std::string nameID, glm::vec2 position, glm::vec2 throwForce, agent* thrower);
	void destroy(world* world);

	void setAnimation(std::string animName, int loops);

	glm::vec2* getPositionPointer();
	float* getAnglePointer();

	void update();
	void draw(gengine::spriteBatch* spriteBatch);

	void startContact(userData* collidedWith, userData* thisFixture, b2Contact* contact, world* world);

	void LUA_PROJECTILE_onCreate();
	void LUA_PROJECTILE_update();
	void LUA_PROJECTILE_startContact(userData* collidedWith, userData* thisFixture, b2Contact* contact, world* world);

private:
	std::map<std::string, std::pair<void*, luaVariableType>> LUA_cppvars;
	std::string _nameID;
	gengine::spriteSheet _texture;
	agent* _thrower;
	int _lightID = -1;
	int _particleEmitterID = -1;

	bool _justCreated = true;

	world* _world;
};

