#include "usableItem.h"
#include "luaHandler.h"
#include "timer.h"

usableItem::usableItem() {
	_definitionType = definitionType::USABLE_ITEM;
}

void usableItem::init(std::string texture, std::string icon, glm::ivec2 spritesheetSize, glm::vec2 itemSize, bool twoHanded) {
	_texture.init(gengine::ressourceManager::getTexture(texture), spritesheetSize);
	_icon = gengine::ressourceManager::getTexture(icon);
	_usable = false;
	_itemSize = itemSize;
	_useDelay = 0;
	_twoHanded = twoHanded;
	_offset = luaHandler::getTableRange(_nameID, "offset");
	if (_offset.x == -1 && _offset.y == -1) {
		_offset = glm::vec2(0, 0);
	}
	_drawScale = luaHandler::getTableRange(_nameID, "drawScale");
	if (_drawScale.x == -1 && _drawScale.y == -1) {
		_drawScale = glm::vec2(1, 1);
	}
	_angleOffset = luaHandler::getTableFloat(_nameID, "angleOffset") * (M_PI *2) / 360;
	_armAngleOffset = luaHandler::getTableFloat(_nameID, "armAngleOffset");
	_offset.x = globals::pixToMet(_offset.x);
	_offset.y = globals::pixToMet(_offset.y);
}
void usableItem::equipped(heldItem* heldItem, glm::vec2 mousePos, world* toChange) {
	LUA_whileEquipped(heldItem, mousePos);
}
void usableItem::onEquip(heldItem* heldItem, glm::vec2 mousePos, world* toChange) {
	LUA_onEquip(heldItem, mousePos);
}

int luaHandler::LUA_USABLE_newUsable(lua_State *S) {
	std::string usableName = lua_tostring(S, 1); //Get name of the table

	if (!_verifyLuaName(usableName)) {
		return 0;
	}

	lua_getglobal(S, usableName.c_str());
	lua_pushstring(S, "setProperties");
	lua_gettable(S, -2);
	lua_pushvalue(S, -2);
	lua_call(S, 1, 0);
	lua_pop(S, 1);

	usableItem* newUsable = new usableItem;
	newUsable->_nameID = usableName;
	newUsable->init(luaHandler::getPath(getTableString(usableName, "texture")), luaHandler::getPath(getTableString(usableName, "icon")), getTableRange(usableName, "spritesheetSize"), getTableRange(usableName, "itemSize"), getTableBool(usableName, "twoHanded"));
	_definitions.emplace(std::make_pair(usableName, newUsable));

	//Add animations to texture
	lua_getglobal(S, usableName.c_str());		// tableName
	lua_pushstring(S, "animations");			// "animations", tableName
	lua_gettable(S, -2);						// table animations, tableName
	if (lua_istable(S, -1)) {
		lua_pushnil(S);							// nil, table animations, tableName
		while (lua_next(S, -2) != 0) {			// table, key, table animations, tableName
			lua_pushnumber(S, 1);				// 1, table, key, table animations, tableName
			lua_gettable(S, -2);				// string, table, key, table animations, tableName
			std::string animName = lua_tostring(_S, -1);

			lua_pushnumber(S, 2);
			lua_gettable(S, -3);				// int, string, table, key, table animations, tableName
			int beginIndex = lua_tointeger(_S, -1);

			lua_pushnumber(S, 3);
			lua_gettable(S, -4);				// int, int, string, table, key, table animations, tableName
			int endIndex = lua_tointeger(_S, -1);

			lua_pushnumber(S, 4);
			lua_gettable(S, -5);				// int, int, int, string, table, key, table animations, tableName
			int rate = lua_tointeger(_S, -1);

			newUsable->addAnimation(animName, beginIndex, endIndex, rate);
			lua_pop(S, 5);						// key, table animations, tableName
		}
		lua_pop(S, 3);
	}

	return 0;
}

