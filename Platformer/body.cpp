#include "body.h"
#include "Box2d/Box2d.h"
#include "Gengine/errors.h"
#include "world.h"

void body::createBody(world* world, b2BodyDef bodyDef, b2FixtureDef fixtureDef, b2Vec2 beginImpulse) {
	if (_body == nullptr) {
		if (world == nullptr) {
			gengine::fatalError("Cannot create a body inside an invalid world");
		}
		else if (_bodyType == bodyType::uninitialized) {
			gengine::fatalError("Body must be given a type");
		}

		world->createBody(bodyDef, fixtureDef, &_body, &_fixtures, beginImpulse);
	}
}

void body::createBody(world* world, b2BodyDef bodyDef, std::vector<b2FixtureDef> fixtureDefs, b2Vec2 beginImpulse) {
	if (_body == nullptr) {
		if (world == nullptr) {
			gengine::fatalError("Cannot create a body inside an invalid world");
		}
		else if (fixtureDefs.empty()) {
			gengine::fatalError("Cannot create a body without fixtures");
		}
		else if (_bodyType == bodyType::uninitialized) {
			gengine::fatalError("Body must be given a type");
		}

		world->createBody(bodyDef, fixtureDefs, &_body, &_fixtures, beginImpulse);
	}
}

void body::destroy(world* world) {
	world->deleteBody(_body);
	_body = nullptr;
	delete this;
}

b2PolygonShape body::makeBox(const glm::vec2& dimensions) {
	b2PolygonShape boxshape;
	boxshape.SetAsBox(dimensions.x / 2, dimensions.y / 2);

	_bodyType = box;
	_dimensions = dimensions;

	return boxshape;
}

b2ChainShape body::makeChainShape() {
	b2ChainShape chainShapes;

	_bodyType = bodyType::chainShape;

	return chainShapes;
}

b2CircleShape body::makeCircle(float radius) {
	b2CircleShape circleShape;
	circleShape.m_radius = radius;

	_bodyType = circle;
	_dimensions = glm::vec2(radius, 0);

	return circleShape;
}

b2PolygonShape body::makeCapsule(const glm::vec2& dimensions, const glm::vec2& cornerDifference) {
	b2PolygonShape chainBox;
	b2Vec2 vs[8];

	/*
	 _______
	/		\
	|		|
	|		|
	\_______/
	*/
	vs[0].Set((-dimensions.x / 2), (dimensions.y / 2) - cornerDifference.y);
	vs[1].Set((-dimensions.x / 2) + cornerDifference.x, (dimensions.y / 2));
	vs[2].Set((dimensions.x / 2) - cornerDifference.x, (dimensions.y / 2));
	vs[3].Set((dimensions.x / 2), (dimensions.y / 2) - cornerDifference.y);
	vs[4].Set((dimensions.x / 2), (-dimensions.y / 2) + cornerDifference.y);
	vs[5].Set((dimensions.x / 2) - cornerDifference.x, (-dimensions.y / 2));
	vs[6].Set((-dimensions.x / 2) + cornerDifference.x, (-dimensions.y / 2));
	vs[7].Set((-dimensions.x / 2), (-dimensions.y / 2) + cornerDifference.y);

	chainBox.Set(vs, 8);

	_bodyType = capsule;
	_dimensions = dimensions;

	return chainBox;
}

//Setters
void body::applyLinearImpulse(b2Vec2& impulse, b2Vec2& center) {
	_body->ApplyLinearImpulse(impulse, center, true);
}

void body::applyForceToCenter(b2Vec2& force) {
	_body->ApplyForceToCenter(force, true);
}

void body::setLinearVelocity(b2Vec2& velocity) {
	_body->SetLinearVelocity(velocity);
}

