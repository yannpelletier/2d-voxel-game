#pragma once
#include "generationAsset.h"

class cave : public generationAsset {	//Adds caves into the ground
public:
	cave(bool front, double frequencyRatio, double sizeRatio, int undergroundDepth);

	double getFrequencyRatio() { return _frequencyRatio; }
	double getSizeRatio() { return _sizeRatio; }
	int getUndergroundDepth() { return _undergroundDepth; }

	static generationAsset* LUA_handleCreation(lua_State* S);

	void addGenerationLayer(PerlinNoise& pn, worldTemplate& toModify, int planetRadius);

private:
	double _frequencyRatio; // Ratio that sets how many caves there are
	double _sizeRatio; //Value by which the coordinates of the perlin noises are divided (bigger ratio = bigger bunches)
	int _undergroundDepth = 0; // From how many tile from the surface can there be caves
	bool _front;
};