void usableItem::LUA_whileEquipped(heldItem* heldItem, glm::vec2 mousePos) {
	lua_State* S = luaHandler::getLuaState();
	lua_getglobal(S, _nameID.c_str());
	lua_pushstring(S, "whileEquipped");
	lua_gettable(S, -2);
	lua_pushvalue(S, -2);
	lua_pushlightuserdata(S, heldItem);
	luaL_getmetatable(S, "usable_m");
	lua_setmetatable(S, -2);

	lua_newtable(S);
	lua_pushnumber(S, 1);
	lua_pushnumber(S, globals::metToPix(mousePos.x));
	lua_settable(S, -3);
	lua_pushnumber(S, 2);
	lua_pushnumber(S, globals::metToPix(mousePos.y));
	lua_settable(S, -3);

	lua_pcall(S, 3, 0, 0);
	lua_pop(S, 1);
}
void usableItem::LUA_onEquip(heldItem* heldItem, glm::vec2 mousePos) {
	lua_State* S = luaHandler::getLuaState();
	lua_getglobal(S, _nameID.c_str());
	lua_pushstring(S, "onEquip");
	lua_gettable(S, -2);
	lua_pushvalue(S, -2);
	lua_pushlightuserdata(S, heldItem);
	luaL_getmetatable(S, "usable_m");
	lua_setmetatable(S, -2);

	lua_newtable(S);
	lua_pushnumber(S, 1);
	lua_pushnumber(S, globals::metToPix(mousePos.x));
	lua_settable(S, -3);
	lua_pushnumber(S, 2);
	lua_pushnumber(S, globals::metToPix(mousePos.y));
	lua_settable(S, -3);

	lua_pcall(S, 3, 0, 0);
	lua_pop(S, 1);
}


int luaHandler::LUA_USABLE_setAnimation(lua_State *S) {
	heldItem* p = (heldItem*)lua_touserdata(S, 1);

	p->setAnimation(lua_tostring(S, 2));
	return 0;
}

int luaHandler::LUA_USABLE_createProjectile(lua_State *S) {
	heldItem* p = (heldItem*)lua_touserdata(S, 1);
	std::string nameID = lua_tostring(S, 2);

	glm::vec2 position = glm::vec2(globals::pixToMet(lua_tonumber(S, 3)), globals::pixToMet(lua_tonumber(S, 4)));

	glm::vec2 xVec;
	glm::vec2 yVec;


	if (p->_rightDir) {
		xVec.x = cos(p->_angle) * position.x;
		xVec.y = sin(p->_angle) * position.x;

		yVec.x = cos(p->_angle + (M_PI / 2)) * position.y;
		yVec.y = sin(p->_angle + (M_PI / 2)) * position.y;
	}
	else {
		xVec.x = cos(p->_angle) * position.x;
		xVec.y = sin(p->_angle) * position.x;

		yVec.x = cos(p->_angle - (M_PI / 2)) * position.y;
		yVec.y = sin(p->_angle - (M_PI / 2)) * position.y;
	}

	position.x = xVec.x + yVec.x + p->getPosition().x;
	position.y = xVec.y + yVec.y + p->getPosition().y;

	projectile* newProjectile = new projectile;
	newProjectile->create(nameID, glm::vec2(position.x, position.y), glm::vec2(globals::pixToMet(lua_tonumber(S, 5)), globals::pixToMet(lua_tonumber(S, 6))), p->_holder);
	lua_pushlightuserdata(S, 0);
	luaL_getmetatable(S, "usable_m");
	lua_setmetatable(S, -2);

	return 0;
}


int luaHandler::LUA_USABLE_addInt(lua_State *S) {
	heldItem* p = (heldItem*)lua_touserdata(S, 1);

	if (lua_isinteger(S, 3)) {
		auto iterator = p->LUA_cppvars.find(lua_tostring(S, 2));

		if (iterator == p->LUA_cppvars.end()) {
			void* newInt = malloc(sizeof(lua_tointeger(S, 3)));
			*(int*)newInt = lua_tointeger(S, 3);

			p->LUA_cppvars.insert(std::make_pair(lua_tostring(S, 2), std::make_pair((void*)newInt, luaVariableType::INTL)));
		}
	}

	return 0;
}
int luaHandler::LUA_USABLE_addFloat(lua_State *S) {
	heldItem* p = (heldItem*)lua_touserdata(S, 1);

	if (lua_isnumber(S, 3)) {
		auto iterator = p->LUA_cppvars.find(lua_tostring(S, 2));

		if (iterator == p->LUA_cppvars.end()) {
			void* newFloat = malloc(sizeof(lua_tonumber(S, 3)));
			*(float*)newFloat = lua_tonumber(S, 3);

			p->LUA_cppvars.insert(std::make_pair(lua_tostring(S, 2), std::make_pair((void*)newFloat, luaVariableType::FLOATL)));
		}
	}

	return 0;
}
int luaHandler::LUA_USABLE_addBool(lua_State *S) {
	heldItem* p = (heldItem*)lua_touserdata(S, 1);

	auto iterator = p->LUA_cppvars.find(lua_tostring(S, 2));

	if (lua_isboolean(S, 3)) {
		if (iterator == p->LUA_cppvars.end()) {
			void* newBool = malloc(sizeof(lua_toboolean(S, 3)));
			*(bool*)newBool = lua_toboolean(S, 3);

			p->LUA_cppvars.insert(std::make_pair(lua_tostring(S, 2), std::make_pair((void*)newBool, luaVariableType::BOOLL)));
		}
	}

	return 0;
}

