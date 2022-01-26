#pragma once
#include "generationAsset.h"

class groundMaterial : public generationAsset {	//Composes a layer of tiles with minimum height variations
public:
	groundMaterial(std::string material, double percentage, double sizeRatio, int maxHeightOffset);

	std::string getMaterial() { return _material; }
	int getMaxHeightOffset() { return _maxHeightOffset; }
	double getPercentage() { return _percentage; }
	double getSizeRatio() { return _sizeRatio; }

	static generationAsset* LUA_handleCreation(lua_State* S);

	void addGenerationLayer(PerlinNoise& pn, worldTemplate& toModify, int planetRadius);
private:
	std::string _material; //Material that is used to 
	int _maxHeightOffset; //Max number of tiles that a base can go over the default ground level
	double _percentage; //Percentage of the material that composes the ground
	double _sizeRatio; //Value by which the coordinates of the perlin noises are divided (bigger ratio = bigger variation every tile)
};