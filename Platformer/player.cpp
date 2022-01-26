#include "player.h"
#include "world.h"
#include "globals.h"
#include "GUI.h"
#include <typeinfo>
#include <spine/extension.h>
#include <glm/gtx/vector_angle.hpp>

player::player() {
	_inventory = new inventory(60);
	_rightHeldItem = new heldItem;
	_leftHeldItem = new heldItem;
}


player::~player()
{
}

void player::init(world* toAdd, glm::vec2 position, glm::vec2 drawDimensions, glm::vec2 collisionDimensions,
	gengine::color& color, gengine::camera2D* camera, glm::vec4& uvRect /* = glm::vec4(0,0,1,1)*/) {
	_drawDimensions = drawDimensions;
	_camera = camera;
	_world = toAdd;
	initBody(position, collisionDimensions);
	_rightHeldItem->init(this, toAdd, true);
	_leftHeldItem->init(this, toAdd, false);
	_sinceLastRightUse = 0;
	_sinceLastLeftUse = 0;

	_xAcceleration = 200.0f;
	_maxXSpeed = 3.5f;
	_maxJumpSpeed = 27.5f;
	_minJumpSpeed = 25.0f;
	_jumpForce = 15.0f;

	spAtlas* atlas = spAtlas_createFromFile("assets/skeletons/humanoid/humanoid.atlas",nullptr);

	spAtlasAttachmentLoader* spAttachmentLoaders = spAtlasAttachmentLoader_create(atlas);

	spSkeletonJson* json = spSkeletonJson_createWithLoader((spAttachmentLoader*)spAttachmentLoaders);
	json->scale = 0.007;
	spSkeletonData* data = spSkeletonJson_readSkeletonDataFile(json, "assets/skeletons/humanoid/humanoid.json");
	_skeleton.init(data, globals::ENTITY_DRAW_LAYER, glm::vec2(0.43f, -0.1f));

	gengine::spriteSheet bone;
	bone.init(gengine::ressourceManager::getTexture("assets/skeletons/humanoid/bone.png"));
	gengine::spriteSheet chest;
	chest.init(gengine::ressourceManager::getTexture("assets/skeletons/humanoid/chest.png"));
	gengine::spriteSheet head;
	head.init(gengine::ressourceManager::getTexture("assets/skeletons/humanoid/head.png"));
	gengine::spriteSheet foot;
	foot.init(gengine::ressourceManager::getTexture("assets/skeletons/humanoid/foot.png"));
	gengine::spriteSheet neck;
	neck.init(gengine::ressourceManager::getTexture("assets/skeletons/humanoid/neck.png"));
	gengine::spriteSheet none;
	none.init(gengine::ressourceManager::getTexture("assets/skeletons/humanoid/empty.png"));

	_skeleton.setMix("idle", "run", 0.2);
	_skeleton.setMix("idle", "run backward", 0.2);
	_skeleton.setMix("run", "idle", 0.2);
	_skeleton.setMix("run backward", "idle", 0.2);
	_skeleton.setMix("idle", "rise", 0.2);
	_skeleton.setMix("run", "rise", 0.2);
	_skeleton.setMix("run backward", "rise", 0.2);
	_skeleton.setMix("rise", "fall", 0.5);
	_skeleton.setMix("fall", "panic fall", 0.5);
	_skeleton.setMix("fall", "rise", 0.5);
	_skeleton.setMix("panic fall", "rise", 0.5);
	_skeleton.setMix("panic fall", "fall", 0.5);
	_skeleton.setMix("land", "idle", 0.2);
	_skeleton.setMix("land", "run", 0.2);
	_skeleton.setMix("land", "run backward", 0.2);
	_skeleton.setMix("run backward", "run", 0.2);

	_skeleton.newBone("front arm", bone);
	_skeleton.newBone("front forarm", bone);
	_skeleton.newBone("front hand", bone);
	_skeleton.newBone("back arm", bone);
	_skeleton.newBone("back forarm", bone);
	_skeleton.newBone("back hand", bone);
	_skeleton.newBone("chest", chest);
	_skeleton.newBone("neck", neck);
	_skeleton.newBone("head", head);
	_skeleton.newBone("front shin", bone);
	_skeleton.newBone("front thigh", bone);
	_skeleton.newBone("front foot", foot);
	_skeleton.newBone("back shin", bone);
	_skeleton.newBone("back thigh", bone);
	_skeleton.newBone("back foot", foot);
	_skeleton.newBone("back weapon", none);
	_skeleton.newBone("front weapon", none);

	_skeleton.setAnimation("idle", true, 0, false);
}

