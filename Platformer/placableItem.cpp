#include "placableItem.h"
#include "globals.h"
#include "lightSystem.h"
#include "largeTileBody.h"
#include "SDL/SDL.h"
#include "GUI.h"
#include "platform.h"
#include "fatherTile.h"
#include "world.h"

void placableItem::init(std::string texture, std::string icon, int placeConditions, glm::vec2 size, glm::ivec2 spritesheetSize, int healthpoint) {
	_texture.init(gengine::ressourceManager::getTexture(texture), spritesheetSize);
	_icon = gengine::ressourceManager::getTexture(icon);
	_usable = true;
	_placableConditions = placeConditions;
	_size = size;
	_useDelay = 200;
	_itemSize = glm::vec2(globals::TILE_SIZE, globals::TILE_SIZE);
	_twoHanded = true;
	_hp = healthpoint;
	_offset = luaHandler::getTableRange(_nameID, "offset");
	if (_offset.x == -1 && _offset.y == -1) {
		_offset = glm::vec2(0, 0);
	}
	_drawScale = luaHandler::getTableRange(_nameID, "drawScale");
	if (_drawScale.x == -1 && _drawScale.y == -1) {
		_drawScale = glm::vec2(1, 1);
	}
	_angleOffset = luaHandler::getTableFloat(_nameID, "angleOffset") * (M_PI * 2) / 360;
	_armAngleOffset = luaHandler::getTableFloat(_nameID, "armAngleOffset");
	_offset.x = globals::pixToMet(_offset.x);
	_offset.y = globals::pixToMet(_offset.y);

	_definitionType = definitionType::PLACABLE_ITEM;
}

glm::ivec2 placableItem::getSize() {
	return _size;
}

bool placableItem::primaryUse(heldItem* heldItem, glm::vec2 mousePos, world* toChange) {
	glm::vec2 itemPos = heldItem->getPosition();

	glm::vec2 distanceVec(mousePos.x - itemPos.x, mousePos.y - itemPos.y);
	float distance = glm::length(distanceVec);

	if (distance <= globals::MAX_TILE_RANGE) {
		glm::ivec2 size = getSize();
		glm::vec2 sizeInMeter;
		sizeInMeter.x = size.x * globals::pixToMet(globals::TILE_SIZE);
		sizeInMeter.y = size.y * globals::pixToMet(globals::TILE_SIZE);

		int xCenter;
		if (size.x % 2 == 0) {
			xCenter = ceil(mousePos.x / globals::pixToMet(globals::TILE_SIZE));
		}
		else {
			xCenter = (int)(mousePos.x / globals::pixToMet(globals::TILE_SIZE));
		}
		int yCenter;
		if (size.y % 2 == 0) {
			yCenter = ceil(mousePos.y / globals::pixToMet(globals::TILE_SIZE));
		}
		else {
			yCenter = (int)(mousePos.y / globals::pixToMet(globals::TILE_SIZE));
		}

		int itemGridX = (int)(xCenter - (std::floor(size.x / 2)));
		int itemGridY = (int)(yCenter - (std::floor(size.y / 2)));

	
		if (canBePlaced(toChange, itemGridX, itemGridY) && _hasEveryTileFree(toChange,itemGridX,itemGridY)) {
			if (toChange->addTileIngame(this,true,itemGridX, itemGridY)) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}

	}
	else {
		return false;
	}
}
bool placableItem::canBePlaced(world* world, int fatherTileX, int fatherTileY) {
	glm::vec2 size = getSize();

	if (_placableConditions & placableFlags::floors) {
		if (_canBePlacedOnFloor(world, fatherTileX, fatherTileY)) {
			return true;
		}
	}
	if (_placableConditions & placableFlags::background) {
		if (_canBePlacedOnBackground(world, fatherTileX,fatherTileY)) {
			return true;
		}
	}
	if (_placableConditions & placableFlags::ceiling) {
		if (_canBePlacedOnCeiling(world, fatherTileX, fatherTileY)) {
			return true;
		}
	}
	if (_placableConditions & placableFlags::wall) {
		if (_canBePlacedOnWall(world, fatherTileX, fatherTileY)) {
			return true;
		}
	}
	return false;
}

