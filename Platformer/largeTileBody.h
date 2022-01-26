#pragma once
#include "tileHitbox.h"
#include "luaHandler.h"

class largeTileBody : public tileHitbox
{
public:
	void init(world* world, glm::vec2 startPosition, glm::vec2 dimensions);

	void destroy(world* world);

	b2Body* getBody() { return _body; }

	glm::vec2 getDimensions() { return _dimensions; }

private:
	glm::vec2 _dimensions;

	b2Body* _body;

};