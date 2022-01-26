#pragma once
#include "tile.h"
#include "fatherTile.h"
#include "placableItem.h"

class childTile : public tile {
public:
	childTile();
	void addMaterialMod(materialMod* mod, bool front);

	void use(agent* user);

	tileType getType() { return tileType::CHILD_TILE; }
	materialModInstance* getBackModInstance() { return _backMod; }

	void setFatherTile(fatherTile* tile);

	//Makes sure they are never meshed with
	bool isInMesh() {
		return false;
	}
	bool isSolid() { return false; }
	bool hasFront() { return true; }
	bool hasBack();
	bool isFloor() { return false; }

	void setAutoTileCount(bool front, int autoTileCount);

	void setPosition(int gridX, int gridY);

	material* getBackMaterial() { return _backMaterial; }
	fatherTile* getFatherTile() { return _frontMaterial; }

	void setFrontHealth(int frontHealth) { _frontHealth = frontHealth; }
	void setBackHealth(int backHealth) { _backHealth = backHealth; }

	int getFrontHealth() { return _frontHealth; }
	int getBackHealth() { return _backHealth; }
private:
	fatherTile* _frontMaterial = nullptr;
	material* _backMaterial = nullptr;

	int8 _backAutoTileCount = 0;
	materialModInstance* _backMod = nullptr;

	int8 _frontHealth;
	int8 _backHealth;

	void _initBack(int gridX, int gridY, definition* definition, int autoTileCount);

	void _damageFront(int damage);
	void _damageBack(int damage);

	void _drawBack(gengine::spriteBatch* spriteBatch);

	void _updateFront();
	void _updateBack();

	void _destroyBack(bool drop);
};