#pragma once
#include <Gengine/vertex.h>
#include <string>
#include <Functional>
#include "Gengine/spriteSheet.h"
#include "Gengine/spriteBatch.h"
#include <glm/glm.hpp>
#include "luaHandler.h"

class world;

enum definitionType { MATERIAL, MATERIAL_MOD, USABLE_ITEM, PLACABLE_ITEM, PARTICLE_EMITTER, PARTICLE, WORLD_GENERATOR, ERR };

//Will hold all the attributes of a thing in the game that can then be reused and transfered 
class definition
{
	friend class luaHandler;
public:
	void setNameID(std::string ID);
	std::string getNameID() { return _nameID; }
	definitionType getDefinitionType() { return _definitionType; }

protected:
	std::string _nameID;
	definitionType _definitionType;
};