void placableItem::equipped(heldItem* heldItem, glm::vec2 mousePos, world* toChange) {
	LUA_whileEquipped(heldItem, mousePos);
}
void placableItem::onEquip(heldItem* heldItem, glm::vec2 mousePos, world* toChange) {
	LUA_onEquip(heldItem, mousePos);
}

void placableItem::draw(gengine::spriteBatch* spriteBatch, heldItem* heldItem, glm::vec2 mousePos, world* toChange) {
	glm::vec2 itemPos = heldItem->getPosition();

	glm::ivec2 size = getSize();
	glm::vec2 sizeInMeter;
	sizeInMeter.x = size.x * globals::pixToMet(globals::TILE_SIZE);
	sizeInMeter.y = size.y * globals::pixToMet(globals::TILE_SIZE);

	int xCenter;
	if (size.x % 2 == 0) {
		xCenter = ceil(mousePos.x / globals::pixToMet(globals::TILE_SIZE));
	}
	else {
		xCenter = (int)(mousePos.x / globals::pixToMet(globals::TILE_SIZE));
	}
	int yCenter;
	if (size.y % 2 == 0) {
		yCenter = ceil(mousePos.y / globals::pixToMet(globals::TILE_SIZE));
	}
	else {
		yCenter = (int)(mousePos.y / globals::pixToMet(globals::TILE_SIZE));
	}

	int itemGridX = (int)(xCenter - (std::floor(size.x / 2)));
	int itemGridY = (int)(yCenter - (std::floor(size.y / 2)));

	glm::vec2 distanceVec(mousePos.x - itemPos.x, mousePos.y - itemPos.y);
	float distance = glm::length(distanceVec);

	bool placable = true;

	if (distance <= globals::MAX_TILE_RANGE) {
		glm::vec2 sizeInMeter;
		sizeInMeter.x = size.x * globals::pixToMet(globals::TILE_SIZE);
		sizeInMeter.y = size.y * globals::pixToMet(globals::TILE_SIZE);

		int mouseGridX = (int)(mousePos.x / globals::pixToMet(globals::TILE_SIZE));
		int mouseGridY = (int)(mousePos.y / globals::pixToMet(globals::TILE_SIZE));

		for (int i = 0; i < (size.x * size.y); i++) {
			int checkedGridX = (int)((i % size.x) + itemGridX);
			int checkedGridY = (int)((i / size.x) + itemGridY);

			if (toChange->getTile(checkedGridX, checkedGridY) != nullptr) {
				if (toChange->getTile(checkedGridX, checkedGridY)->hasFront()) {
					placable = false;
				}
			}
		}

		if (!canBePlaced(toChange, itemGridX, itemGridY)) {
			placable = false;
		}

	}
	else {
		placable = false;
	}

	_drawItem(toChange, itemGridX, itemGridY, spriteBatch, placable);
}

void placableItem::_drawItem(world* world, int gridX, int gridY, gengine::spriteBatch* spriteBatch, bool canBePlaced) {
	glm::ivec2 size = getSize();
	glm::vec4 destRect;
	destRect.x = gridX * globals::pixToMet(globals::TILE_SIZE);
	destRect.y = gridY * globals::pixToMet(globals::TILE_SIZE);
	destRect.z = globals::pixToMet(globals::TILE_SIZE) * size.x;
	destRect.w = globals::pixToMet(globals::TILE_SIZE) * size.y;

	if (canBePlaced) {
		spriteBatch->draw(destRect, _texture.getAnimUV(), _texture.getTexture()->id, 50, gengine::color(255, 255, 255, 175));

		destRect.x -= 0.075f;
		destRect.y -= 0.075f;
		destRect.z += 0.15f;
		destRect.w += 0.15f;

		spriteBatch->draw(destRect, _texture.getAnimUV(), _texture.getTexture()->id, 49, gengine::color(50, 200, 255, 255));
	}
	else {
		spriteBatch->draw(destRect, _texture.getAnimUV(), _texture.getTexture()->id, 50, gengine::color(255, 255, 255, 175));

		destRect.x -= 0.075f;
		destRect.y -= 0.075f;
		destRect.z += 0.15f;
		destRect.w += 0.15f;

		spriteBatch->draw(destRect, _texture.getAnimUV(), _texture.getTexture()->id, 49, gengine::color(255, 0, 0, 255));
	}
	
}

