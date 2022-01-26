#include "tileTemplate.h"

void tileTemplate::setFrontMaterial(definition * frontMaterial)
{
	_frontMaterial = frontMaterial;
}

void tileTemplate::setBackMaterial(material * backMaterial)
{
	_backMaterial = backMaterial;
}

void tileTemplate::setFrontMaterialMod(materialMod * frontMod)
{
	_frontMod = frontMod;
}

void tileTemplate::setBackMaterialMod(materialMod * backMod)
{
	_backMod = backMod;
}

void tileTemplate::setEmptyFront(bool emptyFront)
{
	_emptyFront = emptyFront;
}

bool tileTemplate::getEmptyFront()
{
	return _emptyFront;
}

void tileTemplate::setEmptyBack(bool emptyBack)
{
	_emptyBack = emptyBack;
}

bool tileTemplate::getEmptyBack()
{
	return _emptyBack;
}

definition * tileTemplate::getFrontMaterial()
{
	return _frontMaterial;
}

material * tileTemplate::getBackMaterial()
{
	return _backMaterial;
}

materialMod * tileTemplate::getFrontMaterialMod()
{
	return _frontMod;
}

materialMod * tileTemplate::getBackMaterialMod()
{
	return _backMod;
}

bool tileTemplate::isNull() {
	if (_frontMaterial == nullptr && _backMaterial == nullptr) {
		return true;
	}
	else {
		return false;
	}
}