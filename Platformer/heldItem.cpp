#include "heldItem.h"
#include "globals.h"
#include "agent.h"
#include "luaHandler.h"
#include "inventoryItem.h"
#include "world.h"

void heldItem::init(agent* holder ,world* world, bool rightHand) {
	_body = new body();

	_rightHand = rightHand;
	_holder = holder;

	std::vector<b2FixtureDef> fixtures;

	b2BodyDef bodyDef;
	bodyDef.fixedRotation = true;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(0, 0);
	bodyDef.allowSleep = false;

	b2PolygonShape box = _body->makeBox(glm::vec2(1, 1));

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &box;
	fixtureDef.userData = new userData(this, gameObjectType::HELDITEM);
	fixtureDef.isSensor = true;
	fixtureDef.filter.categoryBits = globals::bitData::HELDITEM_BIT;
	fixtureDef.filter.maskBits = globals::bitData::PLAYER_BIT | globals::bitData::NPC_BIT;

	_body->createBody(world, bodyDef, fixtureDef, b2Vec2(0, 0));
}

void heldItem::draw(gengine::spriteBatch* spriteBatch, int layer) {
	if (_item != nullptr) {
		glm::vec4 destRect;
		destRect.x = _body->getPosition().x - (_body->getDimensions().x * _item->getItemDefinition()->getDrawScale().x) / 2.0f;
		destRect.y = _body->getPosition().y - (_body->getDimensions().y * _item->getItemDefinition()->getDrawScale().y) / 2.0f;
		destRect.z = _body->getDimensions().x * _item->getItemDefinition()->getDrawScale().x;
		destRect.w = _body->getDimensions().y * _item->getItemDefinition()->getDrawScale().y;

		glm::fvec2 xVec;
		glm::fvec2 yVec;

		if (_rightDir) {
			xVec.x = cos(_angle) * _horizontalOffset.x;
			xVec.y = sin(_angle) * _horizontalOffset.x;

			yVec.x = cos(_angle + (M_PI / 2)) * _horizontalOffset.y;
			yVec.y = sin(_angle + (M_PI / 2)) * _horizontalOffset.y;
		}
		else {
			xVec.x = cos(_angle) * _horizontalOffset.x;
			xVec.y = sin(_angle) * _horizontalOffset.x;

			yVec.x = cos(_angle - (M_PI / 2)) * _horizontalOffset.y;
			yVec.y = sin(_angle - (M_PI / 2)) * _horizontalOffset.y;
		}


		_offset.x = xVec.x + yVec.x;
		_offset.y = xVec.y + yVec.y;

		if (_rightDir) {
			_texture.setFlipY(false);
		}
		else {
			_texture.setFlipY(true);
		}

		spriteBatch->draw(destRect, _texture.getAnimUV(), _texture.getTexture()->id, layer, gengine::color(255, 255, 255, 255), _body->getAngleRad());
	}
}

void heldItem::debugDraw(gengine::debugRenderer* debugRenderer) {
	glm::vec4 destRect;
	destRect.x = _body->getPosition().x - _body->getDimensions().x / 2.0f;
	destRect.y = _body->getPosition().y - _body->getDimensions().y / 2.0f;
	destRect.z = _body->getDimensions().x;
	destRect.w = _body->getDimensions().y;
	debugRenderer->drawBox(destRect, gengine::color(255, 255, 255, 255), _body->getAngleRad());
}

void heldItem::rotate(float angleRad) {
	if (_body != nullptr) {
		if (_body->getBody() != nullptr) {
			if (_rightDir) {
				_body->setAngleRad(angleRad + _angleOffset);
				_angle = angleRad + _angleOffset;
			}
			else {
				_body->setAngleRad(angleRad - _angleOffset);
				_angle = angleRad - _angleOffset;
			}

		}
	}
}

