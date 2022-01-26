#include "generationAsset.h"

void generationAsset::setType(generationAssetType type) {
	_type = type;
}

generationAssetType generationAsset::getType() {
	return _type;
}

void generationAsset::addGenerationLayer(PerlinNoise& pn, worldTemplate& toModify, int planetRadius)
{
	return;
}
