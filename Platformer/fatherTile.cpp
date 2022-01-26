#include "childTile.h"
#include "fatherTile.h"
#include "placableItem.h"
#include "platform.h"
#include "GUI.h"
#include "world.h"

fatherTile::fatherTile() {
	_type = tileType::FATHER_TILE;
}

void fatherTile::_initFront(int gridX, int gridY, definition * definition, int autoTileCount)
{
	if (_initialized == false) {
		if (definition->getDefinitionType() == PLACABLE_ITEM) {
			_position.x = gridX * globals::pixToMet(globals::TILE_SIZE);
			_position.y = gridY * globals::pixToMet(globals::TILE_SIZE);
			_frontMaterial = static_cast<placableItem*>(definition);
			_texture = static_cast<placableItem*>(definition)->getTexture();

			_frontMaterial->LUA_onPlaced(this);
			if (_lightID != -1) {
				luaHandler::getWorld()->orderLights();
			}
			_initialized = true;

			for (int i = 0; i < ((_frontMaterial->getSize().x * _frontMaterial->getSize().y) - 1); i++) {
				int tileGridPosX = (int)((i + 1) % _frontMaterial->getSize().x) + (_position.x / globals::pixToMet(globals::TILE_SIZE));
				int tileGridPosY = (int)((i + 1) / _frontMaterial->getSize().x) + (_position.y / globals::pixToMet(globals::TILE_SIZE));

				childTile* newTile = luaHandler::getWorld()->replaceForChildTile(tileGridPosX, tileGridPosY);
				newTile->setFatherTile(this);
				_childrenTiles.push_back(newTile);
			}

			_frontHealth = _frontMaterial->getMaxHP();
		}
	}
}

void fatherTile::_initBack(int gridX, int gridY, definition * definition, int autoTileCount)
{
	_position.x = gridX * globals::pixToMet(globals::TILE_SIZE);
	_position.y = gridY * globals::pixToMet(globals::TILE_SIZE);

	if (definition->getDefinitionType() == MATERIAL) {
		_backMaterial = (material*)definition;
		_backAutoTileCount = autoTileCount;
		_backHealth = _backMaterial->getMaxHP();
	}
}


void fatherTile::addMaterialMod(materialMod* mod, bool front) {
	if (!front) {
		if (_backMod == nullptr) {
			_backMod = new materialModInstance();
			_backMod->init(mod, this);
		}
	}
}

glm::vec2* fatherTile::getPositionPointer() {
	return &_position;
}

bool fatherTile::isInMesh()
{
	return true;
}

bool fatherTile::isSolid()
{
	return false;
}

bool fatherTile::hasFront()
{
	return true;
}

bool fatherTile::isFloor()
{
	return false;
}

void fatherTile::_destroyFront(bool drop) {
	world* world = luaHandler::getWorld();
	//Destroy all of the child tiles
	for (int i = 0; i < ((_frontMaterial->getSize().x * _frontMaterial->getSize().y) - 1); i++) {
		int tileGridPosX = (int)((i + 1) % _frontMaterial->getSize().x) + (_position.x / globals::pixToMet(globals::TILE_SIZE));
		int tileGridPosY = (int)((i + 1) / _frontMaterial->getSize().x) + (_position.y / globals::pixToMet(globals::TILE_SIZE));

		if (_childrenTiles[i]->hasBack()) {
			world->replaceForRegularTile(tileGridPosX, tileGridPosY);
		}
		else {
			world->replaceForEmptyTile(tileGridPosX, tileGridPosY);
		}
	}
	//Drop the item 
	if (drop) {
		inventoryItem* newItem = new inventoryItem(1, _frontMaterial, glm::vec2(_position.x + (_frontMaterial->getSize().x * globals::pixToMet(globals::TILE_SIZE) / 2), _position.y + (_frontMaterial->getSize().y * globals::pixToMet(globals::TILE_SIZE) / 2)), world, b2Vec2(0, 0));
		world->addDrop(newItem);
	}

	_frontMaterial = nullptr;

	//Delete the light and particle emitter of the item
	if (_lightID != -1) {
		world->deleteLightAndOrder(_lightID);
	}
	if (_particleEmitterID != -1) {
		world->deleteParticleEmitter(_particleEmitterID);
	}

	//Delete the variables stored for LUA uses
	if (!LUA_cppvars.empty()) {
		for (auto it : LUA_cppvars) {
			if (it.second.second == luaVariableType::BODYL) {
				static_cast<largeTileBody*>(it.second.first)->destroy(world);
			}
			else if (it.second.second == luaVariableType::PLATFORML) {
				static_cast<platform*>(it.second.first)->destroy(world);
			}
			else {
				free(it.second.first);
			}
		}
	}
	//Delete every item in the inventory
	if (_inventory != nullptr) {
		glm::vec2 dropPosition = _position;
		dropPosition.x += (_frontMaterial->getSize().x * globals::pixToMet(globals::TILE_SIZE)) / 2;
		dropPosition.x += (_frontMaterial->getSize().y * globals::pixToMet(globals::TILE_SIZE)) / 2;
		for (int i = 0; i < _inventory->getSize(); i++) {
			if (_inventory->getItem(i) != nullptr) {
				_inventory->getItem(i)->drop(_inventory->getQuantity(i), _position + globals::pixToMet(globals::TILE_SIZE) / 2, world, b2Vec2(0, 0));
			}
		}
		if (!_inventory->isClosed()) {
			GUI::openWorldInventory(_inventory);
		}
		delete _inventory;
	}
	glm::ivec2 position = getTilePosition();
	if (hasBack()) {
		world->replaceForRegularTile((int)position.x, (int)position.y);
	}
	else {
		world->replaceForEmptyTile((int)position.x, (int)position.y);
	}
}

