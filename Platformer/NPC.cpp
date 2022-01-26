#include "npc.h"
#include "world.h"

npc::npc()
{
	_inventory = new inventory(60);
	_rightHeldItem = new heldItem;
	_leftHeldItem = new heldItem;
}


npc::~npc()
{
}


void npc::init(world* toAdd, glm::vec2 position, glm::vec2 dimensions, npcType npctype, std::string nameID) {
	_world = toAdd;
	initBody(position, dimensions);

	_xAcceleration = luaHandler::getTableFloat(nameID,"xAcceleration");
	_maxXSpeed = luaHandler::getTableFloat(nameID, "maxXSpeed");;
	_maxJumpSpeed = 27.5f;
	_minJumpSpeed = 25.0f;
	_jumpForce = luaHandler::getTableFloat(nameID, "jumpForce");

	_npcType = npctype;
	_nameID = nameID;

	if (_npcType == npcType::humanoid) {
		spAtlas* atlas = spAtlas_createFromFile("assets/skeletons/humanoid/humanoid.atlas", nullptr);

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
	else if (_npcType == flying) {

	}
	else if (_npcType == fourLegged) {

	}
	else if (_npcType == twoLegged) {

	}
	lua_State* S = luaHandler::getLuaState();

	agent* pushedAgent = this;

	lua_getglobal(S, _nameID.c_str());
	lua_pushstring(S, "onSpawn");
	lua_gettable(S, -2);
	lua_pushvalue(S, -2);
	lua_pushlightuserdata(S, &pushedAgent);
	luaL_getmetatable(S, "agent_m");
	lua_setmetatable(S, -2);

	lua_pcall(S, 2, 0, 0);
	lua_pop(S, 1);
}

void npc::initBody(glm::vec2 position, glm::vec2 dimensions) {
	_body = new body();

	std::vector<b2FixtureDef> fixtures;
	glm::vec2 cornerDifference = glm::vec2(globals::pixToMet(globals::TILE_SIZE) / 3, globals::pixToMet(globals::TILE_SIZE) / 3);

	b2BodyDef bodyDef;
	bodyDef.fixedRotation = true;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(position.x, position.y);

	b2PolygonShape chainHitbox = _body->makeCapsule(dimensions, cornerDifference);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &chainHitbox;
	fixtureDef.userData = new userData(this, gameObjectType::NPC);
	fixtureDef.filter.categoryBits = globals::bitData::NPC_BIT;
	fixtureDef.filter.maskBits = globals::bitData::DROP_BIT | globals::bitData::TILE_BIT | globals::bitData::PROJECTILE_BIT | globals::bitData::HELDITEM_BIT | globals::bitData::SOLID_BODY_BIT | globals::bitData::PLATFORM_BIT;
	fixtureDef.filter.groupIndex = -1;

	fixtures.push_back(fixtureDef);


	//Foot sensor
	b2PolygonShape rectShape;
	rectShape.SetAsBox(dimensions.x / 2 - cornerDifference.x, cornerDifference.y / 2 + 0.05, b2Vec2(0, -dimensions.y / 2 + cornerDifference.y / 2) , 0);

	b2FixtureDef footFixtureDef;
	footFixtureDef.shape = &rectShape;
	footFixtureDef.userData = new userData(this, gameObjectType::FOOT_SENSOR);
	footFixtureDef.isSensor = true;
	fixtures.push_back(footFixtureDef);

	b2FixtureDef footFixtureDef2;
	footFixtureDef2.shape = &rectShape;
	footFixtureDef2.userData = new userData(this, gameObjectType::FOOT_SENSOR);
	footFixtureDef2.filter.categoryBits = globals::bitData::FOOT_FIXTURE_BIT;
	footFixtureDef2.filter.maskBits = globals::bitData::PLATFORM_BIT;
	fixtures.push_back(footFixtureDef2);

	_body->createBody(_world, bodyDef, fixtures, b2Vec2(0, 0));

}

void npc::update() {
	updatePlatformFiltering();
	updateAI();
	updateGroundState();
	updateAnimations();


	if (_npcType == humanoid) {
		inventoryItem* rightItemToUse = refreshRightEquippedItem(aimedPoint);
		inventoryItem* leftItemToUse = refreshLeftEquippedItem(rightItemToUse, aimedPoint);

		/*if (rightItemToUse != nullptr) {
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
		}*/

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

		float armAngle = atan2(aimedPoint.y - _position.x, aimedPoint.x - _position.x);
		if (armAngle < 0) {
			armAngle += M_PI * 2;
		}

		rightItemToUse = getRightEquippedItem();
		leftItemToUse = getLeftEquippedItem();

		if (rightItemToUse != nullptr) {
			rightItemToUse->getItemDefinition()->equipped(_rightHeldItem, aimedPoint, _world);
		}

		if (leftItemToUse != nullptr) {
			leftItemToUse->getItemDefinition()->equipped(_leftHeldItem, aimedPoint, _world);
		}

		_skeleton.updateBegin(_body->getPosition() - _body->getDimensions() / 2.0f, SDL_GetTicks() - _lastTick);
		updateArms(armAngle, aimedPoint);
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
	}

	if (!_isWalking) {
		if (_groundState == groundState::ground) {
			_slow();
		}
	}

	_wasWalking = _isWalking;
	_isWalking = false;

	if (_invulnerabilityTimer > 0) {
		_invulnerabilityTimer -= SDL_GetTicks() - _lastTick;
	}
	_lastTick = SDL_GetTicks();
}

void npc::updateAI() {
	lua_State* S = luaHandler::getLuaState();

	agent* pushedAgent = this;

	lua_getglobal(S, _nameID.c_str());
	lua_pushstring(S, "update");
	lua_gettable(S, -2);
	lua_pushvalue(S, -2);
	lua_pushlightuserdata(S, &pushedAgent);
	luaL_getmetatable(S, "agent_m");
	lua_setmetatable(S, -2);

	lua_pcall(S, 2, 0, 0);
	lua_pop(S, 1);

	for (int i = 0; i < _activeBehaviours.size(); i++) {
		lua_getglobal(S, _activeBehaviours[i].c_str());
		lua_pushstring(S, "whileActive");
		lua_gettable(S, -2);
		lua_pushvalue(S, -2);
		lua_pushlightuserdata(S, &pushedAgent);
		luaL_getmetatable(S, "agent_m");
		lua_setmetatable(S, -2);

		lua_pcall(S, 2, 0, 0);
		lua_pop(S, 1);
	}
}

void npc::updateHeldItem(heldItem* heldItem) {
	bool rightHand = heldItem->isHeldInRightHand();
	bool twoHanded = false;

	glm::vec3 handInfo;
	if (rightHand) {
		if (heldItem->getItem() != nullptr) {
			twoHanded = heldItem->getItem()->getItemDefinition()->isTwoHanded();
		}

		if (_facingRight) {
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
		if (_facingRight) {
			handInfo = _skeleton.getBonePosInfo("back weapon");
		}
		else {
			handInfo = _skeleton.getBonePosInfo("front weapon");
		}
	}
	heldItem->setPosition(glm::vec2(handInfo.x, handInfo.y));
	heldItem->update();

	if (_facingRight) {
		heldItem->setRightDir(true);
		heldItem->rotate(handInfo.z + M_PI);
	}
	else {
		heldItem->setRightDir(false);
		heldItem->rotate(handInfo.z + M_PI);
	}
}

void npc::updateArms(float angle, glm::vec2 aimedPoint) {
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
				_skeleton.setAnimation("holding two-handed", false, 1, false);
			}
			else {
				if (_aimingRight && _aimingLeft) {
					_skeleton.setAnimation("holding both", "holding both", false, 1, false);
				}
				else {
					if (_aimingRight) {
						_skeleton.setAnimation("holding front", "holding back", false, 1, false);
					}
					if (_aimingLeft) {
						_skeleton.setAnimation("holding back", "holding front", false, 1, false);
					}
				}

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
				headAngle = (angle - M_PI) * 0.3;
			}
			else if (angle > M_PI * 3 / 2) {
				headAngle = (angle - M_PI * 2) * 0.3;
			}
			else {
				headAngle = angle * 0.3;
			}

			_skeleton.forceAdditiveRotate("head", headAngle * 57.2958 + 90, 0);
		}

		if (_rightEquippedItem->getItemDefinition()->isTwoHanded()) {
			rotateForTwoHanded(angle, aimedPoint);
		}
		else {
			if (_aimingRight) {
				if (_facingRight) {
					rotateFrontArm(angle, aimedPoint);

				}
				else {
					rotateBackArm(angle, aimedPoint);

				}
			}
			if (_aimingLeft) {
				if (_facingRight) {
					rotateBackArm(angle, aimedPoint);

				}
				else {
					rotateFrontArm(angle, aimedPoint);

				}
			}
		}
	}


}

