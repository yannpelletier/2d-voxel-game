#pragma once
#include <iostream>
#include "luaHandler.h"
#include "worldTemplate.h"
#include "perlinNoise.h"

enum generationAssetType {
	CAVE, FILLING, GROUND_MATERIAL, SPOT, TOP
};


class generationAsset {
public:
	void setType(generationAssetType type);
	generationAssetType getType();

	virtual void addGenerationLayer(PerlinNoise& pn, worldTemplate& toModify, int planetRadius);
protected:
	generationAssetType _type;
};