void player::initBody(glm::vec2 position,glm::vec2 dimensions) {
	_body = new body();

	std::vector<b2FixtureDef> fixtures;
	glm::vec2 cornerDifference = glm::vec2(globals::pixToMet(globals::TILE_SIZE) / 5, globals::pixToMet(globals::TILE_SIZE) / 5);

	b2BodyDef bodyDef;
	bodyDef.fixedRotation = true;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(position.x, position.y);
	bodyDef.allowSleep = false;

	b2PolygonShape chainHitbox = _body->makeCapsule(dimensions, cornerDifference);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &chainHitbox;
	fixtureDef.userData = new userData(this, gameObjectType::PLAYER);
	fixtureDef.filter.categoryBits = globals::PLAYER_BIT;
	fixtureDef.filter.maskBits = globals::DROP_BIT | globals::TILE_BIT | globals::PROJECTILE_BIT | globals::HELDITEM_BIT | globals::SOLID_BODY_BIT;
	fixtureDef.filter.groupIndex = -1;

	fixtures.push_back(fixtureDef);


	//Foot sensor
	b2PolygonShape rectShape;
	rectShape.SetAsBox(dimensions.x / 2 - cornerDifference.x, cornerDifference.y / 2 + 0.05, b2Vec2(0, -dimensions.y / 2 + cornerDifference.y / 2), 0);

	b2FixtureDef footFixtureDef;
	footFixtureDef.shape = &rectShape;
	footFixtureDef.userData = new userData(this, gameObjectType::FOOT_SENSOR);
	footFixtureDef.isSensor = true;
	fixtures.push_back(footFixtureDef);

	rectShape.SetAsBox(dimensions.x / 2 - cornerDifference.x, cornerDifference.y / 2, b2Vec2(0, -dimensions.y / 2 + cornerDifference.y / 2), 0);

	b2FixtureDef footFixtureDef2;
	footFixtureDef2.shape = &rectShape;
	footFixtureDef2.userData = new userData(this, gameObjectType::FOOT_SENSOR);
	footFixtureDef2.filter.categoryBits = globals::FOOT_FIXTURE_BIT;
	footFixtureDef2.filter.maskBits = globals::PLATFORM_BIT;
	fixtures.push_back(footFixtureDef2);

	_body->createBody(_world, bodyDef, fixtures, b2Vec2(0,0));

}

