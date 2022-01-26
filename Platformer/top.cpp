#include "top.h"

top::top(bool front, std::string materialMod, std::string exclusivity) {
	_materialMod = materialMod;
	_exclusivity = exclusivity;
	_front = front;
	_type = generationAssetType::TOP;
}

void top::addGenerationLayer(PerlinNoise& pn, worldTemplate& toModify, int planetRadius)
{
	for (int x = 0; x < toModify.getWidth(); x++) {
		for (int y = toModify.getHeight() - 1; y >= 0; y--) {

			if (_front) {
				if (toModify.getFrontMaterial(x, y) != nullptr) {
					if (toModify.getFrontMaterial(x, y)->getDefinitionType() == definitionType::MATERIAL) {
						toModify.setFrontMaterialMod(x, y, (materialMod*)luaHandler::getDefinition(_materialMod));
						break;
					}
				}
			}
			else {
				if (toModify.getBackMaterial(x, y) != nullptr) {
					if (toModify.getBackMaterial(x, y)->getDefinitionType() == definitionType::MATERIAL) {
						toModify.setBackMaterialMod(x, y, (materialMod*)luaHandler::getDefinition(_materialMod));
						break;
					}
				}
			}


		}
	}
}


generationAsset* top::LUA_handleCreation(lua_State* S) {
	lua_pushnumber(S, 2);
	lua_gettable(S, -3);
	bool front = lua_toboolean(S, -1);
	lua_pop(S, 1);

	lua_pushnumber(S, 3);
	lua_gettable(S, -3);
	std::string materialModName = lua_tostring(S, -1);
	lua_pop(S, 1);

	lua_pushnumber(S, 4);
	lua_gettable(S, -3);
	std::string exclusivity;
	if (lua_isnoneornil(S, -1)) {
		exclusivity = "none";
	}
	else {
		exclusivity = lua_tostring(S, -1);
	}
	lua_pop(S, 1);

	top* newTop = new top(front, materialModName, exclusivity);
	lua_pop(S, 2);
	return newTop;
}