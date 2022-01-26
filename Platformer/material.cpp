#include "material.h"
#include "world.h"
#include "itemDefinition.h"
#include "globals.h"

gengine::particleBatch2D material::materialParticles;

material::material() {
	_definitionType = definitionType::MATERIAL;
}

void material::init(std::string texture, std::string icon, bool autoTile, int healthPoint, glm::vec2 spritesheetSize, gengine::color particleColor) {
	_icon = gengine::ressourceManager::getTexture(icon);
	_usable = true;
	_useDelay = 35;
	_itemSize = glm::vec2(globals::TILE_SIZE, globals::TILE_SIZE);
	_twoHanded = true;
	_maxHP = healthPoint;
	_autoTiling = autoTile;
	_particleColor = particleColor;
	_drawScale = glm::vec2(1, 1);
	if (_autoTiling) {
		_texture.init(gengine::ressourceManager::getTexture(texture), spritesheetSize);
		_texture.addAnimation("default", 0, 15, 0);
		_texture.setLoops(0);
	}
	else {
		_texture.init(gengine::ressourceManager::getTexture(texture));
	}
}

bool material::primaryUse(heldItem* heldItem, glm::vec2 mousePos, world* toChange) {
	int gridX = (int)(mousePos.x / globals::pixToMet(globals::TILE_SIZE));
	int gridY = (int)(mousePos.y / globals::pixToMet(globals::TILE_SIZE));

	glm::vec2 itemPos = heldItem->getPosition();

	glm::vec2 distanceVec(mousePos.x - itemPos.x, mousePos.y - itemPos.y);
	float distance = glm::length(distanceVec);

	if (distance <= globals::MAX_TILE_RANGE) {
		//Checks if there is an adjacent tile
		if (toChange->getTile(gridX + 1, gridY) != nullptr || toChange->getTile(gridX - 1, gridY) != nullptr ||
			toChange->getTile(gridX, gridY + 1) != nullptr || toChange->getTile(gridX, gridY - 1) != nullptr) {
			if (toChange->addTileIngame(this, true, gridX, gridY)) {
				return true;
			}
			else {
				return false;
			}
		}
		//Checks if there is a tile behind tile
		else if (toChange->getTile(gridX, gridY) != nullptr) {
			if (toChange->getTile(gridX, gridY)->hasBack()) {
				if (toChange->addTileIngame(this, true, gridX, gridY)) {
					return true;
				}
				else {
					return false;
				}
			}
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
	return false;
}

bool material::secondaryUse(heldItem* heldItem, glm::vec2 mousePos, world* toChange) {
	int gridX = (int)(mousePos.x / globals::pixToMet(globals::TILE_SIZE));
	int gridY = (int)(mousePos.y / globals::pixToMet(globals::TILE_SIZE));

	glm::vec2 itemPos = heldItem->getPosition();

	glm::vec2 distanceVec(mousePos.x - itemPos.x, mousePos.y - itemPos.y);
	float distance = glm::length(distanceVec);

	if (distance <= globals::MAX_TILE_RANGE) {
		//Checks if there is an adjacent tile
		if (toChange->getTile(gridX + 1, gridY) != nullptr || toChange->getTile(gridX - 1, gridY) != nullptr ||
			toChange->getTile(gridX, gridY + 1) != nullptr || toChange->getTile(gridX, gridY - 1) != nullptr) {
			if (toChange->addTileIngame(this, false, gridX, gridY)) {
				return true;
			}
			else {
				return false;
			}
		}
		//Checks if there is a tile behind
		else if (toChange->getTile(gridX, gridY) != nullptr) {
			if (toChange->getTile(gridX, gridY)->hasBack()) {
				if (toChange->addTileIngame(this, false, gridX, gridY)) {
					return true;
				}
				else {
					return false;
				}
			}
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
	return false;
}
void material::equipped(heldItem* heldItem, glm::vec2 mousePos, world* toChange) {

}

void material::draw(gengine::spriteBatch* spriteBatch, heldItem* heldItem, glm::vec2 mousePos, world* toChange) {
	int gridX = (int)(mousePos.x / globals::pixToMet(globals::TILE_SIZE));
	int gridY = (int)(mousePos.y / globals::pixToMet(globals::TILE_SIZE));

	glm::vec2 itemPos = heldItem->getPosition();

	glm::vec2 distanceVec(mousePos.x - itemPos.x, mousePos.y - itemPos.y);
	float distance = glm::length(distanceVec);

	bool canBePlaced = false;

	if (distance <= globals::MAX_TILE_RANGE) {
		//Checks if there is a tile behind
		if (toChange->getTile(gridX, gridY) != nullptr) {
			if (toChange->getTile(gridX, gridY)->hasBack() && !toChange->getTile(gridX, gridY)->hasFront()) {
				if (!toChange->getTile(gridX, gridY)->hasFront()) {
					canBePlaced = true;
				}
			}
		}
		//Checks if there is an adjacent tile
		else if (toChange->getTile(gridX + 1, gridY) != nullptr || toChange->getTile(gridX - 1, gridY) != nullptr
			|| toChange->getTile(gridX, gridY + 1) != nullptr || toChange->getTile(gridX, gridY - 1) != nullptr) {
			canBePlaced = true;
		}
	}

	_drawItem(toChange, gridX, gridY, spriteBatch, canBePlaced);
}

gengine::color material::getParticleColor()
{
	return _particleColor;
}

int material::getMaxHP()
{
	return _maxHP;
}

void material::_drawItem(world* world, int gridX, int gridY, gengine::spriteBatch* spriteBatch, bool canBePlaced) {
	glm::vec4 destRect;
	destRect.x = gridX * globals::pixToMet(globals::TILE_SIZE);
	destRect.y = gridY * globals::pixToMet(globals::TILE_SIZE);
	destRect.z = globals::pixToMet(globals::TILE_SIZE);
	destRect.w = globals::pixToMet(globals::TILE_SIZE);
	
	if (canBePlaced) {
		spriteBatch->draw(destRect, glm::vec4(0, 0, 1, 1), _texture.getTexture()->id, 50, gengine::color(255, 255, 255, 175));

		destRect.x -= 0.075f;
		destRect.y -= 0.075f;
		destRect.z += 0.15f;
		destRect.w += 0.15f;

		spriteBatch->draw(destRect, glm::vec4(0, 0, 1, 1), _texture.getTexture()->id, 49, gengine::color(50, 200, 255, 255));
	}
	else {
		spriteBatch->draw(destRect, glm::vec4(0, 0, 1, 1), _texture.getTexture()->id, 50, gengine::color(255, 255, 255, 175));

		destRect.x -= 0.075f;
		destRect.y -= 0.075f;
		destRect.z += 0.15f;
		destRect.w += 0.15f;

		spriteBatch->draw(destRect, glm::vec4(0, 0, 1, 1), _texture.getTexture()->id, 49, gengine::color(255, 0, 0, 255));
	}
}

bool material::hasAutoTiling()
{
	return _autoTiling;
}

bool material::isSolid() {
	return luaHandler::getTableBool(_nameID, "solid");
}

int luaHandler::LUA_MATERIAL_newMaterial(lua_State *S) {
	std::string materialName = lua_tostring(S, 1); //Get name of the table

	if (!_verifyLuaName(materialName)) {
		return 0;
	}

	lua_getglobal(S, materialName.c_str()); // "ground"
	lua_pushstring(S, "setProperties");		// "setProperties", "ground"
	lua_gettable(S, -2);					//	function setProperties, "ground"
	lua_pushvalue(S, -2);					//  "ground", function setProperties, "ground"
	lua_call(S, 1, 0);
	lua_pop(S, 1);

	std::vector<float> color;
	color = getTableTableValues(materialName, "particleColor");
	gengine::color particleColor = gengine::color(color[0], color[1], color[2], color[3]);

	material* newMaterial = new material();
	newMaterial->_nameID = materialName;
	newMaterial->init(getPath(getTableString(materialName, "texture")), getPath(getTableString(materialName, "icon")), getTableBool(materialName, "autoTile"), getTableInt(materialName, "hitpoints"), getTableRange(materialName, "spritesheetSize"), particleColor);
	_definitions.emplace(std::make_pair(materialName, newMaterial));

	lua_pop(S, 1);

	return 0;
}