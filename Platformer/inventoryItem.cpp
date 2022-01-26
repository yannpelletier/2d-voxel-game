#include "inventoryItem.h"
#include "world.h"
#include "globals.h"
#include "GUI.h"

inventoryItem::inventoryItem(int quantity, itemDefinition* itemDefinition, glm::vec2 position, world* toAdd, b2Vec2 throwForce)
{
	_state = dropped;
	_timeSinceDropped = 0;
	_lastTick = SDL_GetTicks();
	_quantity = quantity;
	_itemDefinition = itemDefinition;

	_createDrop(quantity, position, toAdd, throwForce);
}

inventoryItem::inventoryItem(int quantity, itemDefinition* itemDefinition, agent* holder, world* toAdd) {
	_state = inInventory;
	_quantity = quantity;
	_itemDefinition = itemDefinition;

	_holder = holder;
	_droppedIn = toAdd;
}

inventoryItem::~inventoryItem()
{
	if (_state == dropped) {
		_droppedIn->deleteBody(_dropBody);
		_droppedIn->deleteDrop(this);
	}
	else if (_state == inInventory) {
		if (_holder != nullptr) {
			_holder->getInventory()->removeItem(this);
		}
	}
	else if (_state == dragged) {
		GUI::deleteDraggedSlot();
	}
	_state = inactive;
}

std::string inventoryItem::getName() {
	return _itemDefinition->getNameID();
}

bool inventoryItem::primaryUse(heldItem* heldItem, glm::vec2 mousePos, world* toChange) {
	if (_itemDefinition->isUsable()) {
		bool used = _itemDefinition->primaryUse(heldItem, mousePos, toChange);
		if (used) {
			_quantity -= 1;
			if (_quantity <= 0) {
				delete this;
			}
		}
		return used;
	}
	else {
		return _itemDefinition->primaryUse(heldItem, mousePos, toChange);
	}

}

bool inventoryItem::secondaryUse(heldItem* heldItem, glm::vec2 mousePos, world* toChange) {
	if (_itemDefinition->isUsable()) {
		bool used = _itemDefinition->secondaryUse(heldItem, mousePos, toChange);

		if (used) {
			_quantity -= 1;
			if (_quantity == 0) {
				delete this;
			}
		}
		return used;
	}
	else {
		_itemDefinition->secondaryUse(heldItem, mousePos, toChange);
	}
}

void inventoryItem::equipped(heldItem* heldItem, glm::vec2 mousePos, world* toChange) {
	_itemDefinition->equipped(heldItem, mousePos, toChange);
}
void inventoryItem::onEquip(heldItem* heldItem, glm::vec2 mousePos, world* toChange) {
	_itemDefinition->onEquip(heldItem, mousePos, toChange);
}


void inventoryItem::consume() {
	if (_state == inInventory || _state == dragged && _holder != nullptr) {
		_quantity -= 1;
		if (_quantity == 0) {
			delete this;
		}
	}
}

int inventoryItem::getQuantity() {
	return _quantity;
}

itemDefinition* inventoryItem::getItemDefinition() {
	return _itemDefinition;
}

void inventoryItem::fuseDrop(inventoryItem* toFuseWith) {
	_quantity += toFuseWith->getQuantity();
	delete toFuseWith;
}

void inventoryItem::_createDrop(unsigned int quantity, glm::vec2 position, world* toChange, b2Vec2 throwForce) {
		_droppedIn = toChange;

		_droppedIn->addDrop(this);

		std::vector<b2FixtureDef> fixtures;

		b2BodyDef bodyDef;
		bodyDef.fixedRotation = true;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(position.x, position.y);
		bodyDef.gravityScale = 0.5;
		glm::vec2 dimensions(DROP_SIZE);

		b2PolygonShape boxShape;
		boxShape.SetAsBox(dimensions.x / 2.0f, dimensions.y / 2.0f);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &boxShape;
		fixtureDef.friction = 10;
		fixtureDef.filter.categoryBits = globals::DROP_BIT;
		fixtureDef.filter.maskBits = globals::TILE_BIT;
		fixtures.push_back(fixtureDef);

		b2PolygonShape boxShape2;
		boxShape2.SetAsBox(dimensions.x / 8, dimensions.y / 8);

		b2FixtureDef fixtureDef2;
		fixtureDef2.shape = &boxShape2;
		fixtureDef2.isSensor = true;
		fixtureDef2.userData = new userData(this, gameObjectType::DROP);
		fixtureDef2.filter.categoryBits = globals::DROP_BIT;
		fixtureDef2.filter.maskBits = globals::PLAYER_BIT;
		fixtures.push_back(fixtureDef2);

		//Drop collect zone sensor
		b2CircleShape circleShape;
		circleShape.m_radius = dimensions.x * 3;

		b2FixtureDef pickUp;
		pickUp.shape = &circleShape;
		pickUp.isSensor = true;
		pickUp.filter.categoryBits = globals::DROP_BIT;
		pickUp.filter.maskBits = globals::PLAYER_BIT;
		pickUp.userData = new userData(this, gameObjectType::DROP_COLLECT_ZONE);
		fixtures.push_back(pickUp);

		toChange->createBody(bodyDef, fixtures, &_dropBody, &_fixtures, throwForce);

		if (_holder != nullptr) {
			_holder->getInventory()->removeItem(this);
			_holder = nullptr;
		}
}

