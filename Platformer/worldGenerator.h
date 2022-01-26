#pragma once
#include "definition.h"
#include "luaHandler.h"
#include <vector>
#include "generationAsset.h"
#include "worldTemplate.h"

class worldGenerator: public definition
{
friend class world;
public:
	worldGenerator();
	~worldGenerator();

	void addGenerationAsset(generationAsset* asset);

	worldTemplate getGeneratedWorldPortion(int width, int planetRadius, int seed);

private:
	std::vector<generationAsset*> _generationAssets;
};

