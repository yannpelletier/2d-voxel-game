#pragma once
#include "Box2d/Box2d.h"
#include "userData.h"
#include "player.h"
#include "projectile.h"
#include "tileSlope.h"
#include "tileBatch.h"
#include "inventoryItem.h"
#include <iostream>


class contactListener: public b2ContactListener
{
public:
	void BeginContact(b2Contact* contact) {
		userData* bodyUserAData = nullptr;
		userData* bodyUserBData = nullptr;

		//Check the userData to see wether it's in the body or the fixture
		getUserDatas(contact, bodyUserAData, bodyUserBData);

		if (bodyUserAData == nullptr || bodyUserBData == nullptr) {
			return;
		}

		if (bodyUserAData->type == gameObjectType::FOOT_SENSOR || bodyUserAData->type == gameObjectType::PLAYER) {
			static_cast<player*>(bodyUserAData->dataUser)->startContact(bodyUserBData, bodyUserAData);
		}
		else if (bodyUserAData->type == gameObjectType::DROP_COLLECT_ZONE || bodyUserAData->type == gameObjectType::DROP) {
			static_cast<inventoryItem*>(bodyUserAData->dataUser)->startContact(bodyUserBData, bodyUserAData);
		}
		else if (bodyUserAData->type == gameObjectType::PROJECTILE) {
			static_cast<projectile*>(bodyUserAData->dataUser)->startContact(bodyUserBData, bodyUserAData, contact, world);
		}
		else if (bodyUserAData->type == gameObjectType::TILE) {
			static_cast<tileBatch*>(bodyUserAData->dataUser)->startContact(bodyUserBData, bodyUserAData, contact, world);
		}
		else if (bodyUserAData->type == gameObjectType::TILESLOPE) {
			static_cast<tileSlope*>(bodyUserAData->dataUser)->startContact(bodyUserBData, bodyUserAData, contact, world);
		}
		else if (bodyUserAData->type == gameObjectType::HELDITEM) {
			static_cast<heldItem*>(bodyUserAData->dataUser)->startContact(bodyUserBData, bodyUserAData);
		}

		if (bodyUserBData->type == gameObjectType::FOOT_SENSOR || bodyUserBData->type == gameObjectType::PLAYER) {
			static_cast<player*>(bodyUserBData->dataUser)->startContact(bodyUserAData, bodyUserBData);
		}
		else if (bodyUserBData->type == gameObjectType::DROP_COLLECT_ZONE || bodyUserBData->type == gameObjectType::DROP) {
			static_cast<inventoryItem*>(bodyUserBData->dataUser)->startContact(bodyUserAData, bodyUserBData);
		}
		else if (bodyUserBData->type == gameObjectType::PROJECTILE) {
			static_cast<projectile*>(bodyUserBData->dataUser)->startContact(bodyUserAData, bodyUserBData, contact, world);
		}
		else if (bodyUserBData->type == gameObjectType::TILE) {
			static_cast<tileBatch*>(bodyUserBData->dataUser)->startContact(bodyUserAData, bodyUserBData,contact, world);
		}
		else if (bodyUserBData->type == gameObjectType::TILESLOPE) {
			static_cast<tileSlope*>(bodyUserBData->dataUser)->startContact(bodyUserAData, bodyUserBData, contact, world);
		}
		else if (bodyUserBData->type == gameObjectType::HELDITEM) {
			static_cast<heldItem*>(bodyUserBData->dataUser)->startContact(bodyUserAData, bodyUserBData);
		}
	}




	void EndContact(b2Contact* contact) {
		userData* bodyUserAData = nullptr;
		userData* bodyUserBData = nullptr;

		getUserDatas(contact, bodyUserAData, bodyUserBData);

		if (bodyUserAData == nullptr || bodyUserBData == nullptr) {
			return;
		}

		if (bodyUserAData->type == gameObjectType::FOOT_SENSOR || bodyUserAData->type == gameObjectType::PLAYER) {
			static_cast<player*>(bodyUserAData->dataUser)->endContact(bodyUserBData, bodyUserAData);
		}
	}

	world* world;

};

