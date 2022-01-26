#pragma once
#include <Gengine/spriteSheet.h>
#include "luaHandler.h"

class materialMod;
class tile;

class materialModInstance
{
public:
	friend class luaHandler;

	materialModInstance();
	~materialModInstance();

	void init(materialMod* mod, tile* placedTile);
	void update(tile* tile);

	void setAnimation(std::string animationName);

	void damage(int damage, tile* tile, materialModInstance** modPtr);

	gengine::spriteSheet getTexture();

	materialMod* getMaterialMod();

private:
	materialMod* _mod;
	gengine::spriteSheet _texture;

	std::map<std::string, std::pair<void*, luaVariableType>> LUA_cppvars;

	int _health;

};