void fatherTile::_destroyBack(bool drop) {
	if (drop) {
		inventoryItem* newItem = new inventoryItem(1, _backMaterial, glm::vec2(_position.x + globals::pixToMet(globals::TILE_SIZE) / 2, _position.y + globals::pixToMet(globals::TILE_SIZE) / 2), luaHandler::getWorld(), b2Vec2(0, 0));
		luaHandler::getWorld()->addDrop(newItem);
	}

	_backMaterial = nullptr;

	if (_backMod != nullptr) {
		delete _backMod;
		_backMod = nullptr;
	}
}

void fatherTile::_drawFront(gengine::spriteBatch* spriteBatch) {
	_texture.update();

	glm::vec2 size = _frontMaterial->getSize();

	glm::vec4 destRect;
	destRect.x = _position.x;
	destRect.y = _position.y;
	destRect.z = size.x * globals::pixToMet(globals::TILE_SIZE);
	destRect.w = size.y * globals::pixToMet(globals::TILE_SIZE);

	spriteBatch->draw(destRect, _texture.getAnimUV(), _texture.getTexture()->id, globals::PLACABLE_ITEM_DRAW_LAYER, gengine::color(255, 255, 255, 255));
}

void fatherTile::_drawBack(gengine::spriteBatch* spriteBatch) {
	if (_backMaterial != nullptr) {
		glm::vec2 size = _frontMaterial->getSize();

		glm::vec4 destRect;
		destRect.x = _position.x;
		destRect.y = _position.y;
		destRect.z = globals::pixToMet(globals::TILE_SIZE);
		destRect.w = globals::pixToMet(globals::TILE_SIZE);

		gengine::spriteSheet texture = _backMaterial->getTexture();

		if (_backMaterial->hasAutoTiling()) {
			spriteBatch->draw(destRect, texture.getUvsOf(_backAutoTileCount), texture.getTexture()->id, globals::BACK_TILE_DRAW_LAYER, globals::BACK_TILE_COLOR);
		}
		else {
			spriteBatch->draw(destRect, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), texture.getTexture()->id, globals::BACK_TILE_DRAW_LAYER, globals::BACK_TILE_COLOR);
		}

		if (_backMod != nullptr) {
			if (_backMod->getMaterialMod()->hasAutoTiling()) {
				spriteBatch->draw(destRect, _backMod->getTexture().getUvsOf(_backAutoTileCount), _backMod->getTexture().getTexture()->id, globals::BACK_TILE_DRAW_LAYER + 1, globals::BACK_TILE_COLOR);
			}
			else {
				spriteBatch->draw(destRect, _backMod->getTexture().getAnimUV(), _backMod->getTexture().getTexture()->id, globals::BACK_TILE_DRAW_LAYER + 1, globals::BACK_TILE_COLOR);
			}
		}

		if (_backHealth <= std::floor(_backMaterial->getMaxHP() / 3)) {
			spriteBatch->draw(destRect, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), gengine::ressourceManager::getTexture("assets/materials/cracks/big crack.png").id, globals::BACK_TILE_DRAW_LAYER + 2, gengine::color(255, 255, 255, 255));
		}
		else if (_backHealth <= std::floor(_backMaterial->getMaxHP() / 3 * 2)) {
			spriteBatch->draw(destRect, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), gengine::ressourceManager::getTexture("assets/materials/cracks/small crack.png").id, globals::BACK_TILE_DRAW_LAYER + 2, gengine::color(255, 255, 255, 255));
		}
	}
}