void player::update() {
	updatePlatformFiltering();
	updateMovement();
	updateGroundState();
	updateAnimations();

	glm::vec2 bodyPos(_body->getPosition().x, _body->getPosition().y);
	glm::vec2 dimensions = _body->getDimensions();

	glm::vec2 mousePosition = gengine::inputManager::getMouseCoords();
	glm::vec2 worldMousePosition = _camera->convertScreenToWorld(mousePosition);

	glm::vec2 distanceVec(worldMousePosition.x - bodyPos.x, worldMousePosition.y - bodyPos.y);
	float distance = glm::length(distanceVec);

	int pressedNum = gengine::inputManager::numberPressed();
	if (pressedNum != -1) {
		if (pressedNum == 10) {
			pressedNum = 9;
		}

		if (gengine::inputManager::isKeyDown(SDLK_LSHIFT)) {
			if (_rightEquippedIndex != pressedNum) {
				_leftEquippedIndex = pressedNum;
			}
		}
		else {
			if (_leftEquippedIndex != pressedNum) {
				_rightEquippedIndex = pressedNum;
			}
		}
	}

	inventoryItem* rightItemToUse = refreshRightEquippedItem(worldMousePosition);
	inventoryItem* leftItemToUse = refreshLeftEquippedItem(rightItemToUse, worldMousePosition);

	if (rightItemToUse != nullptr) {
		if (rightItemToUse->getItemDefinition()->isTwoHanded()) {
			if (_sinceLastRightUse >= rightItemToUse->getItemDefinition()->getUseDelay()) {
				if (gengine::inputManager::isKeyDown(SDL_BUTTON_LEFT) && gengine::inputManager::wasKeyDown(SDL_BUTTON_LEFT)) {
					if (rightItemToUse->primaryUse(_rightHeldItem, worldMousePosition, _world)) {
						_skeleton.setAnimation("use both", "use both", false, 1, true);
					}
					_sinceLastRightUse = 0;
				}
				else if (gengine::inputManager::isKeyDown(SDL_BUTTON_RIGHT) && gengine::inputManager::wasKeyDown(SDL_BUTTON_RIGHT)) {
					if (rightItemToUse->secondaryUse(_rightHeldItem, worldMousePosition, _world)) {
						_skeleton.setAnimation("use both", "use both", false, 1, true);
					}
					_sinceLastRightUse = 0;
				}
			}
		}
		else {
			if (_sinceLastRightUse >= rightItemToUse->getItemDefinition()->getUseDelay()) {
				if (gengine::inputManager::isKeyDown(SDL_BUTTON_RIGHT) && gengine::inputManager::wasKeyDown(SDL_BUTTON_RIGHT)) {
					if (rightItemToUse->secondaryUse(_rightHeldItem, worldMousePosition, _world)) {
						_skeleton.setAnimation("use front", "use back", false, 1, true);
					}
					_sinceLastRightUse = 0;
				}
			}
		}
	}

	if (leftItemToUse != nullptr) {
		if (_sinceLastLeftUse >= leftItemToUse->getItemDefinition()->getUseDelay()) {
			if (gengine::inputManager::isKeyDown(SDL_BUTTON_LEFT) && gengine::inputManager::wasKeyDown(SDL_BUTTON_LEFT)) {
				if (leftItemToUse->primaryUse(_leftHeldItem, worldMousePosition, _world)) {
					_skeleton.setAnimation("use back", "use front", false, 1);
				}
				_sinceLastLeftUse = 0;
			}
		}
	}

	if (rightItemToUse != nullptr) {
		_aimingRight = true;
	}
	else {
		_aimingRight = false;
	}
	if (leftItemToUse != nullptr) {
		_aimingLeft = true;
	}
	else {
		_aimingLeft = false;
	}

	float armAngle = atan2(glm::normalize(distanceVec).y, glm::normalize(distanceVec).x);
	if (armAngle < 0) {
		armAngle += M_PI * 2;
	}

	rightItemToUse = getRightEquippedItem();
	leftItemToUse = getLeftEquippedItem();

	if (rightItemToUse != nullptr) {
		rightItemToUse->getItemDefinition()->equipped(_rightHeldItem, worldMousePosition, _world);
	}

	if (leftItemToUse != nullptr) {
		leftItemToUse->getItemDefinition()->equipped(_leftHeldItem, worldMousePosition, _world);
	}

	_skeleton.updateBegin(_body->getPosition() - _body->getDimensions() / 2.0f, SDL_GetTicks() - _lastTick);
	updateArms(armAngle, worldMousePosition);
	_skeleton.updateEnd();

	if (_facingRight) {
		updateHeldItem(_rightHeldItem);
		updateHeldItem(_leftHeldItem);
	}
	else {
		updateHeldItem(_rightHeldItem);
		updateHeldItem(_leftHeldItem);
	}

	_sinceLastRightUse += SDL_GetTicks() - _lastTick;
	_sinceLastLeftUse += SDL_GetTicks() - _lastTick;
	_lastTick = SDL_GetTicks();
	
	//Drop item
	if(gengine::inputManager::keyPressed(SDLK_q)){
		if(_rightEquippedItem != nullptr){
			glm::vec3 handInfo;

			if (_facingRight) {
				handInfo = _skeleton.getBonePosInfo("front weapon");
			}
			else {
				handInfo = _skeleton.getBonePosInfo("back weapon");
			}

			b2Vec2 direction(worldMousePosition.x - bodyPos.x, worldMousePosition.y - bodyPos.y);
			direction.Normalize();
			float speed = 6.0f;
			
			rightItemToUse->drop(1, glm::vec2(handInfo.x, handInfo.y),_world, b2Vec2(direction.x * speed, direction.y * speed));
		}		
	}

	if (gengine::inputManager::keyPressed(SDLK_i)) {
		GUI::openPlayerInventory(_inventory);
	}

	if (gengine::inputManager::keyPressed(SDLK_e)) {
		if (distance <= globals::MAX_TILE_RANGE) {
			useRayCastCallback rayCastCallback;
			rayCastCallback.closestFraction = 1;
			_world->getB2World()->RayCast(&rayCastCallback, b2Vec2(_body->getPosition().x, _body->getPosition().y), b2Vec2(worldMousePosition.x, worldMousePosition.y));
			if (rayCastCallback.closestFraction == 1) {
				tile* toUse = _world->getTile(worldMousePosition);
				if (toUse != nullptr) {
					toUse->use(this);
				}
			}
		}
	}

	_camera->setPosition(glm::vec2(_body->getPosition().x, _body->getPosition().y));

	glm::vec2 cameraDimensions = _camera->getDimensions() / _camera->getScale();

	if (!_isWalking) {
		if (_groundState == groundState::ground) {
			_slow();
		}
	}
	_wasWalking = _isWalking;
	_isWalking = false;
}

