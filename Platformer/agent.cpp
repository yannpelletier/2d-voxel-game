#include "agent.h"
#include "globals.h"
#include "luaHandler.h"
#include "timer.h"
#include "world.h"

void agent::destroy(world* world) {
	_body->destroy(world);
	if (_inventory != nullptr) {
		delete _inventory;
	}
}

void agent::draw(gengine::spriteBatch* spriteBatch) {
	_skeleton.draw(spriteBatch);
}

void agent::updateGroundState() {
	_body->getBody()->SetGravityScale(1);

	if (_body->getLinearVelocity().y > globals::globals::MAX_Y_SPEED * 0.10) {
		if (_groundState != jumping) {
			_groundState = rising;
		}
	}
	else if (_body->getLinearVelocity().y < -globals::globals::MAX_Y_SPEED * 0.15 && _body->getLinearVelocity().y > -40) {
		if (_groundState != quickFalling) {
			_groundState = falling;
		}
	}
	else if (_body->getLinearVelocity().y < -40) {
		_groundState = quickFalling;
	}
	else {
		for (b2ContactEdge* ce = _body->getContactList(); ce != nullptr; ce = ce->next) {
			b2Contact* c = ce->contact;
			userData* fixtureAUserData;
			userData* fixtureBUserData;

			if (c->IsTouching()) {
				getUserDatas(c, fixtureAUserData, fixtureBUserData);


				if (fixtureAUserData->type == FOOT_SENSOR) {
					if (fixtureBUserData->type == TILE || fixtureBUserData->type == TILESLOPE || fixtureBUserData->type == SOLID_BODY || fixtureBUserData->type == PLATFORM) {
						if (_groundState == quickFalling) {
							damage(_currentFallDamage,0);
						}

						if (_groundState != landing &&_groundState != ground) {
							_groundState = landing;
						}
						else {
							_groundState = ground;
						}
					}
				}
				else if (fixtureBUserData->type == FOOT_SENSOR) {
					if (fixtureAUserData->type == TILE || fixtureAUserData->type == TILESLOPE || fixtureAUserData->type == SOLID_BODY || fixtureAUserData->type == PLATFORM) {
						if (_groundState == quickFalling) {
							damage(_currentFallDamage, 0);
						}

						if (_groundState != landing &&_groundState != ground) {
							_groundState = landing;
						}
						else {
							_groundState = ground;
						}
					}
				}
			}
		}
	}

	for (b2ContactEdge* ce = _body->getContactList(); ce != nullptr; ce = ce->next) {
		b2Contact* c = ce->contact;
		userData* fixtureAUserData;
		userData* fixtureBUserData;

		if (c->IsTouching()) {
			getUserDatas(c, fixtureAUserData, fixtureBUserData);

			if (fixtureAUserData->type == FOOT_SENSOR) {
				if (fixtureBUserData->type == TILESLOPE) {
					agent* collAgent = static_cast<agent*>(fixtureAUserData->dataUser);

					if (!_isWalking) {
						collAgent->getBody()->getBody()->SetGravityScale(0);
						if (_wasWalking) {
							collAgent->getBody()->getBody()->SetLinearVelocity(b2Vec2(0, 0));
						}
						else {
							collAgent->getBody()->getBody()->SetLinearVelocity(b2Vec2(0, collAgent->getBody()->getLinearVelocity().y));
						}
					}
				}
			}
			else if (fixtureBUserData->type == FOOT_SENSOR) {
				if (fixtureAUserData->type == TILESLOPE) {
					agent* collAgent = static_cast<agent*>(fixtureBUserData->dataUser);

					if (!_isWalking) {
						collAgent->getBody()->getBody()->SetGravityScale(0);
						if (_wasWalking) {
							collAgent->getBody()->getBody()->SetLinearVelocity(b2Vec2(0, 0));
						}
						else {
							collAgent->getBody()->getBody()->SetLinearVelocity(b2Vec2(0, collAgent->getBody()->getLinearVelocity().y));
						}
					}
				}
			}
		}


		
	}

	if (_groundState == ground) {
		if (_body->getLinearVelocity().x < -_maxXSpeed) {
			_body->setLinearVelocity(b2Vec2(-_maxXSpeed, _body->getLinearVelocity().y));
		}
		else if (_body->getLinearVelocity().x > _maxXSpeed) {
			_body->setLinearVelocity(b2Vec2(_maxXSpeed, _body->getLinearVelocity().y));
		}
	}
	else {
		if (_body->getLinearVelocity().x < -_maxXSpeed * 2) {
			_body->setLinearVelocity(b2Vec2(_body->getLinearVelocity().x * 0.95, _body->getLinearVelocity().y));
		}
		else if (_body->getLinearVelocity().x > _maxXSpeed * 2) {
			_body->setLinearVelocity(b2Vec2(_body->getLinearVelocity().x * 0.95, _body->getLinearVelocity().y));
		}
	}

	_currentFallDamage = _body->getLinearVelocity().y * -1;

}

