#include "childTile.h"
#include "world.h"

childTile::childTile() {
	_type = tileType::CHILD_TILE;
}

void childTile::_initBack(int gridX, int gridY, definition * definition, int autoTileCount)
{
	_position.x = gridX * globals::pixToMet(globals::TILE_SIZE);
	_position.y = gridY * globals::pixToMet(globals::TILE_SIZE);

	if (definition->getDefinitionType() == MATERIAL) {
		_backMaterial = (material*)definition;
		_backAutoTileCount = autoTileCount;
		_backHealth = _backMaterial->getMaxHP();
	}
}

void childTile::addMaterialMod(materialMod* mod, bool front) {
	if (!front) {
		if (_backMod == nullptr) {
			_backMod = new materialModInstance();
			_backMod->init(mod, this);
		}
	}
}

void childTile::_destroyBack(bool drop) {
	if (drop) {
		inventoryItem* newItem = new inventoryItem(1, _backMaterial, glm::vec2(_position.x + globals::pixToMet(globals::TILE_SIZE) / 2, _position.y + globals::pixToMet(globals::TILE_SIZE) / 2), luaHandler::getWorld(), b2Vec2(0, 0));
		luaHandler::getWorld()->addDrop(newItem);
	}

	_backMaterial = nullptr;

	if (_backMod != nullptr) {
		_backMod->getMaterialMod()->LUA_onDestroy(_backMod, this);
		delete _backMod;
		_backMod = nullptr;
	}
}

void childTile::_drawBack(gengine::spriteBatch* spriteBatch) {
	if (_backMaterial != nullptr) {
		glm::vec4 destRect;
		destRect.x = _position.x;
		destRect.y = _position.y;
		destRect.z = globals::pixToMet(globals::TILE_SIZE);
		destRect.w = globals::pixToMet(globals::TILE_SIZE);

		if (_backMaterial != nullptr) {
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
}

void childTile::_updateFront()
{
	if (_frontMaterial != nullptr) {
		if (_frontHealth < _frontMaterial->getFrontItem()->getMaxHP()) {
			if (SDL_GetTicks() - tileRepairTime >= tile::TIME_TO_REPAIR) {
				_frontHealth += 1;
			}
		}
	}
}

void childTile::_updateBack()
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

void childTile::use(agent* user) {
	_frontMaterial->use(user);
}

void childTile::setFatherTile(fatherTile * tile)
{
	_frontMaterial = tile;
	_frontHealth = tile->getFrontItem()->getMaxHP() - 1;
}

void childTile::_damageFront(int damage) {
	_frontHealth -= damage;

	if (_frontHealth <= 0) {
		world* toChange = luaHandler::getWorld();

		glm::ivec2 tilePos = _frontMaterial->getTilePosition();

		toChange->deleteTileIngame(true, tilePos.x, tilePos.y);
	}
}

void childTile::_damageBack(int damage) {
	if (_backMaterial != nullptr) {
		_backHealth -= damage;

		_emitParticles(_backMaterial->getParticleColor());

		if (_backMod != nullptr) {
			_backMod->damage(damage, this, &_backMod);
		}

		if (_backHealth <= 0) {
			world* toChange = luaHandler::getWorld();

			glm::ivec2 tilePos = _frontMaterial->getTilePosition();

			toChange->deleteTileIngame(false, tilePos.x, tilePos.y);
		}
	}
}

bool childTile::hasBack() {
	if (_backMaterial != nullptr) {
		return true;
	}
	else {
		return false;
	}
}

void childTile::setAutoTileCount(bool front, int autoTileCount) {
	if (!front) {
		_backAutoTileCount = autoTileCount;
	}
}

void childTile::setPosition(int gridX, int gridY) {
	_position.x = gridX * globals::pixToMet(globals::TILE_SIZE);
	_position.y = gridY * globals::pixToMet(globals::TILE_SIZE);
}