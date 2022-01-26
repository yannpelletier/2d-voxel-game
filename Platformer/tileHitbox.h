#pragma once
#include <Box2D/Box2D.h>
#include <glm/glm.hpp>
#include <Gengine/spriteBatch.h>
#include <Gengine/GLTexture.h>
#include <Gengine/debugRenderer.h>
#include "userData.h"
#include "globals.h"

#include <vector>

class tile;
class body;
class world;

class tileHitbox {
public:
	virtual void destroy(world* world) { return; }

	virtual void debugDraw(gengine::debugRenderer* debugRenderer) { return; }

	virtual void startContact(userData* collidedWith, userData* thisFixture, b2Contact* contact, world* world) { return; }
	virtual void endContact(userData* collidedWith, userData* thisFixture) { return; }

	virtual b2Body* getBody() { return _body; }

private:
	b2Body* _body;

	bool _validTile(int gridX, int gridY, world* world);

	void LUA_onHit();
};