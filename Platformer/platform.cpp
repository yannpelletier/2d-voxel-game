#include "platform.h"
#include "world.h"

void platform::init(world* world, glm::vec2 startPosition, float width) {
	float meterTileSize = globals::pixToMet(globals::TILE_SIZE);

	glm::vec2 position;
	position.x = startPosition.x;
	position.y = startPosition.y + meterTileSize;

	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.position.Set(position.x, position.y);
	_body = world->getB2World()->CreateBody(&bodyDef);

	_width = width;

	b2EdgeShape chainShape; 
	b2Vec2 vs[2];

	vs[0].Set(-width / 2, 0);
	vs[1].Set(width / 2, 0);

	chainShape.Set(vs[0], vs[1]);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &chainShape;
	fixtureDef.friction = 0.005f;
	fixtureDef.filter.categoryBits = globals::PLATFORM_BIT;
	fixtureDef.filter.maskBits = globals::FOOT_FIXTURE_BIT | globals::DROP_BIT | globals::PROJECTILE_BIT;
	fixtureDef.userData = new userData(this, PLATFORM);
	_body->CreateFixture(&fixtureDef);
}

void platform::destroy(world* world) {
	world->deleteBody(_body);
	delete this;
}