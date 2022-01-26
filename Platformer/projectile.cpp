#include "projectile.h"
#include "world.h"
#include "inventoryItem.h"
#include <Gengine/timing.h>
#include <math.h>
#include "timer.h"
#include "tileBatch.h"

void projectile::create(std::string nameID, glm::vec2 position, glm::vec2 throwForce, agent* thrower) {
	_nameID = nameID;
	_world = luaHandler::getWorld();
	_thrower = thrower;

	lua_State *S = luaHandler::getLuaState();

	gengine::GLTexture textureID = gengine::ressourceManager::getTexture(luaHandler::getPath(luaHandler::getTableString(_nameID, "texture")));
	_texture.init(textureID, luaHandler::getTableRange(_nameID,"spritesheetSize"));

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
	bodyDef.fixedRotation = false;
	bodyDef.type = b2_dynamicBody;
	bodyDef.angularDamping = 0;
	bodyDef.position.Set(position.x, position.y);

	glm::vec2  projectileSize = luaHandler::getTableRange(_nameID, "size");
	b2PolygonShape box = _body->makeBox(glm::vec2(globals::pixToMet(projectileSize.x), globals::pixToMet(projectileSize.y)));

	bool sensor = luaHandler::getTableBool(nameID, "sensor");

	if (sensor) {
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &box;
		fixtureDef.isSensor = true;
		fixtureDef.userData = new userData(this, gameObjectType::PROJECTILE);
		fixtureDef.density = luaHandler::getTableFloat(nameID, "density");
		fixtureDef.friction = 0;
		fixtureDef.filter.categoryBits = globals::bitData::PROJECTILE_BIT;
		fixtureDef.filter.maskBits = globals::bitData::TILE_BIT | globals::bitData::SOLID_BODY_BIT | globals::bitData::PLATFORM_BIT | globals::bitData::PLAYER_BIT | globals::bitData::NPC_BIT;
		_body->createBody(_world, bodyDef, fixtureDef, b2Vec2(throwForce.x, throwForce.y));
	}
	else {
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &box;
		fixtureDef.isSensor = false;
		fixtureDef.userData = new userData(this, gameObjectType::PROJECTILE);
		fixtureDef.density = luaHandler::getTableFloat(nameID, "density");
		fixtureDef.friction = luaHandler::getTableFloat(nameID, "friction");
		fixtureDef.filter.categoryBits = globals::bitData::PROJECTILE_BIT;
		fixtureDef.filter.maskBits = globals::bitData::TILE_BIT | globals::bitData::SOLID_BODY_BIT | globals::bitData::PLATFORM_BIT;
		fixtureDef.filter.groupIndex = -1;
		fixtureDef.restitution = 0;
		_body->createBody(_world, bodyDef, fixtureDef, b2Vec2(throwForce.x, throwForce.y));
	}

	_world->addEntity(this);
}

void projectile::destroy(world* world) {
	if (_body != nullptr) {
		if (_lightID != -1) {
			luaHandler::getWorld()->deleteLightAndOrder(_lightID);
			_lightID = -1;
		}
		if (_particleEmitterID != -1) {
			luaHandler::getWorld()->deleteParticleEmitter(_particleEmitterID);
			_particleEmitterID = -1;
		}
		if (!LUA_cppvars.empty()) {
			for (auto it : LUA_cppvars) {
				free(it.second.first);
			}
			LUA_cppvars.clear();
		}
		_body->destroy(world);
		_body = nullptr;
		_world->deleteEntity(this);
	}
}

void projectile::setAnimation(std::string animName, int loops) {
	_texture.changeAnimation(animName);
	_texture.setLoops(loops);
}

glm::vec2* projectile::getPositionPointer() {
	return &_position;
}

float* projectile::getAnglePointer() {
	return &_angle;
}

