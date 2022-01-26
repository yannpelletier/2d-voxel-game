#include "tileSpike.h"
#include "tile.h"
#include "world.h"

float tileSpike::getLineXCoordinate(float& y, float slope, float b) {
	return (y - b) / slope;
}

float tileSpike::getLineYCoordinate(float& x, float slope, float b) {
	return slope * x + b;
}

void tileSpike::init(world* world, glm::ivec2 tilePos, glm::vec2 leftStartingPoint, glm::vec2 rightStartingPoint, tile* meshedTile){
	float meterTileSize = globals::pixToMet(globals::TILE_SIZE);

	b2Vec2 position;
	position.x = tilePos.x * meterTileSize + meterTileSize / 2;
	position.y = tilePos.y * meterTileSize + meterTileSize / 2;

	b2BodyDef bodyDef;
	bodyDef.position = position;
	bodyDef.type = b2_staticBody;
	_body = world->getB2World()->CreateBody(&bodyDef);
	b2ChainShape chainShape;

	b2Vec2 vs[4];
	vs[0].x = tilePos.x * meterTileSize;
	vs[0].y = tilePos.y * meterTileSize + meterTileSize;
	vs[1].x = tilePos.x * meterTileSize + meterTileSize;
	vs[1].y = tilePos.y * meterTileSize + meterTileSize;

	//Right slope
	float slope = (rightStartingPoint.y - vs[1].y) / (rightStartingPoint.x - vs[1].x);
	float b = vs[1].y - (vs[1].x * slope);

	vs[2].y = tilePos.y * globals::pixToMet(globals::TILE_SIZE);
	vs[2].x = getLineXCoordinate(vs[2].y, slope, b);

	//Left slope
	slope = (leftStartingPoint.y - vs[0].y) / (leftStartingPoint.x - vs[0].x);
	b = vs[0].y - (vs[0].x * slope);

	vs[3].y = tilePos.y * globals::pixToMet(globals::TILE_SIZE);
	vs[3].x = getLineXCoordinate(vs[2].y, slope, b);

	vs[0].x -= position.x;
	vs[0].y -= position.y;
	vs[1].x -= position.x;
	vs[1].y -= position.y;
	vs[2].x -= position.x;
	vs[2].y -= position.y;
	vs[3].x -= position.x;
	vs[3].y -= position.y;

	chainShape.CreateLoop(vs, 4);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &chainShape;
	fixtureDef.filter.categoryBits = globals::TILE_BIT;
	fixtureDef.filter.maskBits = globals::PLAYER_BIT | globals::NPC_BIT | globals::DROP_BIT | globals::PROJECTILE_BIT;
	fixtureDef.userData = new userData(this, TILESLOPE);
	_body->CreateFixture(&fixtureDef);

	_vertices.push_back(vs[0] + b2Vec2(position.x, position.y));
	_vertices.push_back(vs[1] + b2Vec2(position.x, position.y));
	_vertices.push_back(vs[2] + b2Vec2(position.x, position.y));
	_vertices.push_back(vs[3] + b2Vec2(position.x, position.y));

	_occupyingTile = meshedTile;

	_occupyingTile->occupyBatch(this);
}

void tileSpike::destroy(world* world) {
	_occupyingTile->occupyBatch(nullptr);
	world->deleteBody(_body);
	delete this;
}


void tileSpike::debugDraw(gengine::debugRenderer* debugRenderer) {
	if (!_vertices.empty()) {
		for (int i = 0; i < _vertices.size() - 1; i++) {
			debugRenderer->drawLine(glm::vec2(_vertices[i].x, _vertices[i].y), glm::vec2(_vertices[i + 1].x, _vertices[i + 1].y), gengine::color(255, 255, 255, 255));
		}
		debugRenderer->drawLine(glm::vec2(_vertices[_vertices.size() - 1].x, _vertices[_vertices.size() - 1].y), glm::vec2(_vertices[0].x, _vertices[0].y), gengine::color(255, 255, 255, 255));
	}
}

//Used to find the closest tile to a point
bool tileSpike::_validTile(int gridX, int gridY, world* world) {
	if (world->getTile(gridX, gridY) != nullptr) {
		if (world->getTile(gridX, gridY)->isSolid() && world->getTile(gridX, gridY)->hasFront()) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}

void tileSpike::startContact(userData* collidedWith, userData* thisFixture, b2Contact* contact, world* world) {
	/*	b2WorldManifold worldManifold;
	contact->GetWorldManifold(&worldManifold);

	b2Vec2 contacts = worldManifold.points[0];
	contacts.x -= globals::pixToMet(globals::TILE_SIZE);
	contacts.y -= globals::pixToMet(globals::TILE_SIZE);
	b2Vec2 bodyPos = _body->GetPosition();
	b2Vec2 contactedTile;
	contactedTile.x = round(contacts.x / globals::pixToMet(globals::TILE_SIZE));
	contactedTile.y = round(contacts.y / globals::pixToMet(globals::TILE_SIZE));

	//Finds the closest tile from the collision point
	b2Vec2 xAmbiguity(0,0); //contains the x coordinates of the 2 possible collided x tile positions
	if (contacts.x >= floor(contacts.x) + 0.5) {
	xAmbiguity.x = contactedTile.x;
	xAmbiguity.y = contactedTile.x + 1;
	}
	else if (contacts.x <= floor(contacts.x) + 0.5) {
	xAmbiguity.x = contactedTile.x;
	xAmbiguity.y = contactedTile.x + 1;
	}
	else {
	xAmbiguity.x = contactedTile.x;
	xAmbiguity.y = contactedTile.x;
	}

	b2Vec2 yAmbiguity(0,0); //contains the y coordinates of the 2 possible collided y tile positions
	if (contacts.y >= floor(contacts.y) + 0.5) {
	yAmbiguity.x = contactedTile.y;
	yAmbiguity.y = contactedTile.y + 1;
	}
	else if (contacts.y <= floor(contacts.y) + 0.5) {
	yAmbiguity.x = contactedTile.y;
	yAmbiguity.y = contactedTile.y + 1;
	}
	else {
	yAmbiguity.x = contactedTile.y;
	yAmbiguity.y = contactedTile.y;
	}

	for (int y = yAmbiguity.x; y <= yAmbiguity.y; y++) {
	for (int x = xAmbiguity.x; x <= xAmbiguity.y; x++) {
	if (_validTile(x, y, world)) {
	contactedTile.x = x;
	contactedTile.y = y;
	break;
	}
	}
	}*/
}

void tileSpike::endContact(userData* collidedWith, userData* thisFixture) {

}

void tileSpike::LUA_onHit() {

}