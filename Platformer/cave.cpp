#include "cave.h"

cave::cave(bool front, double frequencyRatio, double sizeRatio, int undergroundDepth) {
	_frequencyRatio = frequencyRatio;
	_sizeRatio = sizeRatio;
	_undergroundDepth = undergroundDepth;
	_front = front;
	_type = generationAssetType::CAVE;
}


void cave::addGenerationLayer(PerlinNoise& pn, worldTemplate& toModify, int planetRadius)
{
	for (unsigned int y = 0; y < toModify.getHeight(); y++) {
		for (unsigned int x = 0; x < toModify.getWidth(); x++) {
			double caveX = (double)x / _sizeRatio;
			double caveY = (double)y / _sizeRatio;
			double n = pn.noise(caveX, caveY, 0.0f);

			if (n * 100 <= _frequencyRatio) {
				if (y <= planetRadius - _undergroundDepth && y != 0) {
					if (_front) {
						toModify.setFrontMaterial(x, y, nullptr);
					}
					else {
						toModify.setBackMaterial(x, y, nullptr);
					}
				}
			}
		}
	}
}


generationAsset* cave::LUA_handleCreation(lua_State* S) {
	lua_pushnumber(S, 2);
	lua_gettable(S, -3);
	bool front = lua_toboolean(S, -1);
	lua_pop(S, 1);

	lua_pushnumber(S, 3);
	lua_gettable(S, -3);
	double frequencyRatio = lua_tonumber(S, -1);
	lua_pop(S, 1);

	lua_pushnumber(S, 4);
	lua_gettable(S, -3);
	double sizeRatio = lua_tonumber(S, -1);
	lua_pop(S, 1);

	lua_pushnumber(S, 5);
	lua_gettable(S, -3);
	int undergroundDepth = lua_tointeger(S, -1);
	lua_pop(S, 1);

	generationAsset* newCave = new cave(front,frequencyRatio, sizeRatio, undergroundDepth);
	lua_pop(S, 2);
	return newCave;
}