void projectile::update() {
	if (_justCreated == true) {
		LUA_PROJECTILE_onCreate();
		_justCreated = false;
	}

	if (_body->getLinearVelocity().y > 0) {
		b2Filter projectileFixture = _body->getFixture(0)->GetFilterData();
		projectileFixture.maskBits = globals::bitData::NPC_BIT | globals::bitData::TILE_BIT | globals::bitData::PLAYER_BIT | globals::bitData::SOLID_BODY_BIT;
		_body->getFixture(0)->SetFilterData(projectileFixture);
	}
	else {
		b2Filter projectileFixture = _body->getFixture(0)->GetFilterData();
		projectileFixture.maskBits = globals::bitData::NPC_BIT | globals::bitData::TILE_BIT | globals::bitData::PLAYER_BIT | globals::bitData::SOLID_BODY_BIT | globals::bitData::PLATFORM_BIT;
		_body->getFixture(0)->SetFilterData(projectileFixture);
	}

	_texture.update();

	if (_body != nullptr) {
		if (_body->getBody() != nullptr) {
			_position = _body->getPosition();
			_angle = _body->getAngleRad();
		}
	}
	LUA_PROJECTILE_update();

}

void projectile::draw(gengine::spriteBatch* spriteBatch) {
	if (_body != nullptr) {
		glm::vec4 destRect;
		destRect.x = _body->getPosition().x - _body->getDimensions().x / 2.0f;
		destRect.y = _body->getPosition().y - _body->getDimensions().y / 2.0f;
		destRect.z = _body->getDimensions().x;
		destRect.w = _body->getDimensions().y;
		spriteBatch->draw(destRect, _texture.getAnimUV(), _texture.getTexture()->id, 7, gengine::color(255, 255, 255, 255), _body->getAngleRad());
	}
}

void projectile::startContact(userData* collidedWith, userData* thisFixture, b2Contact* contact, world* world) {
	LUA_PROJECTILE_startContact(collidedWith, thisFixture, contact, world);
}

void projectile::LUA_PROJECTILE_onCreate() {
	lua_State* S = luaHandler::getLuaState();

	lua_getglobal(S, _nameID.c_str());
	lua_pushstring(S, "onCreate");
	lua_gettable(S, -2);
	lua_pushvalue(S, -2);
	lua_pushlightuserdata(S, this);
	luaL_getmetatable(S, "projectile_m");
	lua_setmetatable(S, -2);

	lua_pcall(S, 2, 0, 0);
	lua_pop(S, 1);
}


void projectile::LUA_PROJECTILE_update() {
	lua_State* S = luaHandler::getLuaState();

	lua_getglobal(S, _nameID.c_str());
	lua_pushstring(S, "update");
	lua_gettable(S, -2);
	lua_pushvalue(S, -2);
	lua_pushlightuserdata(S, this);
	luaL_getmetatable(S, "projectile_m");
	lua_setmetatable(S, -2);

	lua_pcall(S, 2, 0, 0);
	lua_pop(S, 1);
}

