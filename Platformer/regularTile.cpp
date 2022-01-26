#include "regularTile.h"
#include "world.h"

regularTile::regularTile() {
	_type = tileType::REGULAR_TILE;
}

void regularTile::_initFront(int gridX, int gridY, definition * definition, int autoTileCount)
{
	if (definition->getDefinitionType() == MATERIAL) {
		_position.x = gridX * globals::pixToMet(globals::TILE_SIZE);
		_position.y = gridY * globals::pixToMet(globals::TILE_SIZE);

		_frontMaterial = (material*)definition;
		_frontAutoTileCount = autoTileCount;
		_frontHealth = _frontMaterial->getMaxHP();
	}
}

void regularTile::_initBack(int gridX, int gridY, definition * definition, int autoTileCount)
{
	if (definition->getDefinitionType() == MATERIAL) {
		_position.x = gridX * globals::pixToMet(globals::TILE_SIZE);
		_position.y = gridY * globals::pixToMet(globals::TILE_SIZE);

		_backMaterial = (material*)definition;
		_backAutoTileCount = autoTileCount;
		_backHealth = _backMaterial->getMaxHP();
	}
}

void regularTile::addMaterialMod(materialMod* mod, bool front) {
	if (front) {
		if (_frontMod == nullptr) {
			_frontMod = new materialModInstance();
			_frontMod->init(mod, this);
		}
	}
	else {
		if (_backMod == nullptr) {
			_backMod = new materialModInstance();
			_backMod->init(mod, this);
		}
	}
}

void regularTile::_destroyFront(bool drop) {
	if (drop) {
		inventoryItem* newItem = new inventoryItem(1, _frontMaterial, glm::vec2(_position.x + globals::pixToMet(globals::TILE_SIZE) / 2, _position.y + globals::pixToMet(globals::TILE_SIZE) / 2), luaHandler::getWorld(), b2Vec2(0, 0));
		luaHandler::getWorld()->addDrop(newItem);
	}
	if (_frontMod != nullptr) {
		_frontMod->getMaterialMod()->LUA_onDestroy(_frontMod, this);
		delete _frontMod;
		_frontMod = nullptr;
	}
	luaHandler::getWorld()->_deleteBatch(_tileBatch);

	_frontMaterial = nullptr;
	_tileBatch = nullptr;

	if (_frontMaterial == nullptr && _backMaterial == nullptr) {
		glm::vec2 tilePos;
		tilePos.x = _position.x / globals::pixToMet(globals::TILE_SIZE);
		tilePos.y = _position.y / globals::pixToMet(globals::TILE_SIZE);

		luaHandler::getWorld()->replaceForEmptyTile(tilePos.x, tilePos.y);
	}
}

void regularTile::_destroyBack(bool drop) {
	if (drop) {
		inventoryItem* newItem = new inventoryItem(1, _backMaterial, glm::vec2(_position.x + globals::pixToMet(globals::TILE_SIZE) / 2, _position.y + globals::pixToMet(globals::TILE_SIZE) / 2), luaHandler::getWorld(), b2Vec2(0, 0));
		luaHandler::getWorld()->addDrop(newItem);
	}
	if (_backMod != nullptr) {
		_backMod->getMaterialMod()->LUA_onDestroy(_backMod, this);
		delete _backMod;
		_backMod = nullptr;
	}

	_backMaterial = nullptr;
}

bool regularTile::isInMesh()
{
	if (_tileBatch == nullptr) {
		return false;
	}
	else {
		return true;
	}
}

