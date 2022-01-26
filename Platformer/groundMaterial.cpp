#include "groundMaterial.h"

groundMaterial::groundMaterial(std::string material, double percentage, double sizeRatio, int maxHeightOffset) {
	_material = material;
	_maxHeightOffset = maxHeightOffset;
	_percentage = percentage;
	_sizeRatio = sizeRatio;
	_type = generationAssetType::GROUND_MATERIAL;
}

void groundMaterial::addGenerationLayer(PerlinNoise& pn, worldTemplate& toModify, int planetRadius)
{
	int min = planetRadius;
	int max = planetRadius + _maxHeightOffset;

	for (int x = 0; x < toModify.getWidth(); x++) {
		double caveX = (double)x / _sizeRatio;
		double n = pn.noise(caveX, 0.0f, 0.0f);


		int height = min + (int)(std::round((max - min) * n));
		for (int y = height; y >= 0; y--) {
			double caveY = (double)y / _sizeRatio;
			caveX = (double)x / _sizeRatio;
			n = pn.noise(caveX, caveY, 0.0f);

			if (n * 100 <= _percentage) {
				if (toModify.getFrontMaterial(x,y) == nullptr) {
					toModify.setFrontMaterial(x, y, (material*)luaHandler::getDefinition(_material));
				}
			}

			caveY = (double)y / _sizeRatio + 100;
			caveX = (double)x / _sizeRatio + 100;
			n = pn.noise(caveX, caveY, 0.0f);

			if (n * 100 <= _percentage) {
				if (toModify.getBackMaterialMod(x, y) == nullptr) {
					toModify.setBackMaterial(x, y, (material*)luaHandler::getDefinition(_material));
				}
			}
		}
	}
}

generationAsset* groundMaterial::LUA_handleCreation(lua_State *S) {
	lua_pushnumber(S, 2);
	lua_gettable(S, -3);
	std::string materialName = lua_tostring(S, -1);
	lua_pop(S, 1);

	lua_pushnumber(S, 3);
	lua_gettable(S, -3);
	float percentage = lua_tointeger(S, -1);
	lua_pop(S, 1);

	lua_pushnumber(S, 4);
	lua_gettable(S, -3);
	float sizeRatio = lua_tointeger(S, -1);
	lua_pop(S, 1);

	lua_pushnumber(S, 5);
	lua_gettable(S, -3);
	int maxOffsetHeight = lua_tointeger(S, -1);
	lua_pop(S, 1);

	generationAsset* newGroundMaterial = new groundMaterial(materialName,percentage,sizeRatio, maxOffsetHeight);
	lua_pop(S, 2);
	return newGroundMaterial;
}