#pragma once
#include "generationAsset.h"

class spot : public generationAsset {
public:
	spot(bool front, std::string materialName, std::string exclusivity, double frequencyRatio, double sizeRatio);

	std::string getMaterial() { return _materialName; }
	std::string getExclusivity() { return _exclusivity; }
	double getFrequencyRatio() { return _frequencyRatio; }
	double getSizeRatio() { return _sizeRatio; }

	static generationAsset* LUA_handleCreation(lua_State* S);

	void addGenerationLayer(PerlinNoise& pn, worldTemplate& toModify, int planetRadius);
private:
	std::string _materialName;
	std::string _exclusivity;
	double _frequencyRatio;
	double _sizeRatio; //Value by which the coordinates of the perlin noises are divided (bigger ratio = bigger bunches)
	bool _front;
};