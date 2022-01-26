#include "agent.h"
#include "inventoryItem.h"
#include "globals.h"

#pragma once

enum npcType {
	humanoid,
	flying,
	fourLegged,
	twoLegged
};

class npc: public agent
{
public:
	npc();
	~npc();

	void init(world* toAdd, glm::vec2 position, glm::vec2 dimensions, npcType npctype, std::string nameID);
	void initBody(glm::vec2 position, glm::vec2 dimensions);

	void addDamageLabel(glm::vec2 position, int damage, gengine::color labelColor = gengine::color(255, 0, 0, 255));

	void update();
	void updateAI();
	void draw(gengine::spriteBatch* spriteBatch);
	void updateHeldItem(heldItem* heldItem);
	void updateArms(float angle, glm::vec2 aimedPoint);
	void rotateForTwoHanded(float angle, glm::vec2 aimedPoint);
	void rotateFrontArm(float angle, glm::vec2 aimedPoint);
	void rotateBackArm(float angle, glm::vec2 aimedPoint);

	inventoryItem* refreshRightEquippedItem(glm::vec2 mousePos);
	inventoryItem* refreshLeftEquippedItem(inventoryItem* rightEquippedItem, glm::vec2 mousePos);
	inventoryItem* getRightEquippedItem();
	inventoryItem* getLeftEquippedItem();

	void debugDraw(gengine::debugRenderer* debugEngine);


private:
	glm::vec2 aimedPoint = glm::vec2(1,0);
	npcType _npcType;

	int _lastTick;

	inventoryItem* _rightEquippedItem = nullptr;
	inventoryItem* _rightEquippedLastFrame = nullptr;
	inventoryItem* _leftEquippedItem = nullptr;
	inventoryItem* _leftEquippedLastFrame = nullptr;

	std::string _nameID;
};

