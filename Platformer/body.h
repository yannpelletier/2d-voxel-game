#pragma once
#include <Box2D/Box2D.h>
#include <glm/glm.hpp>
#include <Gengine/spriteBatch.h>
#include <Gengine/GLTexture.h>
#include <Gengine/debugRenderer.h>
#include "userData.h"

enum bodyType {
	uninitialized,
	capsule,
	box,
	circle,
	chainShape
};

class world;

class body
{
public:
	//Takes into consideration that the first fixture is the main fixture
	void createBody(world* world, b2BodyDef bodyDef, b2FixtureDef fixtureDef, b2Vec2 beginImpulse);
	void createBody(world* world, b2BodyDef bodyDef, std::vector<b2FixtureDef> fixtureDefs, b2Vec2 beginImpulse);
	void destroy(world* world);

	b2PolygonShape makeBox(const glm::vec2& dimensions);
	b2ChainShape makeChainShape();
	b2CircleShape makeCircle(float radius);
	b2PolygonShape makeCapsule(const glm::vec2& dimensions, const glm::vec2& cornerDifference);

	//Setters
	void applyLinearImpulse(b2Vec2& impulse, b2Vec2& center);
	void applyForceToCenter(b2Vec2& force);
	void setLinearVelocity(b2Vec2& velocity);
	void setSensor(int fixtureIndex, bool value);
	void setFixedRotation(bool value);
	void setUserData(int fixtureIndex, userData* userData);
	void setPosition(glm::vec2& newPosition);
	void setAngleDeg(float angle);
	void setAngleRad(float angle);

	//Getters
	glm::vec2 getPosition();
	glm::vec2 getDimensions();
	b2Body* getBody();
	float getAngleDeg();
	float getAngleRad();
	b2Vec2 getLinearVelocity();
	b2ContactEdge* getContactList();

	virtual void debugDraw(gengine::debugRenderer* debugRenderer);

	b2Fixture* getFixture(int index) { return _fixtures[index]; }

	bool hasBody();

protected:
	bodyType _bodyType = uninitialized;
	glm::vec2 _dimensions;
	glm::vec2 _position;
	b2Body* _body = nullptr;
	std::vector<b2Fixture*> _fixtures;
};

