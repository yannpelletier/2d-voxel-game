#pragma once
#include "tileHitbox.h"

class tileSlope : public tileHitbox {
public:
	~tileSlope() { }

	void init(world* world, glm::ivec2 tilePos, glm::vec2 startingPoint, glm::vec2 endPoint, tile* meshedTile);
	void destroy(world* world);

	void debugDraw(gengine::debugRenderer* debugRenderer);

	void startContact(userData* collidedWith, userData* thisFixture, b2Contact* contact, world* world);
	void endContact(userData* collidedWith, userData* thisFixture);


private:
	b2Body* _body;

	tile* _occupyingTile;

	std::vector<b2Vec2> _vertices;

	bool _validTile(int gridX, int gridY, world* world);

	void LUA_onHit();
};