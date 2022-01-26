#pragma once

class b2Contact;

enum gameObjectType {
	PLAYER,
	NPC,
	FOOT_SENSOR,
	TILE,
	SOLID_BODY,
	PLATFORM,
	DROP,
	DROP_COLLECT_ZONE,
	PROJECTILE, 
	HELDITEM,
	TILESLOPE
};

//This is information that you can give to a body to trace back what object owns it

struct userData {
	userData(void* user, gameObjectType objectType) {
			dataUser = user;
			type = objectType;
	}

	gameObjectType type;
	void* dataUser;
};

extern void getUserDatas(b2Contact* contact, userData*& userDataA, userData*& userDataB);