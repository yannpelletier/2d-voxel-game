#include "spot.h"

spot::spot(bool front, std::string materialName, std::string exclusivity, double frequencyRatio, double sizeRatio) {
	_materialName = materialName;
	_exclusivity = exclusivity;
	_frequencyRatio = frequencyRatio;
	_sizeRatio = sizeRatio;
	_front = front;
	_type = generationAssetType::SPOT;
}

void spot::addGenerationLayer(PerlinNoise& pn, worldTemplate& toModify, int planetRadius)
{
	for (unsigned int y = 0; y < toModify.getHeight(); y++) {
		for (unsigned int x = 0; x < toModify.getWidth(); x++) {
			double spotX = (double)x / _sizeRatio;
			double spotY = (double)y / _sizeRatio;
			double n = pn.noise(spotX, spotY, 0.0);


			if (n * 100 <= _frequencyRatio) {
				if (_exclusivity == "none") {
					if (_front) {
						toModify.setFrontMaterialMod(x, y, (materialMod*)luaHandler::getDefinition(_materialName));
					}
					else {
						toModify.setBackMaterialMod(x, y, (materialMod*)luaHandler::getDefinition(_materialName));
					}
				}
				else {
					if (_front) {
						if (toModify.getFrontMaterial(x,y) != nullptr) {
							if (toModify.getFrontMaterial(x, y)->getNameID() == _exclusivity) {
								toModify.setFrontMaterialMod(x, y, (materialMod*)luaHandler::getDefinition(_materialName));
							}
						}
					}
					else {
						if (toModify.getBackMaterial(x,y) != nullptr) {
							if (toModify.getBackMaterial(x, y)->getNameID() == _exclusivity) {
								toModify.setBackMaterialMod(x, y, (materialMod*)luaHandler::getDefinition(_materialName));
							}
						}
					}
				}
			}
		}
	}
}

generationAsset* spot::LUA_handleCreation(lua_State* S) {
	lua_pushnumber(S, 2);
	lua_gettable(S, -3);
	bool front = lua_toboolean(S, -1);
	lua_pop(S, 1);

	lua_pushnumber(S, 3);
	lua_gettable(S, -3);
	std::string materialName = lua_tostring(S, -1);
	lua_pop(S, 1);

	lua_pushnumber(S, 4);
	lua_gettable(S, -3);
	std::string exclusivity = lua_tostring(S, -1);
	lua_pop(S, 1);

	lua_pushnumber(S, 5);
	lua_gettable(S, -3);
	float frequencyRatio = lua_tonumber(S, -1);
	lua_pop(S, 1);

	lua_pushnumber(S, 6);
	lua_gettable(S, -3);
	float sizeRatio = lua_tonumber(S, -1);
	lua_pop(S, 1);

	generationAsset* newSpot = new spot(front, materialName,exclusivity,frequencyRatio,sizeRatio);
	lua_pop(S, 2);
	return newSpot;
}