void agent::updateAnimations() {
	if (_groundState == ground) {
		if (_body->getLinearVelocity().x > 0.4) {
			if (_facingRight) {
				_skeleton.setAnimation("run", true, 0, false);
			}
			else {
				_skeleton.setAnimation("run backward", true, 0, false);
			}
		}
		else if (_body->getLinearVelocity().x < -0.4) {
			if (!_facingRight) {
				_skeleton.setAnimation("run", true, 0, false);
			}
			else {
				_skeleton.setAnimation("run backward", true, 0, false);
			}
		}
		else {
			_skeleton.setAnimation("idle", true, 0, false);
		}
	}

	if (_groundState == rising || _groundState == jumping) {
		_skeleton.setAnimation("rise", true, 0, false);
	}
	else if (_groundState == falling) {
		_skeleton.setAnimation("fall", true, 0, false);
	}
	else if (_groundState == quickFalling) {
		_skeleton.setAnimation("panic fall", true, 0, false);
	}
	else if (_groundState == landing) {
		_skeleton.setAnimation("land", false, 0, false);
	}
}

void agent::updatePlatformFiltering() {
	if (_body->getLinearVelocity().y > 0) {
		b2Filter footFixtureFilter = _body->getFixture(2)->GetFilterData();
		footFixtureFilter.maskBits = 0;
		_body->getFixture(2)->SetFilterData(footFixtureFilter);
	}
	else {
		b2Filter footFixtureFilter = _body->getFixture(2)->GetFilterData();
		footFixtureFilter.maskBits = globals::PLATFORM_BIT;
		_body->getFixture(2)->SetFilterData(footFixtureFilter);
	}
}

void agent::damage(int damage, int invulnerabilityTime, gengine::color labelColor) {
	if (_invulnerabilityTimer <= 0) {
		_health -= damage;
		_invulnerabilityTimer = invulnerabilityTime;
		addDamageLabel(_body->getPosition(), damage, labelColor);
	}
}

void agent::knockBack(float angle, float force) {
	b2Vec2 forceVec;
	forceVec.x = cos(angle) * force;
	forceVec.y = sin(angle) * force;
	_body->applyForceToCenter(forceVec);
}

void agent::setSpeedMultiplier(float multiplier) {
	if (multiplier > 0) {
		_xAcceleration /= _speedMultiplier;
		_maxXSpeed /= _speedMultiplier;
		_speedMultiplier = multiplier;
		_xAcceleration *= _speedMultiplier;
		_maxXSpeed *= _speedMultiplier;
		_skeleton.setAnimationSpeedMultiplier(multiplier, 0);
	}
}

void agent::_moveRight() {
	if (_groundState == ground) {
		if (_aimingRight || _aimingLeft) {
			if (_facingRight) {
				_body->applyForceToCenter(b2Vec2(_xAcceleration, 0.0f));
			}
			else {
				_body->applyForceToCenter(b2Vec2(_xAcceleration / 2, 0.0f));
			}
		}
		else {
			_body->applyForceToCenter(b2Vec2(_xAcceleration, 0.0f));
		}
	}
	else {
		if (_aimingRight || _aimingLeft) {
			if (_facingRight) {
				_body->applyForceToCenter(b2Vec2(_xAcceleration / 10, 0.0f));
			}
			else {
				_body->applyForceToCenter(b2Vec2(_xAcceleration / 20, 0.0f));
			}
		}
		else {
			_body->applyForceToCenter(b2Vec2(_xAcceleration / 10, 0.0f));
		}
	}


	_isWalking = true;
	_walkingRight = true;
}

void agent::_moveLeft() {
	if (_groundState == ground) {
		if (_aimingRight || _aimingLeft) {
			if (!_facingRight) {
				_body->applyForceToCenter(b2Vec2(-_xAcceleration, 0.0f));
			}
			else {
				_body->applyForceToCenter(b2Vec2(-_xAcceleration / 2, 0.0f));
			}
		}
		else {
			_body->applyForceToCenter(b2Vec2(-_xAcceleration, 0.0f));
		}
	}
	else {
		if (_aimingRight || _aimingLeft) {
			if (!_facingRight) {
				_body->applyForceToCenter(b2Vec2(-_xAcceleration / 10, 0.0f));
			}
			else {
				_body->applyForceToCenter(b2Vec2(-_xAcceleration / 20, 0.0f));
			}
		}
		else {
			_body->applyForceToCenter(b2Vec2(-_xAcceleration / 10, 0.0f));
		}
	}


	_isWalking = true;
	_walkingRight = false;
}

