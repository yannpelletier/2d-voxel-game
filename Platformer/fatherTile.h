#pragma once
#include "tile.h"
#include "inventory.h"
#include <vector>

class childTile;

class fatherTile : public tile {
	friend class luaHandler;
public:
	fatherTile();
	void addMaterialMod(materialMod* mod, bool front);

	void setLightID(int lightID);
	void setParticleEmitterID(int emitterID);

	int getLightID();
	int getEmitterID();

	void use(agent* user);

	bool wasInitialized();

	tileType getType();
	materialModInstance* getBackModInstance();

	glm::vec2* getPositionPointer();
	//Makes sure they are never meshed with
	bool isInMesh();
	bool isSolid();
	bool hasFront();
	bool hasBack();
	bool isFloor();

	void setAutoTileCount(bool front, int autoTileCount);

	void setPosition(int gridX, int gridY);

	placableItem* getFrontItem() { return _frontMaterial; }
	material* getBackMaterial() { return _backMaterial; }

	void createInventory(int size, int width);
	inventory* getInventory();

	void setAnimation(std::string animName, int loops);

	void setFrontHealth(int frontHealth) { _frontHealth = frontHealth; }
	void setBackHealth(int backHealth) { _backHealth = backHealth; }

	int getFrontHealth() { return _frontHealth; }
	int getBackHealth() { return _backHealth; }
private:
	int _lightID = -1;
	int _particleEmitterID = -1;
	inventory* _inventory = nullptr;
	bool _initialized = false;

	gengine::spriteSheet _texture;

	placableItem* _frontMaterial = nullptr;
	material* _backMaterial = nullptr;

	std::vector<childTile*> _childrenTiles;
	std::map<std::string, std::pair<void*, luaVariableType>> LUA_cppvars;

	int8 _backAutoTileCount = 0;
	materialModInstance* _backMod = nullptr;

	int8 _frontHealth;
	int8 _backHealth;

	void _initFront(int gridX, int gridY, definition* definition, int autoTileCount);
	void _initBack(int gridX, int gridY, definition* definition, int autoTileCount);

	void _damageFront(int damage);
	void _damageBack(int damage);

	void _drawFront(gengine::spriteBatch* spriteBatch);
	void _drawBack(gengine::spriteBatch* spriteBatch);

	void _updateFront();
	void _updateBack();

	void _destroyFront(bool drop);
	void _destroyBack(bool drop);
};