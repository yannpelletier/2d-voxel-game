#include "worldGenerator.h"
#include "top.h"
#include "filling.h"
#include "groundMaterial.h"
#include "spot.h"
#include "cave.h"


worldGenerator::worldGenerator()
{
	_definitionType = definitionType::WORLD_GENERATOR;
}


worldGenerator::~worldGenerator()
{
}

void worldGenerator::addGenerationAsset(generationAsset* asset) {
	_generationAssets.push_back(asset);
}

worldTemplate worldGenerator::getGeneratedWorldPortion(int width,int planetRadius, int seed)
{
	worldTemplate newTemplate(width, planetRadius + globals::SKY_HEIGHT_OVER_PLANET);
	PerlinNoise pn(seed);

	for (int i = 0; i < _generationAssets.size(); i++) {
		_generationAssets[i]->addGenerationLayer(pn, newTemplate, planetRadius);
	}

	return newTemplate;
}

int luaHandler::LUA_WORLD_GENERATOR_newWorldGenerator(lua_State *S) {
	std::string worldGeneratorName = lua_tostring(S, 1); //Get name of the table

	if (!_verifyLuaName(worldGeneratorName)) {
		return 0;
	}

	lua_getglobal(S, worldGeneratorName.c_str()); // "ground"
	lua_pushstring(S, "setProperties");		// "setProperties", "ground"
	lua_gettable(S, -2);					//	function setProperties, "ground"
	lua_pushvalue(S, -2);					//  "ground", function setProperties, "ground"
	lua_call(S, 1, 0);
	lua_pop(S, 1);

	worldGenerator* newWorldGenerator = new worldGenerator;

	newWorldGenerator->setNameID(worldGeneratorName);
	_definitions.emplace(std::make_pair(worldGeneratorName, newWorldGenerator));


	//Add animations to texture
	lua_getglobal(S, worldGeneratorName.c_str());		// tableName
	lua_pushstring(S, "generationAssets");			// "animations", tableName
	lua_gettable(S, -2);						// table animations, tableName
	if (lua_istable(S, -1)) {
		lua_pushnil(S);							// nil, table animations, tableName
		while (lua_next(S, -2) != 0) {			// table, key, table animations, tableName
			lua_pushnumber(S, 1);				// 1, table, key, table animations, tableName
			lua_gettable(S, -2);				// string, table, key, table animations, tableName
			std::string type = lua_tostring(_S, -1);

			generationAsset* newAsset = nullptr;

			if (type == "base") {
				newAsset = groundMaterial::LUA_handleCreation(S);
			}
			else if (type == "hills") {
				//TODO
			}
			else if (type == "filling") {
				newAsset = filling::LUA_handleCreation(S);
			}
			else if(type == "top"){
				newAsset = top::LUA_handleCreation(S);
			}
			else if (type == "caves") {
				newAsset = cave::LUA_handleCreation(S);
			}
			else if (type == "spots") {
				newAsset = spot::LUA_handleCreation(S);
			}
			else {
				gengine::fatalError("Invalid generation asset in world generator " + worldGeneratorName);
			}

			newWorldGenerator->addGenerationAsset(newAsset);

		}
	}
	lua_pop(S, 2);

	return 0;
}