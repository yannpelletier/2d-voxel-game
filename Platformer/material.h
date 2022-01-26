#pragma once
#include <string>
#include <Gengine/spriteSheet.h>
#include <Gengine/particleBatch2D.h>
#include <glm/glm.hpp>
#include "luaHandler.h"
#include "itemDefinition.h"

class world;

class material : public itemDefinition
{
public:
	material();

	void init(std::string texture, std::string icon, bool autoTiling, int healthpoint, glm::vec2 spritesheetSize, gengine::color particleColor);

	bool primaryUse(heldItem* heldItem, glm::vec2 mousePos, world* toChange);
	bool secondaryUse(heldItem* heldItem, glm::vec2 mousePos, world* toChange);
	void equipped(heldItem* heldItem, glm::vec2 mousePos, world* toChange);

	void draw(gengine::spriteBatch* spriteBatch, heldItem* heldItem, glm::vec2 mousePos, world* toChange);

	gengine::color getParticleColor();

	bool hasAutoTiling();

	bool isSolid();
	static gengine::particleBatch2D materialParticles;

	int getMaxHP();
private:
	void _drawItem(world* world, int gridX, int gridY, gengine::spriteBatch* spriteBatch, bool canBePlaced);
	bool _autoTiling;
	int _maxHP;
	gengine::color _particleColor;
};
