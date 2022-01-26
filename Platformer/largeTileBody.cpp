#include "largeTileBody.h"
#include "world.h"


void largeTileBody::init(world* world, glm::vec2 startPosition, glm::vec2 dimensions) {
	float meterTileSize = globals::pixToMet(globals::TILE_SIZE);

	glm::vec2 position;
	position.x = startPosition.x;
	position.y = startPosition.y;

	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.position.Set(position.x, position.y);
	_body = world->getB2World()->CreateBody(&bodyDef);

	_dimensions = dimensions;

	b2ChainShape chainShape;
	b2Vec2 vs[4];

	vs[0].Set(dimensions.x / 2, dimensions.y / 2);
	vs[1].Set(dimensions.x / 2, -dimensions.y / 2);
	vs[2].Set(-dimensions.x / 2, -dimensions.y / 2);
	vs[3].Set(-dimensions.x / 2, dimensions.y / 2);

	chainShape.CreateLoop(vs, 4);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &chainShape;
	fixtureDef.friction = 0.005f;
	fixtureDef.filter.categoryBits = globals::SOLID_BODY_BIT;
	fixtureDef.filter.maskBits = globals::PLAYER_BIT | globals::NPC_BIT | globals::DROP_BIT | globals::PROJECTILE_BIT;
	fixtureDef.userData = new userData(this, SOLID_BODY);
	_body->CreateFixture(&fixtureDef);
}

void largeTileBody::destroy(world* world) {
	world->deleteBody(_body);
	delete this;
}

int luaHandler::LUA_SOLID_BODY_setSolid(lua_State *S) {
	largeTileBody* p = *((largeTileBody**)lua_touserdata(S, 1));

	_world->setChanged();

	p->getBody()->SetActive(lua_toboolean(S, 2));

	return 0;
}

void luaHandler::_registerSolidBodyFunction() {
	luaL_Reg solidBodylib_m[] = {
		{ "setSolid", LUA_SOLID_BODY_setSolid },
		{ NULL, NULL }
	};

	luaL_newmetatable(_S, "solid_body_m");

	lua_pushstring(_S, "__index");
	lua_pushvalue(_S, -2);  /* pushes the metatable*/
	lua_settable(_S, -3);  /* metatable.__index = metatable */

	luaL_setfuncs(_S, solidBodylib_m, 0);
}