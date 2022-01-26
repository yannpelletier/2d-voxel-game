#pragma once
#include <string>
#include <glm/glm.hpp>
#include "world.h"
#include "definition.h"
#include "heldItem.h"
#include "body.h"

class usableItem: public itemDefinition
{
friend class luaHandler;
public:
	usableItem();
	void init(std::string texture, std::string icon, glm::ivec2 spritesheetSize, glm::vec2 itemSize, bool twoHanded);

	void equipped(heldItem* heldItem, glm::vec2 mousePos, world* toChange);
	void onEquip(heldItem* heldItem, glm::vec2 mousePos, world* toChange);

	void LUA_whileEquipped(heldItem* heldItem, glm::vec2 mousePos);
	void LUA_onEquip(heldItem* heldItem, glm::vec2 mousePos);
	void LUA_onUnequip(heldItem* heldItem, glm::vec2 mousePos);

};