void body::setSensor(int fixtureIndex, bool value) {
	_fixtures[fixtureIndex]->SetSensor(value);
}
void body::setFixedRotation(bool value) {
	_body->SetFixedRotation(value);
}
void body::setUserData(int fixtureIndex, userData* userData) {
	_fixtures[fixtureIndex]->SetUserData(userData);
}
void body::setPosition(glm::vec2& newPosition) {
	_body->SetTransform(b2Vec2(newPosition.x, newPosition.y), _body->GetAngle());
}
void body::setAngleDeg(float angle) {
	float angleToSet = angle;
	angleToSet *= 0.0174533;

	_body->SetTransform(b2Vec2(_body->GetPosition().x, _body->GetPosition().y), angleToSet);
}
void body::setAngleRad(float angle) {
	_body->SetTransform(b2Vec2(_body->GetPosition().x, _body->GetPosition().y), angle);
}

//Getters
glm::vec2 body::getPosition() {
	return glm::vec2(_body->GetPosition().x, _body->GetPosition().y);
}
glm::vec2 body::getDimensions() {
	return _dimensions;
}
b2Body* body::getBody() {
	return _body;
}
float body::getAngleDeg() {
	return _body->GetAngle() * 57.2958;
}
float body::getAngleRad() {
	return _body->GetAngle();
}
b2Vec2 body::getLinearVelocity() {
	return _body->GetLinearVelocity();
}

b2ContactEdge* body::getContactList() {
	return _body->GetContactList();
}

bool body::hasBody() {
	if (_body == nullptr) {
		return false;
	}
	else {
		return true;
	}
}

void body::debugDraw(gengine::debugRenderer* debugRenderer) {
	gengine::color debugColor = gengine::color(255, 255, 255, 255);

	glm::vec4 destRect;

	switch (_bodyType) {
	case circle:
		debugRenderer->drawCircle(glm::vec2(_body->GetPosition().x, _body->GetPosition().y), debugColor, _dimensions.x);

		break;
	case box:
		destRect.x = _body->GetPosition().x - _dimensions.x / 2;
		destRect.y = _body->GetPosition().y - _dimensions.y / 2;
		destRect.z = _dimensions.x;
		destRect.w = _dimensions.y;

		debugRenderer->drawBox(destRect, debugColor, _body->GetAngle());

		break;
	case capsule:
		b2PolygonShape* shape = static_cast<b2PolygonShape*>(_fixtures[0]->GetShape());

			glm::vec2 cornerDifference;
			cornerDifference.x = shape->GetVertex(3).x - shape->GetVertex(4).x;
			cornerDifference.y = shape->GetVertex(3).y - shape->GetVertex(4).y;

			glm::vec2 position(_body->GetPosition().x, _body->GetPosition().y);

			b2Vec2 vs[8];
			vs[0].Set((-_dimensions.x / 2), (_dimensions.y / 2) - cornerDifference.y);
			vs[1].Set((-_dimensions.x / 2) + cornerDifference.x, (_dimensions.y / 2));
			vs[2].Set((_dimensions.x / 2) - cornerDifference.x, (_dimensions.y / 2));
			vs[3].Set((_dimensions.x / 2), (_dimensions.y / 2) - cornerDifference.y);
			vs[4].Set((_dimensions.x / 2), (-_dimensions.y / 2) + cornerDifference.y);
			vs[5].Set((_dimensions.x / 2) - cornerDifference.x, (-_dimensions.y / 2));
			vs[6].Set((-_dimensions.x / 2) + cornerDifference.x, (-_dimensions.y / 2));
			vs[7].Set((-_dimensions.x / 2), (-_dimensions.y / 2) + cornerDifference.y);

			for (int i = 0; i < 7; i++) {
				debugRenderer->drawLine(glm::vec2(vs[i].x + position.x, vs[i].y + position.y), glm::vec2(vs[i + 1].x + position.x, vs[i + 1].y + position.y), debugColor);
			}
			debugRenderer->drawLine(glm::vec2(vs[7].x + position.x, vs[7].y + position.y), glm::vec2(vs[0].x + position.x, vs[0].y + position.y), debugColor);

		break;
	}
}
