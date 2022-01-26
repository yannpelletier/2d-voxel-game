#pragma once
#include "tileTemplate.h"
#include "definition.h"
#include <vector>

class worldTemplate
{
public:
	worldTemplate(unsigned int width, unsigned int height);

	void setFrontMaterial(unsigned int x, unsigned int y,definition* frontMaterial);
	void setBackMaterial(unsigned int x, unsigned int y, material* backMaterial);

	void setFrontMaterialMod(unsigned int x, unsigned int y, materialMod* frontMaterial);
	void setBackMaterialMod(unsigned int x, unsigned int y, materialMod* backMaterial);

	void setEmptyFront(unsigned int x, unsigned int y, bool emptyFront);
	bool getEmptyFront(unsigned int x, unsigned int y);

	void setEmptyBack(unsigned int x, unsigned int y, bool emptyBack);
	bool getEmptyBack(unsigned int x, unsigned int y);

	definition* getFrontMaterial(unsigned int x, unsigned int y);
	material* getBackMaterial(unsigned int x, unsigned int y);

	materialMod* getFrontMaterialMod(unsigned int x, unsigned int y);
	materialMod* getBackMaterialMod(unsigned int x, unsigned int y);

	int getWidth();
	int getHeight();

private:
	std::vector<std::vector<tileTemplate>> _tileTemplates;

	bool _validateTile(unsigned int x, unsigned int y);
};

