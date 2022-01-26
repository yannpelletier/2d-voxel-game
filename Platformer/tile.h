#pragma once
#include "materialModInstance.h"
#include "materialMod.h"
#include "tileHitbox.h"

enum tileType{ INVALID, REGULAR_TILE, FATHER_TILE, CHILD_TILE};

class tile {
public: 
	tile();

	void init(int gridX, int gridY, definition* definition, bool front, int autoTileCount);
	virtual void addMaterialMod(materialMod* mod, bool front);

	//Returns true if there is no material is both the front and the back of the tile
	void destroy(bool front, bool drop);

	void draw(gengine::spriteBatch* spriteBatch);
	void update();

	virtual void occupyBatch(tileHitbox* tileBatch);

	virtual void use(agent* user);

	void damage(int damage, bool front);

	virtual void setAutoTileCount(bool front, int autoTileCount);

	virtual void setPosition(int gridX, int gridY);

	virtual tileType getType();
	virtual materialModInstance* getFrontModInstance();
	virtual materialModInstance* getBackModInstance();

	virtual tileHitbox* getBatch();
	virtual glm::vec2 getTilePosition();
	virtual bool isInMesh();
	virtual bool isSolid();
	virtual bool hasFront();
	virtual bool hasBack();
	virtual bool isFloor();

	virtual material* getFrontMaterial();
	virtual material* getBackMaterial();

	virtual int getFrontHealth();
	virtual int getBackHealth();

	static int tileRepairTime;

	static const int TIME_TO_REPAIR;
	static const glm::vec2 PARTICLE_SPEED_RANGE;
	static const glm::vec2 PARTICLE_WIDTH_RANGE;
	static const glm::vec2 PARTICLE_NUMBER_RANGE;

protected:
	glm::vec2 _position;
	tileType _type;

	virtual void _initFront(int gridX, int gridY, definition* definition, int autoTileCount);
	virtual void _initBack(int gridX, int gridY, definition* definition, int autoTileCount);

	virtual void _damageFront(int damage);
	virtual void _damageBack(int damage);

	virtual void _drawFront(gengine::spriteBatch* spriteBatch);
	virtual void _drawBack(gengine::spriteBatch* spriteBatch);

	virtual void _updateFront();
	virtual void _updateBack();

	virtual void _destroyFront(bool drop);
	virtual void _destroyBack(bool drop);

	void _emitParticles(gengine::color particleColor);
};
