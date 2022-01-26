#include "materialModInstance.h"
#include "materialMod.h"
#include "tile.h"


materialModInstance::materialModInstance()
{
}


materialModInstance::~materialModInstance()
{
}


void materialModInstance::init(materialMod* mod, tile* placedTile) {
	_texture = mod->getTexture();
	_mod = mod;
	_health = mod->getMaxHP();

	_mod->LUA_onPlaced(this, placedTile);
}

void materialModInstance::update(tile* tile) {
	_mod->LUA_update(this, tile);

	if (_health < _mod->getMaxHP()) {
		if (SDL_GetTicks() - tile::tileRepairTime >= 3000) {
			_health += 1;
		}
	}

}

void materialModInstance::setAnimation(std::string animationName) {
	_texture.changeAnimation(animationName);
}

void materialModInstance::damage(int damage, tile* tile, materialModInstance** modPtr) {
	_health -= damage;
	if (_health <= 0) {
		_mod->LUA_onDestroy(this, tile);
		*modPtr = nullptr;
		delete this;
	}
}

gengine::spriteSheet materialModInstance::getTexture()
{
	return _texture;
}

materialMod * materialModInstance::getMaterialMod()
{
	return _mod;
}