void player::updateMovement(){
	if (gengine::inputManager::isKeyDown(SDLK_a)) {
		_moveLeft();
	}
	else if (gengine::inputManager::isKeyDown(SDLK_d)) {
		_moveRight();
	}
	
	if (gengine::inputManager::isKeyDown(SDLK_SPACE)) {
		_body->applyForceToCenter(b2Vec2(0,100));
	}

	if (gengine::inputManager::keyPressed(SDLK_w)) {
		_jump();
	}

	if (gengine::inputManager::isKeyDown(SDLK_SPACE) && gengine::inputManager::isKeyDown(SDLK_s)) {
		b2Filter footFixtureFilter = _body->getFixture(2)->GetFilterData();
		footFixtureFilter.maskBits = 0;
		_body->getFixture(2)->SetFilterData(footFixtureFilter);
	}
	
	if (_groundState == jumping) {

		if (!gengine::inputManager::isKeyDown(SDLK_w)) {
			if (_body->getLinearVelocity().y >= _minJumpSpeed && _body->getLinearVelocity().y <= _maxJumpSpeed) {
				_releaseJump();
			}
		}
	}

}

void player::updateHeldItem(heldItem* heldItem) {
	bool rightHand = heldItem->isHeldInRightHand();
	bool twoHanded = false;

	glm::vec3 handInfo;
	if (rightHand) {
		if (heldItem->getItem() != nullptr) {
			twoHanded = heldItem->getItem()->getItemDefinition()->isTwoHanded();
		}

		if (_facingRight){
			handInfo = _skeleton.getBonePosInfo("front weapon");
		}
		else {
			if (twoHanded) {
				handInfo = _skeleton.getBonePosInfo("front weapon");
			}
			else {
				handInfo = _skeleton.getBonePosInfo("back weapon");
			}
		}
	}
	else {
		if (_facingRight){
			handInfo = _skeleton.getBonePosInfo("back weapon");
		}
		else {
			handInfo = _skeleton.getBonePosInfo("front weapon");
		}
	}
	if (_facingRight) {
		heldItem->setRightDir(true);
		heldItem->rotate(handInfo.z + M_PI);
	}
	else {
		heldItem->setRightDir(false);
		heldItem->rotate(handInfo.z + M_PI);
	}

	heldItem->setPosition(glm::vec2(handInfo.x, handInfo.y));
	heldItem->update();
}