void agent::_slow() {
	_body->setLinearVelocity(b2Vec2(_body->getLinearVelocity().x * 0.90, _body->getLinearVelocity().y));
}

void agent::_jump() {
	if (_groundState == ground) {
		_body->applyLinearImpulse(b2Vec2(0.0f, _jumpForce), b2Vec2(0, 0));
		_groundState = jumping;
	}
}

void agent::_releaseJump() {
	_body->applyForceToCenter(b2Vec2(0.0f, -350));
}

inventory* agent::getInventory() {
	return _inventory;
}

void agent::update() {
	return; 
}

void agent::startContact(userData* collidedWith, userData* thisFixture) {
	return;
}

void agent::endContact(userData* collidedWith, userData* thisFixture) {
	return;
}

void agent::equipItem(int itemIndex) {
	return;
}

/*int luaHandler::LUA_AGENT_hasItemInInventory() {
	agent* p = *((agent**)lua_touserdata(S, 1));
}

int luaHandler::LUA_AGENT_consumeItemInInventory() {

}*/

int luaHandler::LUA_AGENT_playAnimation(lua_State *S) {
	agent* p = *((agent**)lua_touserdata(S, 1));

	std::string animationName = lua_tostring(S, 2);
	int animationTime = lua_tointeger(S, 3);

	return 0;
}

int luaHandler::LUA_AGENT_walkRight(lua_State *S) {
	agent* p = *((agent**)lua_touserdata(S, 1));

	p->_moveRight();

	return 0;
}

int luaHandler::LUA_AGENT_isWalkingRight(lua_State *S) {
	agent* p = *((agent**)lua_touserdata(S, 1));

	if (p->_isWalking && p->_walkingRight) {
		lua_pushboolean(S, true);
	}
	else {
		lua_pushboolean(S, false);
	}

	return 1;
}

int luaHandler::LUA_AGENT_isWalkingLeft(lua_State *S) {
	agent* p = *((agent**)lua_touserdata(S, 1));

	if (p->_isWalking && !p->_walkingRight) {
		lua_pushboolean(S, true);
	}
	else {
		lua_pushboolean(S, false);
	}

	return 1;
}

int luaHandler::LUA_AGENT_walkLeft(lua_State *S) {
	agent* p = *((agent**)lua_touserdata(S, 1));

	p->_moveLeft();

	return 0;
}

int luaHandler::LUA_AGENT_jump(lua_State *S) {
	agent* p = *((agent**)lua_touserdata(S, 1));

	p->_jump();

	return 0;
}

int luaHandler::LUA_AGENT_getPosition(lua_State *S) {
	agent* p = *((agent**)lua_touserdata(S, 1));

	lua_newtable(S);
	lua_pushnumber(S, 1);
	lua_pushnumber(S, globals::metToPix(p->_body->getPosition().x));
	lua_settable(S, -3);
	lua_pushnumber(S, 2);
	lua_pushnumber(S, globals::metToPix(p->_body->getPosition().y));
	lua_settable(S, -3);

	return 1;
}

int luaHandler::LUA_AGENT_getDimensions(lua_State *S) {
	agent* p = *((agent**)lua_touserdata(S, 1));

	lua_newtable(S);
	lua_pushnumber(S, 1);
	lua_pushnumber(S, globals::metToPix(p->_body->getDimensions().x));
	lua_settable(S, -3);
	lua_pushnumber(S, 2);
	lua_pushnumber(S, globals::metToPix(p->_body->getDimensions().y));
	lua_settable(S, -3);

	return 1;
}

int luaHandler::LUA_AGENT_getVelocity(lua_State *S) {
	agent* p = *((agent**)lua_touserdata(S, 1));

	lua_newtable(S);
	lua_pushnumber(S, 1);
	lua_pushnumber(S, globals::metToPix(p->_body->getLinearVelocity().x));
	lua_settable(S, -3);
	lua_pushnumber(S, 2);
	lua_pushnumber(S, globals::metToPix(p->_body->getLinearVelocity().y));
	lua_settable(S, -3);

	return 1;
}