int luaHandler::LUA_USABLE_addTimer(lua_State *S) {
	heldItem* p = (heldItem*)lua_touserdata(S, 1);

	auto iterator = p->LUA_cppvars.find(lua_tostring(S, 2));

	if (iterator == p->LUA_cppvars.end()) {
		void* newTimerPtr = new timer;

		p->LUA_cppvars.insert(std::make_pair(lua_tostring(S, 2), std::make_pair((void*)newTimerPtr, luaVariableType::TIMERL)));
	}

	return 0;
}

int luaHandler::LUA_USABLE_setValue(lua_State *S) {
	heldItem* p = (heldItem*)lua_touserdata(S, 1);
	std::string name = lua_tostring(S, 2);

	auto iterator = p->LUA_cppvars.find(name);

	//Checks if the value exists
	if (iterator != p->LUA_cppvars.end()) {
		//Checks the type of the value, and if the value pasted as the 2nd parameter fits the type of the value
		if (p->LUA_cppvars[name].second == luaVariableType::INTL) {
			if (lua_isinteger(S, 3)) {
				*((int*)(p->LUA_cppvars[name].first)) = lua_tointeger(S, 3);
			}
		}
		else if (p->LUA_cppvars[name].second == luaVariableType::FLOATL) {
			if (lua_isnumber(S, 3)) {
				*((float*)(p->LUA_cppvars[name].first)) = lua_tonumber(S, 3);
			}
		}
		else if (p->LUA_cppvars[name].second == luaVariableType::BOOLL) {
			if (lua_isboolean(S, 3)) {
				*((bool*)(p->LUA_cppvars[name].first)) = lua_toboolean(S, 3);
			}
		}
	}

	return 0;
}

int luaHandler::LUA_USABLE_getValue(lua_State *S) {
	heldItem* p = (heldItem*)lua_touserdata(S, 1);
	std::string name = lua_tostring(S, 2);
	if (p->LUA_cppvars[name].second == luaVariableType::INTL) {
		lua_pushinteger(S, *(int*)(p->LUA_cppvars[lua_tostring(S, 2)].first));
		return 1;
	}
	else if (p->LUA_cppvars[name].second == luaVariableType::FLOATL) {
		lua_pushnumber(S, *(float*)(p->LUA_cppvars[lua_tostring(S, 2)].first));
		return 1;
	}
	else if (p->LUA_cppvars[name].second == luaVariableType::BOOLL) {
		lua_pushboolean(S, *(bool*)(p->LUA_cppvars[lua_tostring(S, 2)].first));
		return 1;
	}
	else if (p->LUA_cppvars[name].second == luaVariableType::TIMERL) {
		timer* pe = (timer*)(p->LUA_cppvars[lua_tostring(S, 2)].first);
		timer** addedLightUserdata = (timer**)lua_newuserdata(S, sizeof(timer*));
		*addedLightUserdata = pe;
		luaL_getmetatable(S, "timer_m");
		lua_setmetatable(S, -2);
		return 1;
	}

}

int luaHandler::LUA_USABLE_consume(lua_State *S) {
	heldItem* p = (heldItem*)lua_touserdata(S, 1);

	p->_item->consume();

	return 0;
}
int luaHandler::LUA_USABLE_addLight(lua_State *S) {
	heldItem* p = (heldItem*)lua_touserdata(S, 1);

	if (p->_lightID == -1) {
		if (p->_initialized == false) {
			light newLight;
			p->_lightID = luaHandler::getWorld()->addLight(p->getPositionPointer(), newLight);
		}
	}

	return 0;
}
int luaHandler::LUA_USABLE_getLight(lua_State *S) {
	heldItem* p = (heldItem*)lua_touserdata(S, 1);

	if (p->_lightID != -1) {
		light** addedLightUserdata = (light**)lua_newuserdata(S, sizeof(light*));
		*addedLightUserdata = getWorld()->getLight(p->_lightID);
		luaL_getmetatable(S, "light_m");
		lua_setmetatable(S, -2);
	}

	return 1;
}
int luaHandler::LUA_USABLE_setEmitter(lua_State *S) {
	heldItem* p = (heldItem*)lua_touserdata(S, 1);
	particleEmitter* newEmitter = new particleEmitter(lua_tostring(S, 2));
	newEmitter->emitterPosition = p->getPositionPointer();
	newEmitter->emitterAngle = p->getAnglePointer();
	newEmitter->dimensions = glm::vec2(globals::pixToMet(p->getItem()->getItemDefinition()->getItemSize().x), globals::pixToMet(p->getItem()->getItemDefinition()->getItemSize().y));
	p->_particleEmitterID = getWorld()->addParticleEmitter(newEmitter);
	return 0;
}

