#pragma once
#include "Box2d/Box2d.h"
#include <vector>

//Allows us to create a body outside of a Step() in the Box2d world
class bodyTemplate
{
	friend class world;

	b2BodyDef _bodyDefinition;
	std::vector<b2FixtureDef> _fixtures;
	std::vector<b2Shape*> _shapes;

	//A pointer to the pointer to the body of the object that created the body
	b2Body** _objectsBody = nullptr;
	std::vector<b2Fixture*>* _objectsFixtures = nullptr;

	b2Body* _createdBody = nullptr;
	//The impulse that has to be given to the body when it gets created
	b2Vec2 _beginImpulse;
};