#pragma once

#include "itemDefinition.h"
#include "globals.h"
#include "userData.h"

enum itemState {
	inInventory,
	dropped,
	dragged,
	inactive
};

class agent;

const float DROP_SIZE = 0.4f;
const int ACTIVATE_TIME = 750;

class inventoryItem
{
public:
	friend class inventory;
	//Dropped
	inventoryItem(int quantity, itemDefinition* itemDefinition, glm::vec2 position, world* toAdd, b2Vec2 throwForce = b2Vec2(0,0));
	//In an inventory
	inventoryItem(int quantity, itemDefinition* itemDefinition, agent* holder, world* toAdd);
	~inventoryItem();

	std::string getName();

	//Getters
	int getQuantity();
	itemDefinition* getItemDefinition();

	void draw(gengine::spriteBatch* spriteBatch);
	void updateDrop();

	bool primaryUse(heldItem* heldItem, glm::vec2 mousePos, world* toChange);
	bool secondaryUse(heldItem* heldItem, glm::vec2 mousePos, world* toChange);
	void equipped(heldItem* heldItem, glm::vec2 mousePos, world* toChange);
	void onEquip(heldItem* heldItem, glm::vec2 mousePos, world* toChange);

	void consume();

	void drop(unsigned int quantity, glm::vec2 position, world* toChange, b2Vec2 throwForce = b2Vec2(0, 0));
	void take();
	void drag();

	void startContact(userData* collidedWith, userData* thisFixture);

	//Delete 1 drop and add its quantity to this 
	void fuseDrop(inventoryItem* toFuseWith);
private:
	//Create a new dropped object
	void _createDrop(unsigned int quantity, glm::vec2 position, world* toChange, b2Vec2 throwForce = b2Vec2(0,0));

	int _quantity;
	itemState _state;

	itemDefinition* _itemDefinition;

	//Dropped
	bool _canBePicked;
	bool _neverLeftZone;
	int _timeSinceDropped;
	int _lastTick;

	b2Body* _dropBody = nullptr;
	std::vector<b2Fixture*> _fixtures;

	world* _droppedIn = nullptr;
	agent* _holder = nullptr;
};