void npc::rotateForTwoHanded(float angle, glm::vec2 aimedPoint) {
	if (_skeleton.getAnimationNames(1).first == "holding two-handed") {
		//Front Arm
		glm::vec3 frontArmInfo = _skeleton.getBonePosInfo("front arm");
		float frontArmAngle = atan2(aimedPoint.y - frontArmInfo.y, aimedPoint.x - frontArmInfo.x);
		_skeleton.forceAbsoluteRotate("front arm", frontArmAngle * 57.2958);

		//Back Arm	
		glm::vec3 backArmInfo = _skeleton.getBonePosInfo("back arm");
		float backArmAngle = atan2(aimedPoint.y - backArmInfo.y, aimedPoint.x - backArmInfo.x);
		_skeleton.forceAbsoluteRotate("back arm", backArmAngle * 57.2958);
	}
	else {
		//Front Arm
		glm::vec3 frontArmInfo = _skeleton.getBonePosInfo("front arm");
		float frontArmAngle = atan2(aimedPoint.y - frontArmInfo.y, aimedPoint.x - frontArmInfo.x);
		_skeleton.forceAdditiveRotate("front arm", frontArmAngle * 57.2958 + 120, -120);

		//Back Arm	
		glm::vec3 backArmInfo = _skeleton.getBonePosInfo("back arm");
		float backArmAngle = atan2(aimedPoint.y - backArmInfo.y, aimedPoint.x - backArmInfo.x);
		_skeleton.forceAdditiveRotate("back arm", backArmAngle * 57.2958 + 120, -120);
	}
}

