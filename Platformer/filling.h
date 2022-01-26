#pragma once
#include "generationAsset.h"

class filling : public generationAsset {		// Fills the empty tiles up the the highest tile found
public:
	filling(std::string material);

	static generationAsset* LUA_handleCreation(lua_State* S);

	std::string getMaterial() { return _material; }

	void addGenerationLayer(PerlinNoise& pn, worldTemplate& toModify, int planetRadius);

private:
	std::string _material;
};