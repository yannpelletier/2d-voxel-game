#include "tileSlope.h"
#include "tile.h"
#include "world.h"

float getLineXCoordinate(float& y, float slope, float b) {
	return (y - b) / slope;
}

float getLineYCoordinate(float& x, float slope, float b) {
	return slope * x + b;
}

void tileSlope::init(world* world, glm::ivec2 tilePos, glm::vec2 startingPoint, glm::vec2 endPoint, tile* meshedTile) {
	float meterTileSize = globals::pixToMet(globals::TILE_SIZE);

	b2Vec2 position;
	position.x = tilePos.x * meterTileSize + meterTileSize / 2;
	position.y = tilePos.y * meterTileSize + meterTileSize / 2;

	b2BodyDef bodyDef;
	bodyDef.position = position;
	bodyDef.type = b2_staticBody;
	_body = world->getB2World()->CreateBody(&bodyDef);
	b2ChainShape chainShape;

	int vrtxCount = 0;
	b2Vec2 vs[5];

	float slope = (endPoint.y - startingPoint.y) / (endPoint.x - startingPoint.x);
	float b = startingPoint.y - (startingPoint.x * slope);

	if (endPoint.x > startingPoint.x) {	//Slope going top right

		if (tilePos.x * meterTileSize - meterTileSize == startingPoint.x) {  //Is the first tile in the slope
			vrtxCount = 4;

			vs[0].Set(meterTileSize / 2, -meterTileSize / 2); //Bottom right corner
			vs[1].Set(startingPoint.x - position.x, startingPoint.y - position.y);//Bottom left corner

			vs[2].y = position.y + meterTileSize / 2;								//Top intersection point
			vs[2].x = getLineXCoordinate(vs[2].y, slope, b);
			vs[2].x -= position.x;
			vs[2].y -= position.y;

			vs[3].x = position.x + meterTileSize / 2;		//Left intersection point
			vs[3].y = getLineYCoordinate(vs[3].x, slope, b);
			vs[3].x -= position.x;
			vs[3].y -= position.y;

			_vertices.push_back(vs[0] + b2Vec2(position.x, position.y));
			_vertices.push_back(vs[1] + b2Vec2(position.x, position.y));
			_vertices.push_back(vs[2] + b2Vec2(position.x, position.y));
			_vertices.push_back(vs[3] + b2Vec2(position.x, position.y));
		}
		else if (tilePos.x * meterTileSize + meterTileSize == endPoint.x) {	//Is the last tile in the slope
			vrtxCount = 4;

			vs[0].Set(meterTileSize / 2, -meterTileSize / 2); //Bottom right corner
			vs[1].Set(-meterTileSize / 2, -meterTileSize / 2);//Bottom left corner

			vs[2].x = position.x - meterTileSize / 2;		//Left intersection point
			vs[2].y = getLineYCoordinate(vs[2].x, slope, b);
			vs[2].x -= position.x;
			vs[2].y -= position.y;

			vs[3].Set(endPoint.x - position.x, endPoint.y - position.y);

			_vertices.push_back(vs[0] + b2Vec2(position.x, position.y));
			_vertices.push_back(vs[1] + b2Vec2(position.x, position.y));
			_vertices.push_back(vs[2] + b2Vec2(position.x, position.y));
			_vertices.push_back(vs[3] + b2Vec2(position.x, position.y));
		}
		else {	//Is in the middle of the slope
			vrtxCount = 5;

			vs[0].Set(meterTileSize / 2, -meterTileSize / 2);						//Bottom right corner
			vs[1].Set(-meterTileSize / 2, -meterTileSize / 2);						//Bottom left corner

			vs[2].x = position.x - meterTileSize / 2;								//Left intersection point
			vs[2].y = getLineYCoordinate(vs[2].x, slope, b);
			vs[2].x -= position.x;
			vs[2].y -= position.y;

			vs[3].y = position.y + meterTileSize / 2;								//Top intersection point
			vs[3].x = getLineXCoordinate(vs[3].y, slope, b);
			vs[3].x -= position.x;
			vs[3].y -= position.y;

			vs[4].x = position.x + meterTileSize / 2;								//Top intersection point
			vs[4].y = getLineYCoordinate(vs[4].x, slope, b);
			vs[4].x -= position.x;
			vs[4].y -= position.y;

			_vertices.push_back(vs[0] + b2Vec2(position.x, position.y));
			_vertices.push_back(vs[1] + b2Vec2(position.x, position.y));
			_vertices.push_back(vs[2] + b2Vec2(position.x, position.y));
			_vertices.push_back(vs[3] + b2Vec2(position.x, position.y));
			_vertices.push_back(vs[4] + b2Vec2(position.x, position.y));
		}

	}
	else {	//Slope going top left
		if (tilePos.x * meterTileSize + 2 * meterTileSize == startingPoint.x) {  //Is the first tile in the slope
			vrtxCount = 3;

			vs[0].Set(startingPoint.x - position.x, startingPoint.y - position.y);//Bottom left corner

			vs[1].Set(-meterTileSize / 2, -meterTileSize / 2); //Bottom right corner

			vs[2].x = position.x - meterTileSize / 2;		//Left intersection point
			vs[2].y = getLineYCoordinate(vs[2].x, slope, b);
			vs[2].x -= position.x;
			vs[2].y -= position.y;


			_vertices.push_back(vs[0] + b2Vec2(position.x, position.y));
			_vertices.push_back(vs[1] + b2Vec2(position.x, position.y));
			_vertices.push_back(vs[2] + b2Vec2(position.x, position.y));
		}
		else if (tilePos.x * meterTileSize == endPoint.x) {	//Is the last tile in the slope
			vrtxCount = 4;

			vs[0].Set(meterTileSize / 2, -meterTileSize / 2); //Bottom right corner
			vs[1].Set(-meterTileSize / 2, -meterTileSize / 2);//Bottom left corner

			vs[2].Set(endPoint.x - position.x, endPoint.y - position.y);

			vs[3].x = position.x + meterTileSize / 2;		//Left intersection point
			vs[3].y = getLineYCoordinate(vs[3].x, slope, b);
			vs[3].x -= position.x;
			vs[3].y -= position.y;


			_vertices.push_back(vs[0] + b2Vec2(position.x, position.y));
			_vertices.push_back(vs[1] + b2Vec2(position.x, position.y));
			_vertices.push_back(vs[2] + b2Vec2(position.x, position.y));
			_vertices.push_back(vs[3] + b2Vec2(position.x, position.y));
		}
		else {	//Is in the middle of the slope
			vrtxCount = 5;

			vs[0].Set(meterTileSize / 2, -meterTileSize / 2);						//Bottom right corner
			vs[1].Set(-meterTileSize / 2, -meterTileSize / 2);						//Bottom left corner

			vs[2].x = position.x - meterTileSize / 2;								//Left intersection point
			vs[2].y = getLineYCoordinate(vs[2].x, slope, b);
			vs[2].x -= position.x;
			vs[2].y -= position.y;

			vs[3].y = position.y + meterTileSize / 2;								//Top intersection point
			vs[3].x = getLineXCoordinate(vs[3].y, slope, b);
			vs[3].x -= position.x;
			vs[3].y -= position.y;

			vs[4].x = position.x + meterTileSize / 2;								//Left intersection point
			vs[4].y = getLineYCoordinate(vs[4].x, slope, b);
			vs[4].x -= position.x;
			vs[4].y -= position.y;

			_vertices.push_back(vs[0] + b2Vec2(position.x, position.y));
			_vertices.push_back(vs[1] + b2Vec2(position.x, position.y));
			_vertices.push_back(vs[2] + b2Vec2(position.x, position.y));
			_vertices.push_back(vs[3] + b2Vec2(position.x, position.y));
			_vertices.push_back(vs[4] + b2Vec2(position.x, position.y));
		}
	}

	chainShape.CreateLoop(vs, vrtxCount);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &chainShape;
	fixtureDef.filter.categoryBits = globals::TILE_BIT;
	fixtureDef.filter.maskBits = globals::PLAYER_BIT | globals::NPC_BIT | globals::DROP_BIT | globals::PROJECTILE_BIT;
	fixtureDef.userData = new userData(this, TILESLOPE);
	_body->CreateFixture(&fixtureDef);

	_occupyingTile = meshedTile;

	_occupyingTile->occupyBatch(this);
}

void tileSlope::destroy(world* world) {
	_occupyingTile->occupyBatch(nullptr);
	world->deleteBody(_body);
	delete this;
}


void tileSlope::debugDraw(gengine::debugRenderer* debugRenderer) {
	if (!_vertices.empty()) {
		for (int i = 0; i < _vertices.size() - 1; i++) {
			debugRenderer->drawLine(glm::vec2(_vertices[i].x, _vertices[i].y), glm::vec2(_vertices[i + 1].x, _vertices[i + 1].y), gengine::color(255, 255, 255, 255));
		}
		debugRenderer->drawLine(glm::vec2(_vertices[_vertices.size() - 1].x, _vertices[_vertices.size() - 1].y), glm::vec2(_vertices[0].x, _vertices[0].y), gengine::color(255, 255, 255, 255));
	}
}

//Used to find the closest tile to a point
bool tileSlope::_validTile(int gridX, int gridY, world* world) {
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

void tileSlope::startContact(userData* collidedWith, userData* thisFixture, b2Contact* contact, world* world) {
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

void tileSlope::endContact(userData* collidedWith, userData* thisFixture) {

}

void tileSlope::LUA_onHit() {

}