bool placableItem::_canBePlacedOnFloor(world* world, int fatherTileX, int fatherTileY)
{
	glm::ivec2 size = getSize();

	for (int x = fatherTileX; x < fatherTileX + size.x; x++) {
		tile* inspectedTile = world->getTile(x, fatherTileY - 1);
		if (inspectedTile == nullptr) {
			return false;
		}
		else {
			if (inspectedTile->isFloor() == false) {
				return false;
			}
		}
	}

	return true;
}

bool placableItem::_canBePlacedOnCeiling(world* world, int fatherTileX, int fatherTileY)
{
	glm::ivec2 size = getSize();

	for (int x = fatherTileX; x < fatherTileX + size.x; x++) {
		tile* inspectedTile = world->getTile(x, fatherTileY + size.y);
		if (inspectedTile == nullptr) {
			return false;
		}
		else {
			if (inspectedTile->isFloor() == false) {
				return false;
			}
		}
	}

	return true;
}

bool placableItem::_canBePlacedOnBackground(world* world, int fatherTileX, int fatherTileY)
{
	glm::ivec2 size = getSize();

	for (int y = fatherTileY; y < fatherTileY + size.y; y++) {
		for (int x = fatherTileX; x < fatherTileX + size.x; x++) {
			tile* inspectedTile = world->getTile(x, y);
			if (inspectedTile == nullptr) {
				return false;
			}
			else {
				if (!inspectedTile->hasBack()) {
					return false;
				}
			}
		}
	}

	return true;
}

bool placableItem::_canBePlacedOnWall(world* world, int fatherTileX, int fatherTileY)
{
	glm::ivec2 size = getSize();

	for (int y = fatherTileY; y < fatherTileY + size.y; y++) {
		tile* inspectedTile = world->getTile(fatherTileX - 1, y);
		if (inspectedTile == nullptr) {
			return false;
		}
		else {
			if (!inspectedTile->hasFront()) {
				return false;
			}
		}
	}

	return true;
}

bool placableItem::_hasEveryTileFree(world* world, int fatherTileX, int fatherTileY)
{
	glm::ivec2 size = getSize();

	for (int y = fatherTileY; y < fatherTileY + size.y; y++) {
		for (int x = fatherTileX; x < fatherTileX + size.x; x++) {
			tile* inspectedTile = world->getTile(x, y);
			if (inspectedTile != nullptr) {
				if (inspectedTile->hasFront()) {
					return false;
				}
			}
		}
	}
	return true;
}

