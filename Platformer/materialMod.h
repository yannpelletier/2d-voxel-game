#pragma once
#include "definition.h"
#include "timer.h"
#include "materialModInstance.h"
#include "heldItem.h"

class tile;

class materialMod: public definition
{
public:
	friend class luaHandler;
	materialMod();
	~materialMod();

	void LUA_onPlaced(materialModInstance* modInstance, tile* tile);
	void LUA_update(materialModInstance* modInstance, tile* tile);
	void LUA_onDestroy(materialModInstance* modInstance, tile* tile);

	bool hasAutoTiling();

	void init(std::string texture, glm::vec2 spritesheetSize, bool autoTiling, int hp);

	void addAnimation(std::string animationName, int beginIndex, int endIndex, int speedBetweenFrames);

	int getMaxHP() { return _hp; }

	gengine::spriteSheet getTexture();

private:
	gengine::spriteSheet _texture;
	bool _autoTiling;
	int _hp;
};