void npc::rotateFrontArm(float angle, glm::vec2 aimedPoint) {
	if (_skeleton.getAnimationNames(1).first == "holding front") {
		glm::vec3 frontArmInfo = _skeleton.getBonePosInfo("front arm");
		float frontArmAngle = atan2(aimedPoint.y - frontArmInfo.y, aimedPoint.x - frontArmInfo.x);

		_skeleton.forceAbsoluteRotate("front arm", frontArmAngle * 57.2958);
	}
	else {
		glm::vec3 frontArmInfo = _skeleton.getBonePosInfo("front arm");
		float frontArmAngle = atan2(aimedPoint.y - frontArmInfo.y, aimedPoint.x - frontArmInfo.x);

		_skeleton.forceAdditiveRotate("front arm", frontArmAngle * 57.2958 + 120, -120);
	}
}
void npc::rotateBackArm(float angle, glm::vec2 aimedPoint) {
	if (_skeleton.getAnimationNames(1).first == "holding back") {
		glm::vec3 backArmInfo = _skeleton.getBonePosInfo("back arm");
		float backArmAngle = atan2(aimedPoint.y - backArmInfo.y, aimedPoint.x - backArmInfo.x);

		_skeleton.forceAbsoluteRotate("back arm", backArmAngle * 57.2958);
	}
	else {
		glm::vec3 backArmInfo = _skeleton.getBonePosInfo("back arm");
		float backArmAngle = atan2(aimedPoint.y - backArmInfo.y, aimedPoint.x - backArmInfo.x);

		_skeleton.forceAdditiveRotate("back arm", backArmAngle * 57.2958 + 120, -120);
	}
}

inventoryItem* npc::refreshRightEquippedItem(glm::vec2 mousePos) {
	_rightEquippedItem = _inventory->getItem(_rightEquippedIndex);

	if (_rightEquippedLastFrame != _rightEquippedItem) {
		_rightHeldItem->changeItem(_rightEquippedItem, mousePos, _world);
		_rightEquippedLastFrame = _rightEquippedItem;
	}

	return _rightEquippedItem;
}
inventoryItem* npc::refreshLeftEquippedItem(inventoryItem* rightEquippedItem, glm::vec2 mousePos) {
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

inventoryItem* npc::getRightEquippedItem() {
	return _rightEquippedItem;
}
inventoryItem* npc::getLeftEquippedItem() {
	return _leftEquippedItem;
}

void npc::addDamageLabel(glm::vec2 position, int damage, gengine::color labelColor) {
	_world->addDamageLabel(position, damage, labelColor);
}

void npc::draw(gengine::spriteBatch* spriteBatch) {
	_skeleton.draw(spriteBatch);

	inventoryItem* rightItemToUse = getRightEquippedItem();
	inventoryItem* leftItemToUse = getLeftEquippedItem();

	if (rightItemToUse != nullptr) {
		rightItemToUse->getItemDefinition()->draw(spriteBatch, _rightHeldItem, aimedPoint, _world);

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
		leftItemToUse->getItemDefinition()->draw(spriteBatch, _leftHeldItem, aimedPoint, _world);

		if (_facingRight) {
			_leftHeldItem->draw(spriteBatch, _skeleton.getDrawLayer() - 1);
		}
		else {
			_leftHeldItem->draw(spriteBatch, _skeleton.getDrawLayer() + 1);
		}
	}

}

void npc::debugDraw(gengine::debugRenderer* debugRenderer) {
	_body->debugDraw(debugRenderer);
}