void fatherTile::setLightID(int lightID)
{
	if (lightID >= 0) {
		_lightID = lightID;
	}
}

void fatherTile::setParticleEmitterID(int emitterID) {
	if (emitterID >= 0) {
		_particleEmitterID = emitterID;
	}
}

int fatherTile::getLightID()
{
	return _lightID;
}

int fatherTile::getEmitterID()
{
	return _particleEmitterID;
}

void fatherTile::_updateFront() {
	if (_frontMaterial != nullptr) {
		_frontMaterial->LUA_update(this);

		if (_frontHealth < _frontMaterial->getMaxHP()) {
			if (SDL_GetTicks() - tileRepairTime >= tile::TIME_TO_REPAIR) {
				_frontHealth += 1;
			}
		}
	}
}

void fatherTile::_updateBack() {
	if (_backMaterial != nullptr) {
		if (_backHealth < _backMaterial->getMaxHP()) {
			if (SDL_GetTicks() - tileRepairTime >= tile::TIME_TO_REPAIR) {
				_backHealth += 1;
			}
		}
	}

	if (_backMod != nullptr) {
		_backMod->update(this);
	}
}

void fatherTile::use(agent* user) {
	_frontMaterial->LUA_use(this, user);
}

bool fatherTile::wasInitialized()
{
	return _initialized;
}

tileType fatherTile::getType()
{
	return _type;
}

materialModInstance * fatherTile::getBackModInstance()
{
	return _backMod;
}

void fatherTile::_damageFront(int damage) {
	_frontHealth -= damage;

	if (_frontHealth <= 0) {
		world* toChange = luaHandler::getWorld();

		glm::ivec2 tilePos;
		tilePos.x = _position.x / globals::pixToMet(globals::TILE_SIZE);
		tilePos.y = _position.y / globals::pixToMet(globals::TILE_SIZE);

		toChange->deleteTileIngame(true, tilePos.x, tilePos.y);
	}
}

void fatherTile::_damageBack(int damage) {
	if (_backMaterial != nullptr) {
		_backHealth -= damage;

		_emitParticles(_backMaterial->getParticleColor());

		if (_backMod != nullptr) {
			_backMod->damage(damage, this, &_backMod);
		}

		if (_backHealth <= 0) {
			world* toChange = luaHandler::getWorld();

			glm::ivec2 tilePos;
			tilePos.x = _position.x / globals::pixToMet(globals::TILE_SIZE);
			tilePos.y = _position.y / globals::pixToMet(globals::TILE_SIZE);

			toChange->deleteTileIngame(false, tilePos.x, tilePos.y);
		}
	}
}


bool fatherTile::hasBack() {
	if (_backMaterial != nullptr) {
		return true;
	}
	else {
		return false;
	}
}

void fatherTile::setAutoTileCount(bool front, int autoTileCount) {
	if (!front) {
		_backAutoTileCount = autoTileCount;
	}
}

void fatherTile::setPosition(int gridX, int gridY) {
	_position.x = gridX * globals::pixToMet(globals::TILE_SIZE);
	_position.y = gridY * globals::pixToMet(globals::TILE_SIZE);
}

void fatherTile::createInventory(int size, int width)
{
	if (_inventory == nullptr) {
		_inventory = new inventory(size);
		_inventory->setWidth(width);
	}
}

inventory * fatherTile::getInventory()
{
	return _inventory;
}

void fatherTile::setAnimation(std::string animName, int loops) {
	_texture.changeAnimation(animName);
	_texture.setLoops(loops);
}