void placableItem::LUA_onPlaced(fatherTile* newTile) {
	lua_State* S = luaHandler::getLuaState();
	lua_getglobal(S, _nameID.c_str());
	lua_pushstring(S, "onPlaced");
	lua_gettable(S, -2);
	lua_pushvalue(S, -2);
	lua_pushlightuserdata(S, newTile);
	luaL_getmetatable(S, "placable_m");
	lua_setmetatable(S, -2);
	lua_pcall(S, 2, 0, 0);
	lua_pop(S, 1);
}
void placableItem::LUA_update(fatherTile* newTile) {
	lua_State* S = luaHandler::getLuaState();
	lua_getglobal(S, _nameID.c_str());
	lua_pushstring(S, "update");
	lua_gettable(S, -2);
	lua_pushvalue(S, -2);
	lua_pushlightuserdata(S, newTile);
	luaL_getmetatable(S, "placable_m");
	lua_setmetatable(S, -2);
	lua_pcall(S, 2, 0, 0);
	lua_pop(S, 1);
}
void placableItem::LUA_use(fatherTile* newTile, agent* user) {
	lua_State* S = luaHandler::getLuaState();
	lua_getglobal(S, _nameID.c_str());
	lua_pushstring(S, "use");
	lua_gettable(S, -2);
	lua_pushvalue(S, -2);
	lua_pushlightuserdata(S, newTile);
	luaL_getmetatable(S, "placable_m");
	lua_setmetatable(S, -2);

	agent** addedAgentUserdata = (agent**)lua_newuserdata(S, sizeof(agent*));
	*addedAgentUserdata = static_cast<agent*>(user);
	luaL_getmetatable(S, "agent_m");
	lua_setmetatable(S, -2);

	lua_pcall(S, 3, 0, 0);
	lua_pop(S, 1);
}
void placableItem::LUA_whileEquipped(heldItem* heldItem, glm::vec2 mousePos) {
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
	lua_pushnumber(S, mousePos.x);
	lua_settable(S, -3);
	lua_pushnumber(S, 2);
	lua_pushnumber(S, mousePos.y);
	lua_settable(S, -3);

	lua_pcall(S, 3, 0, 0);
	lua_pop(S, 1);
}
void placableItem::LUA_onEquip(heldItem* heldItem, glm::vec2 mousePos) {
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
	lua_pushnumber(S, mousePos.x);
	lua_settable(S, -3);
	lua_pushnumber(S, 2);
	lua_pushnumber(S, mousePos.y);
	lua_settable(S, -3);

	lua_pcall(S, 3, 0, 0);
	lua_pop(S, 1);
}
void placableItem::LUA_onUnequip(heldItem* heldItem, glm::vec2 mousePos) {
	lua_State* S = luaHandler::getLuaState();
	lua_getglobal(S, _nameID.c_str());
	lua_pushstring(S, "onUnequip");
	lua_gettable(S, -2);
	lua_pushvalue(S, -2);
	lua_pushlightuserdata(S, heldItem);
	luaL_getmetatable(S, "usable_m");
	lua_setmetatable(S, -2);

	lua_newtable(S);
	lua_pushnumber(S, 1);
	lua_pushnumber(S, mousePos.x);
	lua_settable(S, -3);
	lua_pushnumber(S, 2);
	lua_pushnumber(S, mousePos.y);
	lua_settable(S, -3);

	lua_pcall(S, 3, 0, 0);
	lua_pop(S, 1);
}


int luaHandler::LUA_PLACABLE_newPlacableObject(lua_State *S) {
	std::string placableName = lua_tostring(S, 1); //Get name of the table

	if (!_verifyLuaName(placableName)) {
		return 0;
	}

	lua_getglobal(S, placableName.c_str());
	lua_pushstring(S, "setProperties");		
	lua_gettable(S, -2);					
	lua_pushvalue(S, -2);					
	lua_call(S, 1, 0);
	lua_pop(S, 1);

	std::vector<std::string> placableConditions = getTableTableStrings(placableName, "placedOn");
	int placableConditionFlags = 0;
	for (int i = 0; i < placableConditions.size(); i++) {
		if (placableConditions[i] == "floor") {
			placableConditionFlags |= placableFlags::floors;
		}
		else if (placableConditions[i] == "background") {
			placableConditionFlags |= placableFlags::background;
		}
		else if (placableConditions[i] == "ceiling") {
			placableConditionFlags |= placableFlags::ceiling;
		}
		else if (placableConditions[i] == "wall") {
			placableConditionFlags |= placableFlags::wall;
		}
	}

	glm::vec2 size = luaHandler::getTableRange(placableName, "tileSize");
	glm::vec2 spritesheetSize = luaHandler::getTableRange(placableName, "spritesheetSize");
	//Doesn't exist
	if (spritesheetSize.x == -1 && spritesheetSize.y == -1) {
		spritesheetSize.x = 1;
		spritesheetSize.y = 1;
	}

	placableItem* newPlacable = new placableItem();
	newPlacable->_nameID = placableName;
	newPlacable->init(getPath(getTableString(placableName, "texture")), getPath(getTableString(placableName, "icon")), placableConditionFlags,size, spritesheetSize, getTableInt(placableName, "hitpoints"));
	_definitions.emplace(std::make_pair(placableName, newPlacable));
	
	//Add animations to texture
	lua_getglobal(S, placableName.c_str());		// tableName
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

			newPlacable->addAnimation(animName, beginIndex, endIndex, rate);
			lua_pop(S, 5);						// key, table animations, tableName
		}
		lua_pop(S, 3);
	}

	return 0;
}