void projectile::LUA_PROJECTILE_startContact(userData* collidedWith, userData* thisFixture, b2Contact* contact, world* world) {
	lua_State* S = luaHandler::getLuaState();

	if (_body != nullptr) {
		if (collidedWith->type == gameObjectType::PLAYER || collidedWith->type == gameObjectType::NPC) {
			lua_getglobal(S, _nameID.c_str());
			lua_pushstring(S, "onContact");
			lua_gettable(S, -2);
			lua_pushvalue(S, -2);
			lua_pushlightuserdata(S, this);
			luaL_getmetatable(S, "projectile_m");
			lua_setmetatable(S, -2);

			if (collidedWith->dataUser == _thrower) {
				agent** addedAgentUserdata = (agent**)lua_newuserdata(S, sizeof(agent*));
				*addedAgentUserdata = static_cast<agent*>(collidedWith->dataUser);
				luaL_getmetatable(S, "agent_m");
				lua_setmetatable(S, -2);
				lua_pushstring(S, "thrower");
			}
			else {
				agent** addedAgentUserdata = (agent**)lua_newuserdata(S, sizeof(agent*));
				*addedAgentUserdata = static_cast<agent*>(collidedWith->dataUser);
				luaL_getmetatable(S, "agent_m");
				lua_setmetatable(S, -2);
				lua_pushstring(S, "agent");
			}

			lua_pcall(S, 4, 0, 0);
			lua_pop(S, 1);

		}
		else if (collidedWith->type == gameObjectType::TILE) {
			lua_getglobal(S, _nameID.c_str());
			lua_pushstring(S, "onContact");
			lua_gettable(S, -2);
			lua_pushvalue(S, -2);
			lua_pushlightuserdata(S, this);
			luaL_getmetatable(S, "projectile_m");
			lua_setmetatable(S, -2);

			/*glm::ivec2 tilePosition = static_cast<tileBatch*>(collidedWith->dataUser)->getCollisionedTile(collidedWith, thisFixture, contact, world);

			if (world->getTile(tilePosition.x, tilePosition.y) != nullptr) {
				tile** addedLightUserdata = (tile**)lua_newuserdata(S, sizeof(tile*));
				*addedLightUserdata = world->getTile(tilePosition.x, tilePosition.y);

				luaL_getmetatable(S, "tile_m");
				lua_setmetatable(S, -2);
				lua_pushstring(S, "tile");
			}
			else {
				lua_pushnil(S);
			}*/

			tile** addedLightUserdata = (tile**)lua_newuserdata(S, sizeof(tile*));
			*addedLightUserdata = static_cast<tile*>(collidedWith->dataUser);
			/*luaL_getmetatable(S, "tile_m");
			lua_setmetatable(S, -2);*/
			lua_pushstring(S, "tile");


			lua_pcall(S, 4, 0, 0);
			lua_pop(S, 1);
		}
		else if (collidedWith->type == gameObjectType::TILESLOPE) {
			lua_getglobal(S, _nameID.c_str());
			lua_pushstring(S, "onContact");
			lua_gettable(S, -2);
			lua_pushvalue(S, -2);
			lua_pushlightuserdata(S, this);
			luaL_getmetatable(S, "projectile_m");
			lua_setmetatable(S, -2);

			/*glm::ivec2 tilePosition = static_cast<tileBatch*>(collidedWith->dataUser)->getCollisionedTile(collidedWith, thisFixture, contact, world);

			if (world->getTile(tilePosition.x, tilePosition.y) != nullptr) {
			tile** addedLightUserdata = (tile**)lua_newuserdata(S, sizeof(tile*));
			*addedLightUserdata = world->getTile(tilePosition.x, tilePosition.y);

			luaL_getmetatable(S, "tile_m");
			lua_setmetatable(S, -2);
			lua_pushstring(S, "tile");
			}
			else {
			lua_pushnil(S);
			}*/

			tile** addedLightUserdata = (tile**)lua_newuserdata(S, sizeof(tile*));
			*addedLightUserdata = static_cast<tile*>(collidedWith->dataUser);
			/*luaL_getmetatable(S, "tile_m");
			lua_setmetatable(S, -2);*/
			lua_pushstring(S, "tile");


			lua_pcall(S, 4, 0, 0);
			lua_pop(S, 1);
		}
		else if (collidedWith->type == gameObjectType::PLATFORM || collidedWith->type == gameObjectType::SOLID_BODY) {
			lua_getglobal(S, _nameID.c_str());
			lua_pushstring(S, "onContact");
			lua_gettable(S, -2);
			lua_pushvalue(S, -2);
			lua_pushlightuserdata(S, this);
			luaL_getmetatable(S, "projectile_m");
			lua_setmetatable(S, -2);

			/*glm::ivec2 tilePosition = static_cast<tileBatch*>(collidedWith->dataUser)->getCollisionedTile(collidedWith, thisFixture, contact, world);

			if (world->getTile(tilePosition.x, tilePosition.y) != nullptr) {
			tile** addedLightUserdata = (tile**)lua_newuserdata(S, sizeof(tile*));
			*addedLightUserdata = world->getTile(tilePosition.x, tilePosition.y);

			luaL_getmetatable(S, "tile_m");
			lua_setmetatable(S, -2);
			lua_pushstring(S, "tile");
			}
			else {
			lua_pushnil(S);
			}*/
				
			tile** addedLightUserdata = (tile**)lua_newuserdata(S, sizeof(tile*));
			*addedLightUserdata = static_cast<tile*>(collidedWith->dataUser);
			/*luaL_getmetatable(S, "tile_m");
			lua_setmetatable(S, -2);*/
			lua_pushstring(S, "tile");


			lua_pcall(S, 4, 0, 0);
			lua_pop(S, 1);
		}

	}

}