int luaHandler::LUA_AGENT_getJumpForce(lua_State *S) {
	agent* p = *((agent**)lua_touserdata(S, 1));

	lua_pushnumber(S, globals::metToPix(p->_jumpForce));

	return 1;
}

int luaHandler::LUA_AGENT_enableBehaviour(lua_State *S) {
	agent* p = *((agent**)lua_touserdata(S, 1));

	std::string behaviourName = lua_tostring(S, 2);

	p->_activeBehaviours.push_back(behaviourName);

	lua_getglobal(S, behaviourName.c_str());
	lua_pushstring(S, "whenActivated");
	lua_gettable(S, -2);
	lua_pushvalue(S, -2);
	lua_pushlightuserdata(S, &p);
	luaL_getmetatable(S, "agent_m");
	lua_setmetatable(S, -2);

	lua_pcall(S, 2, 0, 0);
	lua_pop(S, 1);

	return 0;

}
int luaHandler::LUA_AGENT_disableBehaviour(lua_State *S) {
	agent* p = *((agent**)lua_touserdata(S, 1));

	std::string behaviourName = lua_tostring(S, 2);

	for (int i = 0; i < p->_activeBehaviours.size(); i++) {
		if (p->_activeBehaviours[i] == behaviourName) {
			p->_activeBehaviours[i] = p->_activeBehaviours.back();
			p->_activeBehaviours.pop_back();

			lua_getglobal(S, behaviourName.c_str());
			lua_pushstring(S, "whenActivated");
			lua_gettable(S, -2);
			lua_pushvalue(S, -2);
			lua_pushlightuserdata(S, &p);
			luaL_getmetatable(S, "agent_m");
			lua_setmetatable(S, -2);

			lua_pcall(S, 2, 0, 0);
			lua_pop(S, 1);
		}
	}

	return 0;
}
int luaHandler::LUA_AGENT_doBehaviourOnce(lua_State *S) {
	agent* p = *((agent**)lua_touserdata(S, 1));

	std::string behaviourName = lua_tostring(S, 2);

	lua_getglobal(S, behaviourName.c_str());
	lua_pushstring(S, "whileActive");
	lua_gettable(S, -2);
	lua_pushvalue(S, -2);
	lua_pushlightuserdata(S, p);
	luaL_getmetatable(S, "agent_m");
	lua_setmetatable(S, -2);

	lua_pcall(S, 2, 0, 0);
	lua_pop(S, 1);

	return 0;
}

int luaHandler::LUA_AGENT_damage(lua_State *S) {
	agent* p = *((agent**)lua_touserdata(S, 1));

	p->damage(lua_tointeger(S, 2), lua_tointeger(S,3));
	
	return 0;
}

int luaHandler::LUA_AGENT_knockBack(lua_State *S) {
	agent* p = *((agent**)lua_touserdata(S, 1));

	p->knockBack(lua_tonumber(S,2),lua_tonumber(S, 3));

	return 0;
}