int luaHandler::LUA_USABLE_setOffset(lua_State *S) {
	heldItem* p = (heldItem*)lua_touserdata(S, 1);
	if (lua_isnoneornil(S, 2) || lua_isnoneornil(S, 3)) {
		p->setOffset(0,0);
	}
	else {
		p->setOffset(lua_tointeger(S, 2), lua_tointeger(S, 3));
	}

	return 0;
}

int luaHandler::LUA_USABLE_setAngleOffset(lua_State *S) {
	heldItem* p = (heldItem*)lua_touserdata(S, 1);
	if (lua_isnoneornil(S, 2)) {
		p->setAngleOffset(0);
	}
	else {
		p->setAngleOffset(lua_tonumber(S, 2));
	}

	return 0;
}

int luaHandler::LUA_USABLE_getEmitter(lua_State *S) {
	heldItem* p = (heldItem*)lua_touserdata(S, 1);
	particleEmitter** addedEmitterUserdata = (particleEmitter**)lua_newuserdata(S, sizeof(particleEmitter*));
	*addedEmitterUserdata = getWorld()->getEmitter(p->_particleEmitterID);
	luaL_getmetatable(S, "particleEmitter_m");
	lua_setmetatable(S, -2);
	return 1;
}
int luaHandler::LUA_USABLE_getPosition(lua_State *S) {
	heldItem* p = (heldItem*)lua_touserdata(S, 1);

	lua_newtable(S);
	lua_pushnumber(S, 1);
	lua_pushnumber(S, globals::metToPix(p->getPosition().x));
	lua_settable(S, -3);
	lua_pushnumber(S, 2);
	lua_pushnumber(S, globals::metToPix(p->getPosition().y));
	lua_settable(S, -3);

	return 1;
}

int luaHandler::LUA_USABLE_calculateOffset(lua_State *S) {
	heldItem* p = (heldItem*)lua_touserdata(S, 1);

	glm::vec2 offset = glm::vec2(lua_tonumber(S, 2), lua_tonumber(S, 3));

	glm::vec2 xVec;
	glm::vec2 yVec;


	if (p->_rightDir) {
		xVec.x = cos(p->_angle) * offset.x;
		xVec.y = sin(p->_angle) * offset.x;

		yVec.x = cos(p->_angle + (M_PI / 2)) * offset.y;
		yVec.y = sin(p->_angle + (M_PI / 2)) * offset.y;
	}
	else {
		xVec.x = cos(p->_angle) * offset.x;
		xVec.y = sin(p->_angle) * offset.x;

		yVec.x = cos(p->_angle - (M_PI / 2)) * offset.y;
		yVec.y = sin(p->_angle - (M_PI / 2)) * offset.y;
	}

	offset.x = xVec.x + yVec.x;
	offset.y = xVec.y + yVec.y;

	lua_newtable(S);
	lua_pushnumber(S, 1);
	lua_pushnumber(S, offset.x);
	lua_settable(S, -3);
	lua_pushnumber(S, 2);
	lua_pushnumber(S, offset.y);
	lua_settable(S, -3);

	return 1;
}

int luaHandler::LUA_USABLE_getAngleDeg(lua_State *S) {
	heldItem* p = (heldItem*)lua_touserdata(S, 1);

	lua_newtable(S);
	lua_pushnumber(S, p->_angle * 57.2958);

	return 1;
}

int luaHandler::LUA_USABLE_getAngleRad(lua_State *S) {
	heldItem* p = (heldItem*)lua_touserdata(S, 1);

	lua_newtable(S);
	lua_pushnumber(S, p->_angle);

	return 1;
}