int luaHandler::LUA_PROJECTILE_newProjectile(lua_State *S) {
	std::string projectileName = lua_tostring(S, 1);

	if (!_verifyLuaName(projectileName)) {
		return 0;
	}

	lua_getglobal(S, projectileName.c_str());
	lua_pushstring(S, "setProperties");
	lua_gettable(S, -2);
	lua_pushvalue(S, -2);
	lua_pcall(S, 1, 0, 0);
	lua_pop(S, 1);

	return 0;
}

int luaHandler::LUA_PROJECTILE_setEmitter(lua_State *S){
	projectile* p = (projectile*)lua_touserdata(S, 1);
	particleEmitter* newEmitter = new particleEmitter(lua_tostring(S, 2));
	glm::vec2  projectileSize = luaHandler::getTableRange(p->_nameID, "size");
	newEmitter->emitterPosition = p->getPositionPointer();
	newEmitter->emitterAngle = p->getAnglePointer();
	newEmitter->dimensions = glm::vec2(globals::pixToMet(projectileSize.x), globals::pixToMet(projectileSize.y));
	p->_particleEmitterID = getWorld()->addParticleEmitter(newEmitter);
	return 0;
}
int luaHandler::LUA_PROJECTILE_getEmitter(lua_State *S) {
	projectile* p = (projectile*)lua_touserdata(S, 1);
	particleEmitter** addedEmitterUserdata = (particleEmitter**)lua_newuserdata(S, sizeof(particleEmitter*));
	*addedEmitterUserdata = getWorld()->getEmitter(p->_particleEmitterID);
	luaL_getmetatable(S, "particleEmitter_m");
	lua_setmetatable(S, -2);
	return 1;
}
int luaHandler::LUA_PROJECTILE_addLight(lua_State *S) {
	projectile* p = (projectile*)lua_touserdata(S, 1);
	if (p->_lightID == -1) {
		light newLight;
		p->_lightID = luaHandler::getWorld()->addLightAndOrder(p->getPositionPointer(), newLight);
	}
	return 0;
}
int luaHandler::LUA_PROJECTILE_getLight(lua_State *S) {
	projectile* p = (projectile*)lua_touserdata(S, 1);
	if (p->_lightID != -1) {
		light** addedLightUserdata = (light**)lua_newuserdata(S, sizeof(light*));
		*addedLightUserdata = getWorld()->getLight(p->_lightID);
		luaL_getmetatable(S, "light_m");
		lua_setmetatable(S, -2);
	}
	return 1;
}
int luaHandler::LUA_PROJECTILE_setSide(lua_State *S) {
	projectile* p = (projectile*)lua_touserdata(S, 1);
	p->_texture.changeDirection(lua_tointeger(S, 2));
	return 0;
}
int luaHandler::LUA_PROJECTILE_setAngleDeg(lua_State *S) {
	projectile* p = (projectile*)lua_touserdata(S, 1);
	if (p->_body != nullptr) {
		p->getBody()->setAngleDeg(lua_tonumber(S, 2));
	}
	return 0;
}
int luaHandler::LUA_PROJECTILE_setAngleRad(lua_State *S) {
	projectile* p = (projectile*)lua_touserdata(S, 1);
	if (p->_body != nullptr) {
		p->getBody()->setAngleRad(lua_tonumber(S, 2));
	}
	return 0;
}
int luaHandler::LUA_PROJECTILE_setDirection(lua_State *S) {
	projectile* p = (projectile*)lua_touserdata(S, 1);
	b2Vec2 direction(lua_tonumber(S, 2), lua_tonumber(S, 3));
	direction.Normalize();
	float velocityStrength = b2Vec2(p->getBody()->getLinearVelocity()).Length();

	if (p->_body != nullptr) {
		p->_body->setLinearVelocity(b2Vec2(direction.x * velocityStrength, direction.y * velocityStrength));
	}
	return 0;
}
int luaHandler::LUA_PROJECTILE_setVelocity(lua_State *S) {
	projectile* p = (projectile*)lua_touserdata(S, 1);
	if (p->_body != nullptr) {
		p->_body->setLinearVelocity(b2Vec2(globals::pixToMet(lua_tonumber(S, 2)), globals::pixToMet(lua_tonumber(S, 3))));
	}
	return 0;
}
int luaHandler::LUA_PROJECTILE_setGravityScale(lua_State *S) {
	projectile* p = (projectile*)lua_touserdata(S, 1);
	if (p->_body != nullptr) {
		p->_body->getBody()->SetGravityScale(lua_tonumber(S,2));
	}
	return 0;
}
int luaHandler::LUA_PROJECTILE_setAngularVelocity(lua_State *S) {
	projectile* p = (projectile*)lua_touserdata(S, 1);
	if (p->_body != nullptr) {
		p->_body->getBody()->SetAngularVelocity(lua_tonumber(S,2));
	}
	return 0;
}
int luaHandler::LUA_PROJECTILE_setAnimation(lua_State *S) {
	projectile* p = (projectile*)lua_touserdata(S, 1);
	if (p->_body != nullptr) {
		if (lua_isnoneornil(S, 3)) {
			p->setAnimation(lua_tostring(S, 2), -1);
		}
		else {
			p->setAnimation(lua_tostring(S, 2), lua_tonumber(S, 3));
		}
	}
	return 0;
}
int luaHandler::LUA_PROJECTILE_getAngle(lua_State *S) {
	projectile* p = (projectile*)lua_touserdata(S, 1);
	if (p->_body != nullptr) {
		b2Vec2 velocity = p->getBody()->getLinearVelocity();
		lua_pushnumber(S, atan2(velocity.y, velocity.x));
	}
	return 1;
}
int luaHandler::LUA_PROJECTILE_getDirection(lua_State *S) {
	projectile* p = (projectile*)lua_touserdata(S, 1);
	b2Vec2 dir = p->getBody()->getLinearVelocity();
	dir.Normalize();

	lua_newtable(S);
	lua_pushnumber(S, 1);
	lua_pushnumber(S, dir.x);
	lua_settable(S, -3);
	lua_pushnumber(S, 2);
	lua_pushnumber(S, dir.y);
	lua_settable(S, -3);

	return 1;
}
int luaHandler::LUA_PROJECTILE_getVelocity(lua_State *S) {
	projectile* p = (projectile*)lua_touserdata(S, 1);
	b2Vec2 dir = p->getBody()->getLinearVelocity();

	lua_newtable(S);
	lua_pushnumber(S, 1);
	lua_pushnumber(S, globals::metToPix(dir.x));
	lua_settable(S, -3);
	lua_pushnumber(S, 2);
	lua_pushnumber(S, globals::metToPix(dir.y));
	lua_settable(S, -3);

	return 1;
}
int luaHandler::LUA_PROJECTILE_getAngularVelocity(lua_State *S) {
	projectile* p = (projectile*)lua_touserdata(S, 1);
	float dir = p->getBody()->getBody()->GetAngularVelocity();

	lua_newtable(S);
	lua_pushnumber(S, dir);

	return 1;
}
int luaHandler::LUA_PROJECTILE_getPosition(lua_State *S) {
	projectile* p = (projectile*)lua_touserdata(S, 1);
	glm::vec2 position = p->getBody()->getPosition();

	lua_newtable(S);
	lua_pushnumber(S, 1);
	lua_pushnumber(S, globals::metToPix(position.x));
	lua_settable(S, -3);
	lua_pushnumber(S, 2);
	lua_pushnumber(S, globals::metToPix(position.y));
	lua_settable(S, -3);

	return 1;
}
int luaHandler::LUA_PROJECTILE_destroy(lua_State *S) {
	projectile* p = (projectile*)lua_touserdata(S, 1);

	string toDrop = "";
	if (lua_isnone(S, 2) == false) {
		toDrop = lua_tostring(S, 2);
	}

	if (toDrop != "") {
		new inventoryItem(1, (itemDefinition*)(luaHandler::getDefinition(toDrop)), p->getBody()->getPosition(), luaHandler::getWorld());
	}

	p->destroy(luaHandler::getWorld());

	return 0;
}

