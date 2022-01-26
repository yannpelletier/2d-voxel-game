#include "userData.h"
#include "Box2d/Box2d.h"


void getUserDatas(b2Contact* contact, userData*& userDataA, userData*& userDataB) {
	//check if fixture A was an entity
	void* userDataCheck;
	userData* bodyUserAData = nullptr;
	userData* bodyUserBData = nullptr;

	//Check the userData to see wether it's in the body or the fixture
	userDataCheck = contact->GetFixtureA()->GetUserData();
	if (userDataCheck) {
		bodyUserAData = (userData*)(contact->GetFixtureA()->GetUserData());
	}
	else {
		userDataCheck = contact->GetFixtureA()->GetBody()->GetUserData();
		if (userDataCheck) {
			bodyUserAData = (userData*)(contact->GetFixtureA()->GetBody()->GetUserData());
		}
	}

	userDataCheck = contact->GetFixtureB()->GetUserData();
	if (userDataCheck) {
		bodyUserBData = (userData*)(contact->GetFixtureB()->GetUserData());
	}
	else {
		userDataCheck = contact->GetFixtureB()->GetBody()->GetUserData();
		if (userDataCheck) {
			bodyUserBData = (userData*)(contact->GetFixtureB()->GetBody()->GetUserData());
		}
	}

	userDataA = bodyUserAData;
	userDataB = bodyUserBData;
}