int luaHandler::LUA_USABLE_getDirection(lua_State *S) {
	heldItem* p = (heldItem*)lua_touserdata(S, 1);

	float angleRad = p->_angle;
	glm::vec2 dir;
	dir.x = cos(angleRad);
	dir.y = sin(angleRad);

	lua_newtable(S);
	lua_pushnumber(S, 1);
	lua_pushnumber(S, dir.x);
	lua_settable(S, -3);
	lua_pushnumber(S, 2);
	lua_pushnumber(S, dir.y);
	lua_settable(S, -3);

	return 1;
}

int luaHandler::LUA_USABLE_getHolder(lua_State *S) {
	heldItem* p = (heldItem*)lua_touserdata(S, 1);

	agent** addedAgentUserdata = (agent**)lua_newuserdata(S, sizeof(agent*));
	*addedAgentUserdata = p->_holder;
	luaL_getmetatable(S, "agent_m");
	lua_setmetatable(S, -2);

	return 1;
}

int luaHandler::LUA_USABLE_getUseButton(lua_State *S) {
	heldItem* p = (heldItem*)lua_touserdata(S, 1);

	if (p->_rightHand == true) {
		lua_pushstring(S, "MOUSE_LEFT");
	}
	else {
		lua_pushstring(S, "MOUSE_RIGHT");
	}

	return 1;
}

int luaHandler::LUA_USABLE_getArmAnimationName(lua_State *S) {
	heldItem* p = (heldItem*)lua_touserdata(S, 1);

	if (p->isHeldInRightHand()) {
		std::string animName = p->_holder->getSkeleton()->getAnimationNames(1).first;
		if (animName == "holding front" || animName == "holding back" || animName == "holding two-handed") {
			lua_pushstring(S, "holding");
		}
		else if (animName == "swing front" || animName == "swing back" || animName == "swing two-handed") {
			lua_pushstring(S, "swinging");
		}
		else if (animName == "stab front" || animName == "stab back" || animName == "stab two-handed") {
			lua_pushstring(S, "stabbing");
		}
		else if (animName == "spin front" || animName == "spin back" || animName == "spin two-handed") {
			lua_pushstring(S, "spinning");
		}
		else if (animName == "use front" || animName == "use back" || animName == "use two-handed") {
			lua_pushstring(S, "use");
		}
		else if (animName == "slash up front" || animName == "slash up back" || animName == "slash up two-handed") {
			lua_pushstring(S, "slashing up");
		}
		else if (animName == "shoot front" || animName == "shoot back" || animName == "shoot two-handed") {
			lua_pushstring(S, "shooting");
		}
	}
	else {
		std::string animName = p->_holder->getSkeleton()->getAnimationNames(2).first;
		if (animName == "holding front" || animName == "holding back" || animName == "holding two-handed") {
			lua_pushstring(S, "holding");
		}
		else if (animName == "swing front" || animName == "swing back" || animName == "swing two-handed") {
			lua_pushstring(S, "swinging");
		}
		else if (animName == "stab front" || animName == "stab back" || animName == "stab two-handed") {
			lua_pushstring(S, "stabbing");
		}
		else if (animName == "spin front" || animName == "spin back" || animName == "spin two-handed") {
			lua_pushstring(S, "spinning");
		}
		else if (animName == "use front" || animName == "use back" || animName == "use two-handed") {
			lua_pushstring(S, "use");
		}
		else if (animName == "slash up front" || animName == "slash up back" || animName == "slash up two-handed") {
			lua_pushstring(S, "slashing up");
		}
		else if (animName == "shoot front" || animName == "shoot back" || animName == "shoot two-handed") {
			lua_pushstring(S, "shooting");
		}
	}

	return 1;
}

int luaHandler::LUA_USABLE_getEventState(lua_State *S) {
	heldItem* p = (heldItem*)lua_touserdata(S, 1);

	std::string eventName = lua_tostring(S, 2);

	if (eventName == "swinging") {
		lua_pushboolean(S, p->_isBeingSwung);
	}
	else {
		lua_pushboolean(S, p->_holder->_skeleton.getEvent(eventName));
	}

	return 1;
}

