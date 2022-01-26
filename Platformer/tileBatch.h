#pragma once
#include <Box2D/Box2D.h>
#include <glm/glm.hpp>
#include <Gengine/spriteBatch.h>
#include <Gengine/GLTexture.h>
#include <Gengine/debugRenderer.h>
#include "userData.h"
#include "globals.h"
#include "tileHitbox.h"

class tileBatch : public tileHitbox
{
public:
	~tileBatch();

	void init(world* world, glm::ivec2 topLeft, glm::ivec2 bottomRight, std::vector<tile*> meshedTiles);
	void destroy(world* world);

	void debugDraw(gengine::debugRenderer* debugRenderer);

	void startContact(userData* collidedWith, userData* thisFixture, b2Contact* contact, world* world);
	void endContact(userData* collidedWith, userData* thisFixture);

	glm::ivec2 getCollisionedTile(userData* collidedWith, userData* thisFixture, b2Contact* contact, world* world);

	glm::vec2 getDimensions() { return _dimensions; }

private:
	b2Body* _body;

	glm::vec2 _dimensions;

	std::vector<tile*> _occupyingTiles;

	bool _validTile(int gridX, int gridY, world* world);

	void LUA_onHit();
};