int luaHandler::LUA_PLACABLE_getCenterPosition(lua_State *S) {
	fatherTile* p = (fatherTile*)lua_touserdata(S, 1);
	
	lua_newtable(S);
	lua_pushnumber(S, 1);
	lua_pushnumber(S, globals::metToPix((p->getTilePosition().x + (p->getFrontItem()->getSize().x / 2)) * globals::pixToMet(globals::TILE_SIZE)));
	lua_settable(S, -3);
	lua_pushnumber(S, 2);
	lua_pushnumber(S, globals::metToPix((p->getTilePosition().y + (p->getFrontItem()->getSize().y / 2)) * globals::pixToMet(globals::TILE_SIZE)));
	lua_settable(S, -3);

	return 1;
}

int luaHandler::LUA_PLACABLE_addLight(lua_State *S) {
	fatherTile* p = (fatherTile*) lua_touserdata(S, 1);

	if (p->_lightID == -1) {
		if (p->wasInitialized() == false) {
			light newLight;
			p->_lightID = luaHandler::getWorld()->addLight(p->getPositionPointer(), newLight);
		}
	}

	return 0;
}
int luaHandler::LUA_PLACABLE_getLight(lua_State *S) {
	fatherTile* p = (fatherTile*)lua_touserdata(S, 1);

	if (p->_lightID != -1) {
		light** addedLightUserdata = (light**)lua_newuserdata(S, sizeof(light*));
		*addedLightUserdata = getWorld()->getLight(p->_lightID);
		luaL_getmetatable(S, "light_m");
		lua_setmetatable(S, -2);
	}

	return 1;
}
int luaHandler::LUA_PLACABLE_setEmitter(lua_State *S) {
	fatherTile* p = (fatherTile*)lua_touserdata(S, 1);
	particleEmitter* newEmitter = new particleEmitter(lua_tostring(S, 2));
	newEmitter->emitterPosition = p->getPositionPointer();
	glm::vec2 tileSize = p->getFrontItem()->getSize();
	newEmitter->dimensions = glm::vec2(tileSize.x * globals::pixToMet(globals::TILE_SIZE), tileSize.y * globals::pixToMet(globals::TILE_SIZE));
	p->setParticleEmitterID(getWorld()->addParticleEmitter(newEmitter));
	return 0;
}
int luaHandler::LUA_PLACABLE_getEmitter(lua_State *S) {
	fatherTile* p = (fatherTile*)lua_touserdata(S, 1);
	particleEmitter** addedEmitterUserdata = (particleEmitter**)lua_newuserdata(S, sizeof(particleEmitter*));
	*addedEmitterUserdata = getWorld()->getEmitter(p->getEmitterID());
	luaL_getmetatable(S, "particleEmitter_m");
	lua_setmetatable(S, -2);
	return 1;
}
int luaHandler::LUA_PLACABLE_initInventory(lua_State *S) {
	fatherTile* p = (fatherTile*)lua_touserdata(S, 1);
	std::string format = lua_tostring(S, 2);
	inventory* itemInventory = p->getInventory();
	if (itemInventory == nullptr) {
		if (format == "small") {
			p->createInventory(9, 3);
		}
		else if (format == "large") {
			p->createInventory(36, 6);
		}
		else if (format == "longSmall") {
			p->createInventory(26, 13);
		}
		else if (format == "longLarge") {
			p->createInventory(52, 13);
		}
		else if (format == "highSmall") {
			p->createInventory(24, 3);
		}
		else if (format == "highLarge") {
			p->createInventory(48, 6);
		}
	}
	return 0;
}
int luaHandler::LUA_PLACABLE_openInventory(lua_State *S) {
	fatherTile* p = (fatherTile*)lua_touserdata(S, 1);
	inventory* inventory = p->getInventory();
	if (inventory != nullptr) {
		GUI::openWorldInventory(inventory);
	}
	return 0;
}
int luaHandler::LUA_PLACABLE_isInventoryClosed(lua_State *S) {
	fatherTile* p = (fatherTile*)lua_touserdata(S, 1);
	inventory* inventory = p->getInventory();
	if (inventory != nullptr) {
		lua_pushboolean(S, inventory->isClosed());
	}
	return 1;
}