int luaHandler::LUA_USABLE_playAnimation(lua_State *S) {
	heldItem* p = (heldItem*)lua_touserdata(S, 1);
	
	std::string animationName = lua_tostring(S, 2);
	int animationTime = lua_tointeger(S, 3);

	if (animationName == "mine") {
		if (p->_item->getItemDefinition()->isTwoHanded()) {
			p->_holder->playAnimation("mine two-handed", "mine two-handed", animationTime, 1, false);
		}
		else {
			if (p->isHeldInRightHand()) {
				p->_holder->playAnimation("mine front", "mine back", animationTime, 1, false);
			}
			else {
				p->_holder->playAnimation("mine back", "mine front", animationTime, 2, false);
			}
		}
	}

	if (animationName == "swing") {
		if (p->_item->getItemDefinition()->isTwoHanded()) {
			p->_holder->playAnimation("swing two-handed", "swing two-handed", animationTime, 1, false);
		}
		else {
			if (p->isHeldInRightHand()) {
				p->_holder->playAnimation("swing front", "swing back", animationTime, 1, false);
			}
			else {
				p->_holder->playAnimation("swing back", "swing front", animationTime, 2, false);
			}
		}
	}

	if (animationName == "stab") {
		if (p->_item->getItemDefinition()->isTwoHanded()) {
			p->_holder->playAnimation("stab two-handed", "stab two-handed", animationTime, 1, false);
		}
		else {
			if (p->isHeldInRightHand()) {
				p->_holder->playAnimation("stab front", "stab back", animationTime, 1,false);
			}
			else {
				p->_holder->playAnimation("stab back", "stab front", animationTime, 2,false);
			}
		}
	}

	if (animationName == "slash up") {
		if (p->_item->getItemDefinition()->isTwoHanded()) {
			p->_holder->playAnimation("slash up two-handed", "slash up two-handed", animationTime, 1, false);
		}
		else {
			if (p->isHeldInRightHand()) {
				p->_holder->playAnimation("slash up front", "slash up back", animationTime, 1, false);
			}
			else {
				p->_holder->playAnimation("slash up back", "slash up front", animationTime, 2, false);
			}
		}
	}

	if (animationName == "spin") {
		if (p->_item->getItemDefinition()->isTwoHanded()) {
			p->_holder->playAnimation("spin two-handed", "spin two-handed", animationTime, 1, false);
		}
		else {
			if (p->isHeldInRightHand()) {
				p->_holder->playAnimation("spin front", "spin back", animationTime, 1, false);
			}
			else {
				p->_holder->playAnimation("spin back", "spin front", animationTime, 2, false);
			}
		}
	}

	if (animationName == "shoot") {
		if (p->_item->getItemDefinition()->isTwoHanded()) {
			p->_holder->playAnimation("shoot two-handed", "shoot two-handed", animationTime, 1, false);
		}
		else {
			if (p->isHeldInRightHand()) {
				p->_holder->playAnimation("shoot front", "shoot back", animationTime, 1, false);
			}
			else {
				p->_holder->playAnimation("shoot back", "shoot front", animationTime, 2, false);
			}
		}
	}

	return 0;  
}

void luaHandler::_registerUsableFunction() {
	luaL_Reg usablelib_m[] = {
		{ "addLight", LUA_USABLE_addLight },
		{ "getLight", LUA_USABLE_getLight },
		{ "consume", LUA_USABLE_consume },
		{ "setEmitter", LUA_USABLE_setEmitter },
		{ "setOffset", LUA_USABLE_setOffset },
		{ "setAngleOffset", LUA_USABLE_setAngleOffset },
		{ "calculateOffset", LUA_USABLE_calculateOffset },
		{ "getEmitter", LUA_USABLE_getEmitter },
		{ "getPosition", LUA_USABLE_getPosition },
		{ "getAngleDeg", LUA_USABLE_getAngleDeg },
		{ "getAngleRad", LUA_USABLE_getAngleRad },
		{ "getDirection", LUA_USABLE_getDirection },
		{ "setAnimation", LUA_USABLE_setAnimation },
		{ "createProjectile", LUA_USABLE_createProjectile },
		{ "addInt", LUA_USABLE_addInt },
		{ "addNum", LUA_USABLE_addFloat },
		{ "addBool", LUA_USABLE_addBool },
		{ "addTimer", LUA_USABLE_addTimer },
		{ "setValue", LUA_USABLE_setValue },
		{ "getValue", LUA_USABLE_getValue },
		{ "getHolder", LUA_USABLE_getHolder },
		{ "getUseButton", LUA_USABLE_getUseButton },
		{ "getEventState", LUA_USABLE_getEventState },
		{ "playAnimation", LUA_USABLE_playAnimation },
		{ "getArmAnimationName", LUA_USABLE_getArmAnimationName },
		{ NULL, NULL }
	};

	luaL_newmetatable(_S, "usable_m");

	lua_pushstring(_S, "__index");
	lua_pushvalue(_S, -2);  /* pushes the metatable*/
	lua_settable(_S, -3);  /* metatable.__index = metatable */

	luaL_setfuncs(_S, usablelib_m, 0);
}