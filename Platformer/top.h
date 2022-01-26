#pragma once
#include "generationAsset.h"

class top : public generationAsset {	//Adds something at the top of the ground
public:
	top(bool front, std::string materialMod, std::string exclusivity);

	std::string getMaterialMod() { return _materialMod; }
	std::string getExclusivity() { return _exclusivity; }

	static generationAsset* LUA_handleCreation(lua_State* S);

	void addGenerationLayer(PerlinNoise& pn, worldTemplate& toModify, int planetRadius);

private:
	std::string _materialMod;
	std::string _exclusivity;
	bool _front;
};