int luaHandler::LUA_PLACABLE_setAnimation(lua_State *S) {
	fatherTile* p = (fatherTile*)lua_touserdata(S, 1);
	if (lua_isnoneornil(S, 3)) {
		p->setAnimation(lua_tostring(S, 2), -1);
	}
	else {
		p->setAnimation(lua_tostring(S, 2), lua_tonumber(S, 3));
	}
	return 0;
}

int luaHandler::LUA_PLACABLE_addInt(lua_State *S) {
	fatherTile* p = (fatherTile*)lua_touserdata(S, 1);

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
int luaHandler::LUA_PLACABLE_addFloat(lua_State *S) {
	fatherTile* p = (fatherTile*)lua_touserdata(S, 1);

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
int luaHandler::LUA_PLACABLE_addBool(lua_State *S) {
	fatherTile* p = (fatherTile*)lua_touserdata(S, 1);

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

int luaHandler::LUA_PLACABLE_addTimer(lua_State *S) {
	fatherTile* p = (fatherTile*)lua_touserdata(S, 1);

	auto iterator = p->LUA_cppvars.find(lua_tostring(S, 2));

	if (iterator == p->LUA_cppvars.end()) {
		void* newTimerPtr = new timer;

		p->LUA_cppvars.insert(std::make_pair(lua_tostring(S, 2), std::make_pair((void*)newTimerPtr, luaVariableType::TIMERL)));
	}

	return 0;
}

int luaHandler::LUA_PLACABLE_addBody(lua_State *S) {
	fatherTile* p = (fatherTile*)lua_touserdata(S, 1);

	auto iterator = p->LUA_cppvars.find(lua_tostring(S, 2));

	if (iterator == p->LUA_cppvars.end()) {
		largeTileBody* newTilePtr = new largeTileBody;
		glm::ivec2 positionOffset = glm::ivec2(lua_tointeger(S, 3), lua_tointeger(S, 4));
		glm::ivec2 tileDimensions = glm::ivec2(lua_tointeger(S, 5), lua_tointeger(S, 6));

		glm::vec2 position;
		position.x = (p->getTilePosition().x + positionOffset.x) * globals::pixToMet(globals::TILE_SIZE);
		position.y = (p->getTilePosition().y + positionOffset.y) * globals::pixToMet(globals::TILE_SIZE);

		glm::vec2 dimensions;
		dimensions.x = tileDimensions.x * globals::pixToMet(globals::TILE_SIZE);
		dimensions.y = tileDimensions.y * globals::pixToMet(globals::TILE_SIZE);

		position.x += dimensions.x / 2;
		position.y += dimensions.y / 2;

		newTilePtr->init(_world, position, dimensions);

		p->LUA_cppvars.insert(std::make_pair(lua_tostring(S, 2), std::make_pair((void*)newTilePtr, luaVariableType::BODYL)));
	}

	return 0;
}
int luaHandler::LUA_PLACABLE_addPlatform(lua_State *S) {
	fatherTile* p = (fatherTile*)lua_touserdata(S, 1);

	auto iterator = p->LUA_cppvars.find(lua_tostring(S, 2));

	if (iterator == p->LUA_cppvars.end()) {
		platform* newPlatformPtr = new platform;
		glm::ivec2 positionOffset = glm::ivec2(lua_tointeger(S, 3), lua_tointeger(S, 4));
		int tileWidth = lua_tointeger(S, 5);

		glm::vec2 position;
		position.x = (p->getTilePosition().x + positionOffset.x) * globals::pixToMet(globals::TILE_SIZE);
		position.y = (p->getTilePosition().y + positionOffset.y) * globals::pixToMet(globals::TILE_SIZE);

		float width = tileWidth * globals::pixToMet(globals::TILE_SIZE);

		position.x += width / 2;

		newPlatformPtr->init(_world, position, width);


		p->LUA_cppvars.insert(std::make_pair(lua_tostring(S, 2), std::make_pair((void*)newPlatformPtr, luaVariableType::PLATFORML)));
	}

	return 0;
}

int luaHandler::LUA_PLACABLE_setValue(lua_State *S) {
	fatherTile* p = (fatherTile*)lua_touserdata(S, 1);
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

int luaHandler::LUA_PLACABLE_getValue(lua_State *S) {
	fatherTile* p = (fatherTile*)lua_touserdata(S, 1);
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
	else if (p->LUA_cppvars[name].second == luaVariableType::BODYL) {
		largeTileBody* pe = (largeTileBody*)(p->LUA_cppvars[lua_tostring(S, 2)].first);
		largeTileBody** addedLightUserdata = (largeTileBody**)lua_newuserdata(S, sizeof(tileBatch*));
		*addedLightUserdata = pe;
		luaL_getmetatable(S, "solid_body_m");
		lua_setmetatable(S, -2);
		return 1;
	}
	else if (p->LUA_cppvars[name].second == luaVariableType::PLATFORML) {
		platform* pe = (platform*)(p->LUA_cppvars[lua_tostring(S, 2)].first);
		platform** addedLightUserdata = (platform**)lua_newuserdata(S, sizeof(tileBatch*));
		*addedLightUserdata = pe;
		luaL_getmetatable(S, "platform_m");
		lua_setmetatable(S, -2);
		return 1;
	}

}



void luaHandler::_registerPlacableFunction() {
	luaL_Reg placablelib_m[] = {
		{ "getCenterPosition", LUA_PLACABLE_getCenterPosition },
		{ "addLight", LUA_PLACABLE_addLight },
		{ "getLight", LUA_PLACABLE_getLight },
		{ "setEmitter", LUA_PLACABLE_setEmitter },
		{ "getEmitter", LUA_PLACABLE_getEmitter },
		{ "setAnimation", LUA_PLACABLE_setAnimation },
		{ "initInventory", LUA_PLACABLE_initInventory },
		{ "openInventory", LUA_PLACABLE_openInventory },
		{ "isInventoryClosed", LUA_PLACABLE_isInventoryClosed },
		{ "addInt", LUA_PLACABLE_addInt },
		{ "addNum", LUA_PLACABLE_addFloat },
		{ "addBool", LUA_PLACABLE_addBool },
		{ "addBody", LUA_PLACABLE_addBody },
		{ "addPlatform", LUA_PLACABLE_addPlatform },
		{ "setValue", LUA_PLACABLE_setValue },
		{ "getValue", LUA_PLACABLE_getValue },
		{ NULL, NULL }
	};

	luaL_newmetatable(_S, "placable_m");

	lua_pushstring(_S, "__index");
	lua_pushvalue(_S, -2);  /* pushes the metatable*/
	lua_settable(_S, -3);  /* metatable.__index = metatable */

	luaL_setfuncs(_S, placablelib_m, 0);
}