void inventoryItem::drop(unsigned int quantity, glm::vec2 position, world* toChange, b2Vec2 throwForce) {
	if (_state == inInventory || _state == dragged) {
		int quantityToDrop = quantity;

		if (quantityToDrop > _quantity) {
			quantityToDrop = _quantity;
		}

		if (_quantity - quantityToDrop == 0) {
			new inventoryItem(quantityToDrop, _itemDefinition, position, toChange, throwForce);
			delete this;
		}
		else {
			_quantity -= quantityToDrop;
			new inventoryItem(quantityToDrop, _itemDefinition, position, toChange, throwForce);
		}
	}
}

void inventoryItem::take() {
	if (_state == dropped) {
		if (_holder->getInventory()->addItem(this) == false) {
			_droppedIn->deleteBody(_dropBody);
			_droppedIn->deleteDrop(this);
			_canBePicked = false;
			_neverLeftZone = false;
			_droppedIn = nullptr;
			_dropBody = nullptr;
			_fixtures.clear();
			_state = inInventory;
		}
	}
}

void inventoryItem::drag() {
	_state = dragged;
}

void inventoryItem::draw(gengine::spriteBatch* spriteBatch) {
	if (_state == dropped) {
		if (_dropBody != nullptr) {
			glm::vec4 destRect;
			destRect.z = DROP_SIZE;
			destRect.w = DROP_SIZE;
			destRect.x = _dropBody->GetPosition().x - DROP_SIZE / 2;
			destRect.y = _dropBody->GetPosition().y - DROP_SIZE / 2;
			destRect.y += 0.1f;

			spriteBatch->draw(destRect, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), _itemDefinition->getIcon().id, 7, gengine::color(255, 255, 255, 255));
		}
	}
}

void inventoryItem::startContact(userData* collidedWith, userData* thisFixture) {
	if (thisFixture->type == DROP_COLLECT_ZONE) {
		if (collidedWith->type == PLAYER) {
			if (_canBePicked) {
				if (!static_cast<player*>(collidedWith->dataUser)->getInventory()->isFull(this)) {
					if (_holder == nullptr) {
						_holder = static_cast<player*>(collidedWith->dataUser);
						_fixtures[0]->SetSensor(true);
					}
				}
			}
		}
	}
	if (thisFixture->type == DROP) {
		if (collidedWith->type == PLAYER) {
			if (_holder != nullptr) {
				take();
			}

		}
	}

}

void inventoryItem::updateDrop() {
	if (_state == dropped) {
		if (!_canBePicked) {

			_timeSinceDropped += SDL_GetTicks() - _lastTick;
			_lastTick = SDL_GetTicks();
			if (_timeSinceDropped > ACTIVATE_TIME) {
				_canBePicked = true;
				_lastTick = 0;
				_timeSinceDropped = 0;
				//Makes sure the item is picked if the player hasn't left the drop hitbox
				for (b2ContactEdge* ce = _dropBody->GetContactList(); ce != nullptr; ce = ce->next) {
					b2Contact* c = ce->contact;
					userData* fixtureAUserData;
					userData* fixtureBUserData;

					getUserDatas(c, fixtureAUserData, fixtureBUserData);

					if (fixtureAUserData != nullptr && fixtureBUserData != nullptr) {
						if (c->IsTouching()) {
							if (fixtureAUserData->type == DROP_COLLECT_ZONE) {
								if (fixtureBUserData->type == PLAYER) {
									if (!static_cast<player*>(fixtureBUserData->dataUser)->getInventory()->isFull(this)) {
										_holder = static_cast<player*>(fixtureBUserData->dataUser);
										_fixtures[0]->SetSensor(true);
										_neverLeftZone = true;
										return;
									}
								}
							}
							else if (fixtureBUserData->type == DROP_COLLECT_ZONE) {
								if (fixtureAUserData->type == PLAYER) {
									if (!static_cast<player*>(fixtureAUserData->dataUser)->getInventory()->isFull(this)) {
										_holder = static_cast<player*>(fixtureAUserData->dataUser);
										_fixtures[0]->SetSensor(true);
										_neverLeftZone = true;
										return;
									}
								}
							}
						}
					}
				}
			}
		}

		if (_holder != nullptr) {
			b2Vec2 direction(_holder->getBody()->getPosition().x - _dropBody->GetPosition().x, _holder->getBody()->getPosition().y - _dropBody->GetPosition().y);
			direction.Normalize();
			float speed = 25;
			_dropBody->SetLinearVelocity(b2Vec2(direction.x * speed, direction.y * speed));

			if (_neverLeftZone) {
				for (b2ContactEdge* ce = _dropBody->GetContactList(); ce != nullptr; ce = ce->next) {
					b2Contact* c = ce->contact;
					userData* fixtureAUserData;
					userData* fixtureBUserData;

					getUserDatas(c, fixtureAUserData, fixtureBUserData);

					if (fixtureAUserData != nullptr && fixtureBUserData != nullptr) {
						if (c->IsTouching()) {
							if (fixtureAUserData->type == DROP) {
								if (fixtureBUserData->type == PLAYER) {
									take();
									return;
								}
							}
							else if (fixtureBUserData->type == DROP) {
								if (fixtureAUserData->type == PLAYER) {
									take();
									return;
								}
							}
						}
					}
				}
			}
		}

	}
}