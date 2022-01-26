#include "tileBatch.h"
#include "world.h"

tileBatch::~tileBatch() {
}

void tileBatch::init(world* world, glm::ivec2 topLeft, glm::ivec2 bottomRight, std::vector<tile*> meshedTiles) {
	float meterTileSize = globals::pixToMet(globals::TILE_SIZE);

	glm::vec2 position;
	position.x = (topLeft.x * meterTileSize + (float)((bottomRight.x - topLeft.x + 1) * meterTileSize / 2));
	position.y = (bottomRight.y * meterTileSize + (float)((topLeft.y - bottomRight.y + 1)  * meterTileSize / 2));

	_dimensions.x = (float)(bottomRight.x + 1 - topLeft.x) * meterTileSize;
	_dimensions.y = (float)(topLeft.y + 1 - bottomRight.y) * meterTileSize;

	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.position.Set(position.x, position.y);
	_body = world->getB2World()->CreateBody(&bodyDef);

	b2ChainShape chainShape;
	b2Vec2 vs[4];

	vs[0].Set(_dimensions.x / 2, _dimensions.y / 2);
	vs[1].Set(_dimensions.x / 2, -_dimensions.y / 2);
	vs[2].Set(-_dimensions.x / 2, -_dimensions.y / 2);
	vs[3].Set(-_dimensions.x / 2, _dimensions.y / 2);

	chainShape.CreateLoop(vs, 4);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &chainShape;
	fixtureDef.friction = 0.025f;
	fixtureDef.density = 0.3;
	fixtureDef.filter.categoryBits = globals::TILE_BIT;
	fixtureDef.filter.maskBits = globals::PLAYER_BIT | globals::NPC_BIT | globals::DROP_BIT | globals::PROJECTILE_BIT;
	fixtureDef.userData = new userData(this, TILE);
	_body->CreateFixture(&fixtureDef);

	_occupyingTiles = meshedTiles;

	for (tile* it : _occupyingTiles) {
		it->occupyBatch(this);
	}
}

void tileBatch::destroy(world* world) {
	for (int i = 0; i < _occupyingTiles.size(); i++) {
		_occupyingTiles[i]->occupyBatch(nullptr);
	}
	world->deleteBody(_body);
	delete this;
}


void tileBatch::debugDraw(gengine::debugRenderer* debugRenderer) {
	glm::vec4 destRect;
	destRect.x = _body->GetPosition().x - _dimensions.x / 2;
	destRect.y = _body->GetPosition().y - _dimensions.y / 2;
	destRect.z = _dimensions.x;
	destRect.w = _dimensions.y;
	debugRenderer->drawBox(destRect, gengine::color(255, 255, 0, 255), 0);
}

//Used to find the closest tile to a point
bool tileBatch::_validTile(int gridX, int gridY, world* world) {
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

void tileBatch::startContact(userData* collidedWith, userData* thisFixture, b2Contact* contact, world* world) {
}

void tileBatch::endContact(userData* collidedWith, userData* thisFixture) {

}

glm::ivec2 tileBatch::getCollisionedTile(userData* collidedWith, userData* thisFixture, b2Contact* contact, world* world) {
	b2WorldManifold worldManifold;
	contact->GetWorldManifold(&worldManifold);

	b2Vec2 contacts = worldManifold.points[0];
	contacts.x -= globals::pixToMet(globals::TILE_SIZE);
	contacts.y -= globals::pixToMet(globals::TILE_SIZE);
	b2Vec2 bodyPos = _body->GetPosition();
	b2Vec2 contactedTile;
	contactedTile.x = round(contacts.x / globals::pixToMet(globals::TILE_SIZE));
	contactedTile.y = round(contacts.y / globals::pixToMet(globals::TILE_SIZE));

	//Finds the closest tile from the collision point
	b2Vec2 xAmbiguity(0, 0); //contains the x coordinates of the 2 possible collided x tile positions
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

	b2Vec2 yAmbiguity(0, 0); //contains the y coordinates of the 2 possible collided y tile positions
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
	}

	return glm::ivec2(contactedTile.x, contactedTile.y);
}

void tileBatch::LUA_onHit() {

}