void player::updateArms(float angle, glm::vec2 aimedPoint) {
	if (_aimingLeft || _aimingRight) {
		if (angle >= M_PI / 2 && angle <= 3 * M_PI / 2) {
			_facingRight = false;
			_skeleton.setDirection(1);
		}
		else {
			_facingRight = true;
			_skeleton.setDirection(0);
		}
	}
	else {
		if (_walkingRight) {
			_facingRight = true;
			_skeleton.setDirection(0);
		}
		else {
			_facingRight = false;
			_skeleton.setDirection(1);
		}
	}

	if (_rightEquippedItem != nullptr) {
		if (_skeleton.getAnimationNames(1).first == "") {
			if (_rightEquippedItem->getItemDefinition()->isTwoHanded()) {
				_skeleton.setAnimation("holding two-handed", false, 1, true);
			}
			else {
				if (_aimingRight) {
					_skeleton.setAnimation("holding front", "holding back", false, 1, true);
				}
			}
		}
	}

	if (_leftEquippedItem != nullptr) {
		if (_skeleton.getAnimationNames(2).first == "") {
			if (_aimingLeft) {
				_skeleton.setAnimation("holding back", "holding front", false, 2, false);
			}
		}
	}

	if (_rightEquippedItem != nullptr) {
		//Chest
		{
			float chestAngle;

			if (angle >= M_PI / 2 && angle <= 3 * M_PI / 2) {
				chestAngle = (angle - M_PI) * 0.1;
			}
			else if (angle > M_PI * 3 / 2) {
				chestAngle = (angle - M_PI * 2) * 0.1;
			}
			else {
				chestAngle = angle * 0.1;
			}

			_skeleton.forceAdditiveRotate("chest", chestAngle * 57.2958, 0);
		}
		//Head
		{
			float headAngle;

			if (angle >= M_PI / 2 && angle <= 3 * M_PI / 2) {
				headAngle = (angle - M_PI) * 0.5;
			}
			else if (angle > M_PI * 3 / 2) {
				headAngle = (angle - M_PI * 2) * 0.5;
			}
			else {
				headAngle = angle * 0.5;
			}

			_skeleton.forceAbsoluteRotate("head", headAngle * 57.2958 + 90);
		}

		if (_rightEquippedItem->getItemDefinition()->isTwoHanded()) {
			rotateForTwoHanded(aimedPoint);
		}
		else {
			if (_aimingRight) {
				if (_facingRight) {
					rotateFrontArm(aimedPoint);
				}
				else {
					rotateBackArm(aimedPoint);

				}
			}
		}
	}

	if (_aimingLeft) {
		if (_facingRight) {
			rotateBackArm(aimedPoint);
		}
		else {
			rotateFrontArm(aimedPoint);

		}
	}

}

void player::rotateForTwoHanded(glm::vec2 aimedPoint) { 
	if (_skeleton.getSkeleton()->flipX == 0) {
		if (_skeleton.getAnimationNames(1).first == "holding two-handed" || _skeleton.getAnimationNames(1).first == "") {
			//Front Arm
			glm::vec3 frontArmInfo = _skeleton.getBonePosInfo("front arm");
			float frontArmAngle = atan2(aimedPoint.y - frontArmInfo.y, aimedPoint.x - frontArmInfo.x);
			_skeleton.forceAbsoluteRotate("front arm", frontArmAngle * 57.2958 + _rightHeldItem->getArmAngleOffset());


			//Back Arm	
			glm::vec3 backArmInfo = _skeleton.getBonePosInfo("back arm");
			float backArmAngle = atan2(aimedPoint.y - backArmInfo.y, aimedPoint.x - backArmInfo.x);
			_skeleton.forceAbsoluteRotate("back arm", backArmAngle * 57.2958 + _rightHeldItem->getArmAngleOffset());
		}
		else {
			//Front Arm
			glm::vec3 frontArmInfo = _skeleton.getBonePosInfo("front arm");
			float frontArmAngle = atan2(aimedPoint.y - frontArmInfo.y, aimedPoint.x - frontArmInfo.x);
			_skeleton.forceAdditiveRotate("front arm", frontArmAngle * 57.2958 + 120 + _rightHeldItem->getArmAngleOffset(), -120);

			//Back Arm	
			glm::vec3 backArmInfo = _skeleton.getBonePosInfo("back arm");
			float backArmAngle = atan2(aimedPoint.y - backArmInfo.y, aimedPoint.x - backArmInfo.x);
			_skeleton.forceAdditiveRotate("back arm", backArmAngle * 57.2958 + 120 + _rightHeldItem->getArmAngleOffset(), -120);
		}
	}
	else {
		if (_skeleton.getAnimationNames(1).first == "holding two-handed" || _skeleton.getAnimationNames(1).first == "") {
			//Front Arm
			glm::vec3 frontArmInfo = _skeleton.getBonePosInfo("front arm");
			float frontArmAngle = atan2(aimedPoint.y - frontArmInfo.y, aimedPoint.x - frontArmInfo.x);
			_skeleton.forceAbsoluteRotate("front arm", frontArmAngle * 57.2958 - _rightHeldItem->getArmAngleOffset());


			//Back Arm	
			glm::vec3 backArmInfo = _skeleton.getBonePosInfo("back arm");
			float backArmAngle = atan2(aimedPoint.y - backArmInfo.y, aimedPoint.x - backArmInfo.x);
			_skeleton.forceAbsoluteRotate("back arm", backArmAngle * 57.2958 - _rightHeldItem->getArmAngleOffset());
		}
		else {
			//Front Arm
			glm::vec3 frontArmInfo = _skeleton.getBonePosInfo("front arm");
			float frontArmAngle = atan2(aimedPoint.y - frontArmInfo.y, aimedPoint.x - frontArmInfo.x);
			_skeleton.forceAdditiveRotate("front arm", frontArmAngle * 57.2958 + 120 - _rightHeldItem->getArmAngleOffset(), -120);

			//Back Arm	
			glm::vec3 backArmInfo = _skeleton.getBonePosInfo("back arm");
			float backArmAngle = atan2(aimedPoint.y - backArmInfo.y, aimedPoint.x - backArmInfo.x);
			_skeleton.forceAdditiveRotate("back arm", backArmAngle * 57.2958 + 120 - _rightHeldItem->getArmAngleOffset(), -120);
		}
	}
}

