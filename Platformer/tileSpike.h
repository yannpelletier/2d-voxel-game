#pragma once
#include "tileHitbox.h"

class tileSpike: public tileHitbox
{
public:
	~tileSpike() { }

	void init(world* world, glm::ivec2 tilePos, glm::vec2 leftStartingPoint, glm::vec2 rightStartingPoint, tile* meshedTile);
	void destroy(world* world);

	void debugDraw(gengine::debugRenderer* debugRenderer);

	void startContact(userData* collidedWith, userData* thisFixture, b2Contact* contact, world* world);
	void endContact(userData* collidedWith, userData* thisFixture);

	float getLineXCoordinate(float& y, float slope, float b);
	float getLineYCoordinate(float& x, float slope, float b);


private:
	b2Body* _body;

	tile* _occupyingTile;

	std::vector<b2Vec2> _vertices;

	bool _validTile(int gridX, int gridY, world* world);

	void LUA_onHit();
};

