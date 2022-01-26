#include "tileDef.h"

void tileDef::setFrontMaterial(definition * frontMaterial)
{
	_frontMaterial = frontMaterial;
}

void tileDef::setBackMaterial(material * backMaterial)
{
	_backMaterial = backMaterial;
}

void tileDef::setFrontMaterialMod(materialMod * frontMod)
{
	_frontMod = frontMod;
}

void tileDef::setBackMaterialMod(materialMod * backMod)
{
	_backMod = backMod;
}

void tileDef::setEmptyFront(bool emptyFront)
{
	_emptyFront = emptyFront;
}

bool tileDef::getEmptyFront()
{
	return _emptyFront;
}

void tileDef::setEmptyBack(bool emptyBack)
{
	_emptyBack = emptyBack;
}

bool tileDef::getEmptyBack()
{
	return _emptyBack;
}

definition * tileDef::getFrontMaterial()
{
	return _frontMaterial;
}

material * tileDef::getBackMaterial()
{
	return _backMaterial;
}

materialMod * tileDef::getFrontMaterialMod()
{
	return _frontMod;
}

materialMod * tileDef::getBackMaterialMod()
{
	return _backMod;
}

bool tileDef::isNull() {
	if (_frontMaterial == nullptr && _backMaterial == nullptr) {
		return true;
	}
	else {
		return false;
	}
}