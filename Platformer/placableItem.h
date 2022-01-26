#pragma once
#include <Gengine/animation.h>
#include "itemDefinition.h"
#include "luaHandler.h"
#include "timer.h"

enum placableFlags { floors = 0x01, ceiling = 0x02, background = 0x04, wall = 0x08 };

class fatherTile;

class placableItem : public itemDefinition
{
public:
	friend class luaHandler;

	void init(std::string texture, std::string icon, int placeConditions, glm::vec2 size, glm::ivec2 spritesheetSize, int healthpoint);

	bool primaryUse(heldItem* heldItem, glm::vec2 mousePos, world* toChange);
	void equipped(heldItem* heldItem, glm::vec2 mousePos, world* toChange);
	void onEquip(heldItem* heldItem, glm::vec2 mousePos, world* toChange);

	void draw(gengine::spriteBatch* spriteBatch, heldItem* heldItem, glm::vec2 mousePos, world* toChange);

	void LUA_onPlaced(fatherTile* newTile);
	void LUA_update(fatherTile* newTile);
	void LUA_use(fatherTile* newTile, agent* user);

	void LUA_whileEquipped(heldItem* heldItem, glm::vec2 mousePos);
	void LUA_onEquip(heldItem* heldItem, glm::vec2 mousePos);
	void LUA_onUnequip(heldItem* heldItem, glm::vec2 mousePos);

	glm::ivec2 getSize();
	int getMaxHP() { return _hp; }

	bool canBePlaced(world* world, int fatherTileX, int fatherTileY);

private:
	void _drawItem(world* world, int gridX, int gridY, gengine::spriteBatch* spriteBatch, bool canBePlaced);
	bool _canBePlacedOnFloor(world* world, int fatherTileX, int fatherTileY);
	bool _canBePlacedOnCeiling(world* world, int fatherTileX, int fatherTileY);
	bool _canBePlacedOnBackground(world* world, int fatherTileX, int fatherTileY);
	bool _canBePlacedOnWall(world* world, int fatherTileX, int fatherTileY);
	bool _hasEveryTileFree(world* world, int fatherTileX, int fatherTileY);

	//Choose where the item can be placed with bitwise operations (&)
	unsigned int _placableConditions;
	glm::vec2 _size;
	int _hp;
};
