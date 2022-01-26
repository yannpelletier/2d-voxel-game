#pragma once
#include "tile.h"
#include "tileHitbox.h"

class regularTile : public tile
{
public:
	regularTile();
	void addMaterialMod(materialMod* mod, bool front);

	void occupyBatch(tileHitbox* tileBatch);

	void use(agent* user);

	void setAutoTileCount(bool front, int autoTileCount);

	void setPosition(int gridX, int gridY);

	tileType getType() { return tileType::REGULAR_TILE; }

	tileHitbox* getBatch() { return _tileBatch; }
	bool isInMesh();
	bool isSolid();
	bool hasFront();
	bool hasBack();
	bool isFloor();

	material* getFrontMaterial() { return _frontMaterial; }
	material* getBackMaterial() { return _backMaterial; }
	materialModInstance* getFrontModInstance() { return _frontMod; }
	materialModInstance* getBackModInstance() { return _backMod; }

	void setFrontHealth(int frontHealth) { _frontHealth = frontHealth; }
	void setBackHealth(int backHealth) { _backHealth = backHealth; }
	int getFrontHealth() { return _frontHealth; }
	int getBackHealth() { return _backHealth; }
private:
	material* _frontMaterial = nullptr;
	material* _backMaterial = nullptr;

	tileHitbox* _tileBatch = nullptr;

	materialModInstance* _frontMod = nullptr;
	materialModInstance* _backMod = nullptr;
	int8 _frontAutoTileCount = 0;
	int8 _backAutoTileCount = 0;

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