void player::rotateFrontArm(glm::vec2 aimedPoint) {
	if (_skeleton.getSkeleton()->flipX == 0) {
		if (_skeleton.getAnimationNames(1).first == "holding front" || _skeleton.getAnimationNames(1).first == "") {
			glm::vec3 frontArmInfo = _skeleton.getBonePosInfo("front arm");
			float frontArmAngle = atan2(aimedPoint.y - frontArmInfo.y, aimedPoint.x - frontArmInfo.x);

			_skeleton.forceAbsoluteRotate("front arm", frontArmAngle * 57.2958 + _rightHeldItem->getArmAngleOffset());
		}
		else {
			glm::vec3 frontArmInfo = _skeleton.getBonePosInfo("front arm");
			float frontArmAngle = atan2(aimedPoint.y - frontArmInfo.y, aimedPoint.x - frontArmInfo.x);

			_skeleton.forceAdditiveRotate("front arm", frontArmAngle * 57.2958 + 109 + _rightHeldItem->getArmAngleOffset(), -109);
		}
	}
	else {
		if (_skeleton.getAnimationNames(2).second == "holding front" || _skeleton.getAnimationNames(2).second == "") {
			glm::vec3 frontArmInfo = _skeleton.getBonePosInfo("front arm");
			float frontArmAngle = atan2(aimedPoint.y - frontArmInfo.y, aimedPoint.x - frontArmInfo.x);

			_skeleton.forceAbsoluteRotate("front arm", frontArmAngle * 57.2958 - _leftHeldItem->getArmAngleOffset());
		}
		else {
			glm::vec3 frontArmInfo = _skeleton.getBonePosInfo("front arm");
			float frontArmAngle = atan2(aimedPoint.y - frontArmInfo.y, aimedPoint.x - frontArmInfo.x);

			_skeleton.forceAdditiveRotate("front arm", frontArmAngle * 57.2958 + 125 - _leftHeldItem->getArmAngleOffset(), - 125);
		}
	}
}
void player::rotateBackArm(glm::vec2 aimedPoint) {
	if (_skeleton.getSkeleton()->flipX == 0) {
		if (_skeleton.getAnimationNames(2).first == "holding back" || _skeleton.getAnimationNames(2).first == "") {
			glm::vec3 backArmInfo = _skeleton.getBonePosInfo("back arm");
			float backArmAngle = atan2(aimedPoint.y - backArmInfo.y, aimedPoint.x - backArmInfo.x);

			_skeleton.forceAbsoluteRotate("back arm", backArmAngle * 57.2958 + _leftHeldItem->getArmAngleOffset());
		}
		else {
			glm::vec3 backArmInfo = _skeleton.getBonePosInfo("back arm");
			float backArmAngle = atan2(aimedPoint.y - backArmInfo.y, aimedPoint.x - backArmInfo.x);

			_skeleton.forceAdditiveRotate("back arm", backArmAngle * 57.2958 + 120 + _leftHeldItem->getArmAngleOffset(), -120);
		}
	}
	else {
		if (_skeleton.getAnimationNames(1).second == "holding back" || _skeleton.getAnimationNames(1).second == "") {
			glm::vec3 backArmInfo = _skeleton.getBonePosInfo("back arm");
			float backArmAngle = atan2(aimedPoint.y - backArmInfo.y, aimedPoint.x - backArmInfo.x);

			_skeleton.forceAbsoluteRotate("back arm", backArmAngle * 57.2958 - _rightHeldItem->getArmAngleOffset());
		}
		else {
			glm::vec3 backArmInfo = _skeleton.getBonePosInfo("back arm");
			float backArmAngle = atan2(aimedPoint.y - backArmInfo.y, aimedPoint.x - backArmInfo.x);

			_skeleton.forceAdditiveRotate("back arm", backArmAngle * 57.2958 + 120 - _rightHeldItem->getArmAngleOffset(), -120);
		}
	}
}

