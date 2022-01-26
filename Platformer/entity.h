#pragma once
#include <glm/glm.hpp>
#include <Box2D/Box2D.h>
#include <Gengine/spriteSheet.h>
#include <Gengine/spriteBatch.h>
#include <Gengine/debugRenderer.h>
#include <Gengine/vertex.h>

#include "inventory.h"
#include "userData.h"
#include "body.h"


class inventory;

class entity{
public:
	virtual void draw(gengine::spriteBatch* spriteBatch);
	virtual void update();
	virtual void destroy(world* world);

	virtual void debugDraw(gengine::debugRenderer* debugRenderer);

	virtual void startContact(userData* collidedWith, userData* thisFixture);
	virtual void endContact(userData* collidedWith, userData* thisFixture);

	body* getBody();

	void setActive(bool value);
	bool isActive();
private:

protected:
	glm::vec2 _position = glm::vec2(0, 0);
	float _angle = 0;
	glm::vec2 _drawDimensions;

	body* _body = nullptr;

	bool _active = true;
};