int luaHandler::LUA_AGENT_setInt(lua_State *S) {
	agent* p = *((agent**)lua_touserdata(S, 1));

	auto iterator = p->LUA_cppvars.find(lua_tostring(S, 2));

	if (iterator == p->LUA_cppvars.end()) {
		void* newInt = malloc(sizeof(lua_tointeger(S, 3)));
		*(int*)newInt = lua_tointeger(S, 3);

		p->LUA_cppvars.insert(std::make_pair(lua_tostring(S, 2), (void*)newInt));
	}
	else {
		*((int*)(p->LUA_cppvars[lua_tostring(S, 2)])) = lua_tointeger(S, 3);
	}

	return 0;
}
int luaHandler::LUA_AGENT_setFloat(lua_State *S) {
	agent* p = *((agent**)lua_touserdata(S, 1));

	auto iterator = p->LUA_cppvars.find(lua_tostring(S, 2));

	if (iterator == p->LUA_cppvars.end()) {
		void* newFloat = malloc(sizeof(lua_tonumber(S, 3)));
		*(float*)newFloat = lua_tonumber(S, 3);

		p->LUA_cppvars.insert(std::make_pair(lua_tostring(S, 2), (void*)newFloat));
	}
	else {
		*((float*)(p->LUA_cppvars[lua_tostring(S, 2)])) = lua_tonumber(S, 3);
	}

	return 0;
}
int luaHandler::LUA_AGENT_setBool(lua_State *S) {
	agent* p = *((agent**)lua_touserdata(S, 1));

	auto iterator = p->LUA_cppvars.find(lua_tostring(S, 2));

	if (iterator == p->LUA_cppvars.end()) {
		void* newBool = malloc(sizeof(lua_toboolean(S, 3)));
		*(bool*)newBool = lua_toboolean(S, 3);

		p->LUA_cppvars.insert(std::make_pair(lua_tostring(S, 2), (void*)newBool));
	}
	else {
		*((bool*)(p->LUA_cppvars[lua_tostring(S, 2)])) = lua_toboolean(S, 3);
	}

	return 0;
}
int luaHandler::LUA_AGENT_setTimer(lua_State *S) {
	agent* p = *((agent**)lua_touserdata(S, 1));

	auto iterator = p->LUA_cppvars.find(lua_tostring(S, 2));

	if (iterator == p->LUA_cppvars.end()) {
		void* newTimerPtr = new timer;

		p->LUA_cppvars.insert(std::make_pair(lua_tostring(S, 2), (void*)newTimerPtr));
	}

	return 0;
}
int luaHandler::LUA_AGENT_getInt(lua_State *S) {
	agent* p = *((agent**)lua_touserdata(S, 1));
	lua_pushinteger(S, *(int*)(p->LUA_cppvars[lua_tostring(S, 2)]));
	return 1;
}
int luaHandler::LUA_AGENT_getFloat(lua_State *S) {
	agent* p = *((agent**)lua_touserdata(S, 1));
	lua_pushnumber(S, *(float*)(p->LUA_cppvars[lua_tostring(S, 2)]));
	return 1;
}
int luaHandler::LUA_AGENT_getBool(lua_State *S) {
	agent* p = *((agent**)lua_touserdata(S, 1));
	lua_pushboolean(S, *(bool*)(p->LUA_cppvars[lua_tostring(S, 2)]));
	return 1;
}
int luaHandler::LUA_AGENT_getTimer(lua_State *S) {
	agent* p = *((agent**)lua_touserdata(S, 1));
	timer* pe = (timer*)(p->LUA_cppvars[lua_tostring(S, 2)]);
	timer** addedLightUserdata = (timer**)lua_newuserdata(S, sizeof(timer*));
	*addedLightUserdata = pe;
	luaL_getmetatable(S, "timer_m");
	lua_setmetatable(S, -2);
	return 1;
}

int luaHandler::LUA_AGENT_newNPC(lua_State *S) {
	std::string npcName = lua_tostring(S, 1);

	if (!_verifyLuaName(npcName)) {
		return 0;
	}

	lua_getglobal(S, npcName.c_str());
	lua_pushstring(S, "setProperties");
	lua_gettable(S, -2);
	lua_pushvalue(S, -2);
	lua_pcall(S, 1, 0, 0);
	lua_pop(S, 1);

	return 0;
}

void luaHandler::_registerAgentFunction() {
	luaL_Reg agentlib_m[] = {
		{ "walkRight", LUA_AGENT_walkRight},
		{ "walkLeft", LUA_AGENT_walkLeft },
		{ "isWalkingRight", LUA_AGENT_isWalkingRight },
		{ "isWalkingLeft", LUA_AGENT_isWalkingLeft },
		{ "jump", LUA_AGENT_jump },
		{ "getPosition", LUA_AGENT_getPosition },
		{ "getDimensions", LUA_AGENT_getDimensions },
		{ "getVelocity", LUA_AGENT_getVelocity },
		{ "getJumpForce", LUA_AGENT_getJumpForce },
		{ "enableBehaviour", LUA_AGENT_enableBehaviour },
		{ "disableBehaviour", LUA_AGENT_disableBehaviour },
		{ "doBehaviourOnce", LUA_AGENT_doBehaviourOnce },
		{ "damage", LUA_AGENT_damage },
		{ "knockBack", LUA_AGENT_knockBack },
		{ "setInt", LUA_AGENT_setInt },
		{ "getInt", LUA_AGENT_getInt },
		{ "setNum", LUA_AGENT_setFloat },
		{ "getNum", LUA_AGENT_getFloat },
		{ "setBool", LUA_AGENT_setBool },
		{ "getBool", LUA_AGENT_getBool },
		{ "setTimer", LUA_AGENT_setTimer },
		{ "getTimer", LUA_AGENT_getTimer },
		{ "playAnimation", LUA_AGENT_playAnimation},
		{ NULL, NULL }
	};

	luaL_newmetatable(_S, "agent_m");

	lua_pushstring(_S, "__index");
	lua_pushvalue(_S, -2);  /* pushes the metatable*/
	lua_settable(_S, -3);  /* metatable.__index = metatable */

	luaL_setfuncs(_S, agentlib_m, 0);
}