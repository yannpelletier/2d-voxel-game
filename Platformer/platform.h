#pragma once
#include "tileHitbox.h"

class platform : public tileHitbox
{
public:
	void init(world* world, glm::vec2 startPosition, float width);

	void destroy(world* world);

	b2Body* getBody() { return _body; }

	float getWidth() { return _width; }

private:
	float _width;

	b2Body* _body;
};

