#pragma once
#include "body.h"
#include "luaHandler.h"
#include <Gengine/spriteBatch.h>
#include <Gengine/spriteSheet.h>
#include <map>

class agent;
class inventoryItem;

class heldItem
{
friend class luaHandler;
friend class usableItem;
public:
	void init(agent* holder, world* world, bool rightHand);

	void draw(gengine::spriteBatch* spriteBatch, int layer);
	void debugDraw(gengine::debugRenderer* debugRenderer);

	void changeItem(inventoryItem* item, glm::vec2 mousePos, world* toChange);

	void rotate(float angleDeg);

	void setAnimation(std::string animName);

	void setOffset(int xOffset, int yOffset);
	void setAngleOffset(float angleOffset);

	void setPosition(glm::vec2 &position);

	bool isHeldInRightHand() { return _rightHand; }
	void setRightDir(bool isRightDir) { _rightDir = isRightDir; }

	inventoryItem* getItem() { return _item; }

	glm::vec2 getPosition();
	glm::vec2* getPositionPointer();

	float* getAnglePointer();

	float getArmAngleOffset() { return _armAngleOffset; }

	void update();

	void startContact(userData* collidedWith, userData* thisFixture);

	void LUA_USABLE_startContact(userData* collidedWith, userData* thisFixture);

private:
	agent* _holder = nullptr;
	body* _body = nullptr;

	glm::vec2 _position;

	gengine::spriteSheet _texture;

	std::map<std::string, std::pair<void*, luaVariableType>> LUA_cppvars;
	int _lightID = -1;
	int _particleEmitterID = -1;

	bool _initialized = false;

	inventoryItem* _item;

	bool _rightHand;
	bool _rightDir;

	bool _isBeingSwung = false;

	float _angle;
	float _angleOffset;
	float _armAngleOffset = 0;
	glm::fvec2 _offset;//Actual calculated offset
	glm::fvec2 _horizontalOffset;//Offset when the item is held horizontally
};

