#pragma once
#include "Box2D/Box2D.h"
#include "Gengine/inputManager.h"
#include "Gengine/spriteSheet.h"
#include <Gengine/camera2D.h>

#include "agent.h"
#include "userData.h"
#include "globals.h"
#include "heldItem.h"

class useRayCastCallback : public b2RayCastCallback {
public:
	float closestFraction;

	float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction) {
		if (fixture->GetUserData() != nullptr) {
			if (static_cast<userData*>(fixture->GetUserData())->type == gameObjectType::TILE) {
				if (closestFraction >= fraction) {
					closestFraction = fraction;
					return fraction;
				}
			}
		}
		return 1;
	}
};

class player: public agent
{
public:
	player();
	~player();

	void init(world* toAdd, glm::vec2 position, glm::vec2 drawDimensions, glm::vec2 collisionDimensions, gengine::color& color, gengine::camera2D* camera, glm::vec4& uvRect = glm::vec4(0, 0, 1, 1));
	void initBody(glm::vec2 position, glm::vec2 dimensions);

	void addDamageLabel(glm::vec2 position, int damage, gengine::color labelColor = gengine::color(255, 0, 0, 255));

	void update();
	void updateMovement();
	void updateHeldItem(heldItem* heldItem);
	void updateArms(float angle,glm::vec2 aimedPoint);
	void rotateForTwoHanded(glm::vec2 aimedPoint);
	void rotateFrontArm(glm::vec2 aimedPoint);
	void rotateBackArm(glm::vec2 aimedPoint);

	void draw(gengine::spriteBatch* spriteBatch);

	inventoryItem* refreshRightEquippedItem(glm::vec2 mousePos);
	inventoryItem* refreshLeftEquippedItem(inventoryItem* rightEquippedItem, glm::vec2 mousePos);
	inventoryItem* getRightEquippedItem();
	inventoryItem* getLeftEquippedItem();

	void debugDraw(gengine::debugRenderer* debugEngine);

	void startContact(userData* collidedWith, userData* thisFixture);
	void endContact(userData* collidedWith, userData* thisFixture);
private:
	int _lastTick;

	bool canJump = false;

	gengine::camera2D* _camera;
	inventoryItem* _draggedItem = nullptr;

	inventoryItem* _rightEquippedItem = nullptr;
	inventoryItem* _rightEquippedLastFrame = nullptr;
	inventoryItem* _leftEquippedItem = nullptr;
	inventoryItem* _leftEquippedLastFrame = nullptr;

	int _sinceLastRightUse;
	int _sinceLastLeftUse;

	bool _aimingRight;
	bool _aimingLeft;
};

