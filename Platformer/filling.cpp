#include "filling.h"

filling::filling(std::string material) {
	_material = material;
	_type = generationAssetType::FILLING;
}


void filling::addGenerationLayer(PerlinNoise& pn, worldTemplate& toModify, int planetRadius)
{
	for (int x = 0; x < toModify.getWidth(); x++) {
		bool reachedGroundFront = false;
		bool reachedGroundBack = false;
		for (int y = toModify.getHeight() - 1; y >= 0; y--) {

			if (!reachedGroundFront) {
				if (toModify.getFrontMaterial(x, y) != nullptr) {
					if (toModify.getFrontMaterial(x, y)->getDefinitionType() == definitionType::MATERIAL) {
						reachedGroundFront = true;
					}
					else if (y <= planetRadius) {
						reachedGroundFront = true;
					}
				}
			}
			else {
				if (toModify.getFrontMaterial(x, y) == nullptr) {
					toModify.setFrontMaterial(x, y, (material*)luaHandler::getDefinition(_material));
				}
			}

			if (!reachedGroundBack) {
				if (toModify.getBackMaterial(x, y) != nullptr) {
					if (toModify.getBackMaterial(x, y)->getDefinitionType() == definitionType::MATERIAL) {
						reachedGroundBack = true;
					}
					else if (y <= planetRadius) {
						reachedGroundBack = true;
					}
				}
			}
			else {
				if (toModify.getBackMaterial(x, y) == nullptr) {
					toModify.setBackMaterial(x, y, (material*)luaHandler::getDefinition(_material));
				}
			}

		}
	}
}


generationAsset* filling::LUA_handleCreation(lua_State* S) {
	lua_pushnumber(S, 2);
	lua_gettable(S, -3);
	std::string materialModName = lua_tostring(S, -1);
	lua_pop(S, 1);

	generationAsset* newFilling = new filling(materialModName);
	lua_pop(S, 2);
	return newFilling;
}