void heldItem::changeItem(inventoryItem* item, glm::vec2 mousePos, world* toChange) {
	_isBeingSwung = false;
	if (_lightID != -1) {
		luaHandler::getWorld()->deleteLight(_lightID);
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
	_initialized = false;
	if (_item != nullptr) {
		delete _body;
	}
	_body = nullptr;

	_item = item;

	if (_item != nullptr) {
		_texture = _item->getItemDefinition()->getTexture();
		_item->onEquip(this, mousePos, toChange);
		glm::vec2  itemSize = _item->getItemDefinition()->getItemSize();
		
		_body = new body();

		b2BodyDef bodyDef;
		bodyDef.fixedRotation = true;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(0, 0);
		bodyDef.allowSleep = false;

		b2PolygonShape box = _body->makeBox(glm::vec2(globals::pixToMet(itemSize.x), globals::pixToMet(itemSize.y)));

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &box;
		fixtureDef.userData = new userData(this, gameObjectType::HELDITEM);
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.0001f;
		fixtureDef.isSensor = true;
		fixtureDef.filter.categoryBits = globals::bitData::HELDITEM_BIT;
		fixtureDef.filter.maskBits = globals::bitData::PLAYER_BIT | globals::bitData::NPC_BIT;

		_body->createBody(toChange, bodyDef, fixtureDef, b2Vec2(0, 0));

		_horizontalOffset = _item->getItemDefinition()->getOffset();
		_angleOffset = _item->getItemDefinition()->getAngleOffset();
		_armAngleOffset = item->getItemDefinition()->getArmAngleOffset();
	}

	luaHandler::getWorld()->orderLights();
	_initialized = true;
}

void heldItem::setAnimation(std::string animName) {
	_texture.changeAnimation(animName);
}

void heldItem::setOffset(int xOffset, int yOffset) {
	_horizontalOffset.x = globals::pixToMet(xOffset);
	_horizontalOffset.y = globals::pixToMet(yOffset);
}

void heldItem::setAngleOffset(float angleOffset) {
	_angleOffset = angleOffset * (M_PI * 2) / 360;;
}

void heldItem::setPosition(glm::vec2 &position) {
	if (_body != nullptr) {
		if (_body->getBody() != nullptr) {
			_body->setPosition(position + _offset);
		}
	}
}

glm::vec2 heldItem::getPosition() {
	return _position;
}

glm::vec2* heldItem::getPositionPointer() {
	return &_position;
}

float* heldItem::getAnglePointer() {
	return &_angle;
}

void heldItem::update() {
	if (_body != nullptr) {
		if (_body->getBody() != nullptr) {
			_position = glm::vec2(_body->getPosition().x, _body->getPosition().y);
			_texture.update();
		}
	}


	if (_holder != nullptr) {
		if (_holder->getSkeleton() != nullptr) {
			if (_holder->getSkeleton()->getEvent("charged")) {
				_isBeingSwung = true;
			}
			if (_holder->getSkeleton()->getEvent("swing end")) {
				_isBeingSwung = false;
			}
		}
	}

	if (_body != nullptr) {
		if (_body->getBody() != nullptr) {
			if (_item != nullptr) {
				for (b2ContactEdge* ce = _body->getContactList(); ce != nullptr; ce = ce->next) {
					b2Contact* c = ce->contact;
					userData* fixtureAUserData;
					userData* fixtureBUserData;

					if (c->IsTouching()) {
						getUserDatas(c, fixtureAUserData, fixtureBUserData);

						if (fixtureAUserData->dataUser == this) {
							LUA_USABLE_startContact(fixtureBUserData, fixtureAUserData);
						}
						else if (fixtureBUserData->dataUser == this) {
							LUA_USABLE_startContact(fixtureAUserData, fixtureBUserData);
						}
					}
				}
			}
		}
	}

}

void heldItem::startContact(userData* collidedWith, userData* thisFixture) {
	LUA_USABLE_startContact(collidedWith, thisFixture);
}

void heldItem::LUA_USABLE_startContact(userData* collidedWith, userData* thisFixture) {
	lua_State* S = luaHandler::getLuaState();

	if (_body != nullptr) {
		if (_item != nullptr) {
			if (collidedWith->type == gameObjectType::PLAYER || collidedWith->type == gameObjectType::NPC) {
				if (collidedWith->dataUser != _holder) {
					lua_getglobal(S, _item->getItemDefinition()->getNameID().c_str());
					lua_pushstring(S, "onContact");
					lua_gettable(S, -2);
					lua_pushvalue(S, -2);
					lua_pushlightuserdata(S, this);
					luaL_getmetatable(S, "usable_m");
					lua_setmetatable(S, -2);

					agent** addedLightUserdata = (agent**)lua_newuserdata(S, sizeof(agent*));
					*addedLightUserdata = static_cast<agent*>(collidedWith->dataUser);
					luaL_getmetatable(S, "agent_m");
					lua_setmetatable(S, -2);
					lua_pushstring(S, "agent");

					lua_pcall(S, 4, 0, 0);
					lua_pop(S, 1);
				}
			}
			else if (collidedWith->type == gameObjectType::TILE) {
				lua_getglobal(S, _item->getItemDefinition()->getNameID().c_str());
				lua_pushstring(S, "onContact");
				lua_gettable(S, -2);
				lua_pushvalue(S, -2);
				lua_pushlightuserdata(S, this);
				luaL_getmetatable(S, "usable_m");
				lua_setmetatable(S, -2);

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
}