void player::addDamageLabel(glm::vec2 position, int damage, gengine::color labelColor) {
	_world->addDamageLabel(position, damage, labelColor);
}

inventoryItem* player::refreshRightEquippedItem(glm::vec2 mousePos) {
	_draggedItem = GUI::getDraggedItem();
	_rightEquippedItem = _inventory->getItem(_rightEquippedIndex);

	inventoryItem* itemToUse;

	if (_draggedItem == nullptr) {
		itemToUse = _rightEquippedItem;
	}
	else {
		itemToUse = _draggedItem;
	}

	if (_rightEquippedLastFrame != itemToUse) {
		_rightHeldItem->changeItem(itemToUse, mousePos, _world);
		_rightEquippedLastFrame = itemToUse;
	}

	return itemToUse;
}
inventoryItem* player::refreshLeftEquippedItem(inventoryItem* rightEquippedItem,glm::vec2 mousePos) {
	_leftEquippedItem = _inventory->getItem(_leftEquippedIndex);

	if (rightEquippedItem != nullptr) {
		if ((rightEquippedItem->getItemDefinition()->isTwoHanded())) {
			_leftEquippedItem = nullptr;
		}
		else {
			if (_leftEquippedItem != nullptr) {
				if (_leftEquippedItem->getItemDefinition()->isTwoHanded()) {
					_leftEquippedItem = nullptr;
				}
			}
		}
	}


	if (_leftEquippedLastFrame != _leftEquippedItem) {
		_leftHeldItem->changeItem(_leftEquippedItem, mousePos, _world);
		_leftEquippedLastFrame = _leftEquippedItem;
	}
	return _leftEquippedItem;
}

inventoryItem* player::getRightEquippedItem(){
	_draggedItem = GUI::getDraggedItem();

	inventoryItem* itemToUse;

	if (_draggedItem == nullptr) {
		itemToUse = _rightEquippedItem;
	}
	else {
		itemToUse = _draggedItem;
	}
	return itemToUse;
}
inventoryItem* player::getLeftEquippedItem() {
	return _leftEquippedItem;
}

void player::draw(gengine::spriteBatch* spriteBatch) {
	_skeleton.draw(spriteBatch);

	inventoryItem* rightItemToUse = getRightEquippedItem();
	inventoryItem* leftItemToUse = getLeftEquippedItem();

	glm::vec2 mousePosition = gengine::inputManager::getMouseCoords();
	glm::vec2 worldMousePosition = _camera->convertScreenToWorld(mousePosition);

	if (rightItemToUse != nullptr) {	
		rightItemToUse->getItemDefinition()->draw(spriteBatch, _rightHeldItem, worldMousePosition, _world);

		if (_facingRight) {
			_rightHeldItem->draw(spriteBatch, _skeleton.getDrawLayer() + 1);
		}
		else {
			if (rightItemToUse->getItemDefinition()->isTwoHanded()) {
				_rightHeldItem->draw(spriteBatch, _skeleton.getDrawLayer() + 1);
			}
			else {
				_rightHeldItem->draw(spriteBatch, _skeleton.getDrawLayer() - 1);
			}
		}
	}
	if (leftItemToUse != nullptr) {
		leftItemToUse->getItemDefinition()->draw(spriteBatch, _leftHeldItem, worldMousePosition, _world);

		if (_facingRight) {
			_leftHeldItem->draw(spriteBatch, _skeleton.getDrawLayer() - 1);
		}
		else {
			_leftHeldItem->draw(spriteBatch, _skeleton.getDrawLayer() + 1);
		}
	}

}

void player::debugDraw(gengine::debugRenderer* debugRenderer) {
	_body->debugDraw(debugRenderer);
}

void player::startContact(userData* collidedWith, userData* thisFixture) {

}

void player::endContact(userData* collidedWith, userData* thisFixture) {

}