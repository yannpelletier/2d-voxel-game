#pragma once
#include "material.h"
#include "materialMod.h"

//Is used to save tile properties for structures
class tileDef
{
public:
	void setFrontMaterial(definition* frontMaterial);
	void setBackMaterial(material* backMaterial);

	void setFrontMaterialMod(materialMod* frontMaterial);
	void setBackMaterialMod(materialMod* backMaterial);

	void setEmptyFront(bool emptyFront);
	bool getEmptyFront();

	void setEmptyBack(bool emptyBack);
	bool getEmptyBack();

	definition* getFrontMaterial();
	material* getBackMaterial();

	materialMod* getFrontMaterialMod();
	materialMod* getBackMaterialMod();

	bool isNull();
private:
	definition* _frontMaterial = nullptr;
	material* _backMaterial = nullptr;

	materialMod* _frontMod = nullptr;
	materialMod* _backMod = nullptr;

	bool _emptyFront;
	bool _emptyBack;
};