int luaHandler::LUA_PROJECTILE_addInt(lua_State *S) {
	projectile* p = (projectile*)lua_touserdata(S, 1);

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
int luaHandler::LUA_PROJECTILE_addFloat(lua_State *S) {
	projectile* p = (projectile*)lua_touserdata(S, 1);

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
int luaHandler::LUA_PROJECTILE_addBool(lua_State *S) {
	projectile* p = (projectile*)lua_touserdata(S, 1);

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

int luaHandler::LUA_PROJECTILE_addTimer(lua_State *S) {
	projectile* p = (projectile*)lua_touserdata(S, 1);

	auto iterator = p->LUA_cppvars.find(lua_tostring(S, 2));

	if (iterator == p->LUA_cppvars.end()) {
		void* newTimerPtr = new timer;

		p->LUA_cppvars.insert(std::make_pair(lua_tostring(S, 2), std::make_pair((void*)newTimerPtr, luaVariableType::TIMERL)));
	}

	return 0;
}

int luaHandler::LUA_PROJECTILE_setValue(lua_State *S) {
	projectile* p = (projectile*)lua_touserdata(S, 1);
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

int luaHandler::LUA_PROJECTILE_getValue(lua_State *S) {
	projectile* p = (projectile*)lua_touserdata(S, 1);
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

void luaHandler::_registerProjectileFunction() {
	luaL_Reg projectilelib_m[] = {
		{ "setEmitter",  LUA_PROJECTILE_setEmitter },
		{ "getEmitter",  LUA_PROJECTILE_getEmitter },
		{ "addLight",  LUA_PROJECTILE_addLight },
		{ "getLight",  LUA_PROJECTILE_getLight },
		{ "setSide",  LUA_PROJECTILE_setSide },
		{ "setAngleDeg",  LUA_PROJECTILE_setAngleDeg },
		{ "setAngleRad",  LUA_PROJECTILE_setAngleRad },
		{ "setDirection", LUA_PROJECTILE_setDirection },
		{ "setVelocity", LUA_PROJECTILE_setVelocity },
		{ "setGravityScale", LUA_PROJECTILE_setGravityScale },
		{ "setAngularVelocity", LUA_PROJECTILE_setAngularVelocity },
		{ "setAnimation", LUA_PROJECTILE_setAnimation },
		{ "getAngle", LUA_PROJECTILE_getAngle },
		{ "getDirection", LUA_PROJECTILE_getDirection },
		{ "getVelocity", LUA_PROJECTILE_getVelocity },
		{ "getAngularVelocity", LUA_PROJECTILE_getAngularVelocity },
		{ "getPosition", LUA_PROJECTILE_getPosition },
		{ "destroy", LUA_PROJECTILE_destroy },
		{ "addInt", LUA_PROJECTILE_addInt },
		{ "addNum", LUA_PROJECTILE_addFloat },
		{ "addBool", LUA_PROJECTILE_addBool },
		{ "addTimer", LUA_PROJECTILE_addTimer },
		{ "setValue", LUA_PROJECTILE_setValue},
		{ "getValue", LUA_PROJECTILE_getValue },
		{ NULL, NULL }
	};

	luaL_newmetatable(_S, "projectile_m");

	lua_pushstring(_S, "__index");
	lua_pushvalue(_S, -2);  /* pushes the metatable*/
	lua_settable(_S, -3);  /* metatable.__index = metatable */

	luaL_setfuncs(_S, projectilelib_m, 0);
}