void regularTile::_drawFront(gengine::spriteBatch* spriteBatch) {
	if (_frontMaterial != nullptr) {
		glm::vec4 destRect;
		destRect.x = _position.x;
		destRect.y = _position.y;
		destRect.z = globals::pixToMet(globals::TILE_SIZE);
		destRect.w = globals::pixToMet(globals::TILE_SIZE);

		gengine::spriteSheet texture = _frontMaterial->getTexture();
		if (_frontMaterial->hasAutoTiling()) {
			spriteBatch->draw(destRect, texture.getUvsOf(_frontAutoTileCount), texture.getTexture()->id, globals::FRONT_TILE_DRAW_LAYER, gengine::color(255, 255, 255, 255));
		}
		else {
			spriteBatch->draw(destRect, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), texture.getTexture()->id, globals::FRONT_TILE_DRAW_LAYER, gengine::color(255, 255, 255, 255));
		}

		if (_frontMod != nullptr) {
			gengine::spriteSheet texture = _frontMod->getTexture();
			if (_frontMod->getMaterialMod()->hasAutoTiling()) {
				spriteBatch->draw(destRect, texture.getUvsOf(_frontAutoTileCount), texture.getTexture()->id, globals::FRONT_TILE_DRAW_LAYER + 1, gengine::color(255, 255, 255, 255));
			}
			else {
				spriteBatch->draw(destRect, texture.getAnimUV(), texture.getTexture()->id, globals::FRONT_TILE_DRAW_LAYER + 1, gengine::color(255, 255, 255, 255));
			}
		}

		if (_frontHealth <= std::floor(_frontMaterial->getMaxHP() / 3)) {
			spriteBatch->draw(destRect, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), gengine::ressourceManager::getTexture("assets/materials/cracks/big crack.png").id, globals::FRONT_TILE_DRAW_LAYER + 2, gengine::color(255, 255, 255, 255));
		}
		else if (_frontHealth <= std::floor(_frontMaterial->getMaxHP() / 3 * 2)) {
			spriteBatch->draw(destRect, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), gengine::ressourceManager::getTexture("assets/materials/cracks/small crack.png").id, globals::FRONT_TILE_DRAW_LAYER + 2, gengine::color(255, 255, 255, 255));
		}
	}
}

void regularTile::_drawBack(gengine::spriteBatch* spriteBatch) {
	if (_backMaterial != nullptr) {
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

void regularTile::_updateFront()
{
	if (_frontMaterial != nullptr) {
		if (_frontHealth < _frontMaterial->getMaxHP()) {
			if (SDL_GetTicks() - tileRepairTime >= tile::TIME_TO_REPAIR) {
				_frontHealth += 1;
			}
		}
	}

	if (_frontMod != nullptr) {
		_frontMod->update(this);
	}
}

void regularTile::_updateBack()
{
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

void regularTile::occupyBatch(tileHitbox* tileBatchs) {
	_tileBatch = tileBatchs;
}

void regularTile::use(agent* user) {
	return;
}

void regularTile::_damageFront(int damage) {
	if (_frontMaterial != nullptr) {
		_frontHealth -= damage;

		_emitParticles(_frontMaterial->getParticleColor());

		if (_frontMod != nullptr) {
			_frontMod->damage(damage, this, &_frontMod);
		}

		if (_frontHealth <= 0) {
			world* toChange = luaHandler::getWorld();

			glm::ivec2 tilePos;
			tilePos.x = _position.x / globals::pixToMet(globals::TILE_SIZE);
			tilePos.y = _position.y / globals::pixToMet(globals::TILE_SIZE);

			toChange->deleteTileIngame(true, tilePos.x, tilePos.y);
		}
	}
}

void regularTile::_damageBack(int damage) {
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

void regularTile::setAutoTileCount(bool front, int autoTileCount) {
	if (front) {
		_frontAutoTileCount = autoTileCount;
	}
	else {
		_backAutoTileCount = autoTileCount;
	}
}

void regularTile::setPosition(int gridX, int gridY) {
	_position.x = gridX * globals::pixToMet(globals::TILE_SIZE);
	_position.y = gridY * globals::pixToMet(globals::TILE_SIZE);
}


bool regularTile::isSolid() {
	if (_frontMaterial != nullptr) {
		return _frontMaterial->isSolid();
	}
	else {
		return false;
	}
}

bool regularTile::hasFront() {
	if (_frontMaterial != nullptr) {
		return true;
	}
	else {
		return false;
	}
}

bool regularTile::hasBack() {
	if (_backMaterial != nullptr) {
		return true;
	}
	else {
		return false;
	}
}

bool regularTile::isFloor() {
	if (_frontMaterial != nullptr) {
		return true;
	}
	else {
		return false;
	}
}