#include "worldTemplate.h"

worldTemplate::worldTemplate(unsigned int width, unsigned int height) {
	for (int y = 0; y < height; y++) {
		std::vector<tileTemplate> templatesRow;
		templatesRow.resize(width);
		_tileTemplates.push_back(templatesRow);
	}
}

void worldTemplate::setFrontMaterial(unsigned int x, unsigned int y, definition * frontMaterial)
{
	if (_validateTile(x, y)) {
		_tileTemplates[y][x].setFrontMaterial(frontMaterial);
	}
}

void worldTemplate::setBackMaterial(unsigned int x, unsigned int y, material * backMaterial)
{
	if (_validateTile(x, y)) {
		_tileTemplates[y][x].setBackMaterial(backMaterial);
	}
}

void worldTemplate::setFrontMaterialMod(unsigned int x, unsigned int y, materialMod * frontMod)
{
	if (_validateTile(x, y)) {
		_tileTemplates[y][x].setFrontMaterialMod(frontMod);
	}
}

void worldTemplate::setBackMaterialMod(unsigned int x, unsigned int y, materialMod * backMod)
{
	if (_validateTile(x, y)) {
		_tileTemplates[y][x].setBackMaterialMod(backMod);
	}
}

void worldTemplate::setEmptyFront(unsigned int x, unsigned int y, bool emptyFront)
{
	if (_validateTile(x, y)) {
		_tileTemplates[y][x].setEmptyFront(emptyFront);
	}
}

bool worldTemplate::getEmptyFront(unsigned int x, unsigned int y)
{
	if (_validateTile(x, y)) {
		return _tileTemplates[y][x].getEmptyFront();
	}
	else {
		return false;
	}
}

void worldTemplate::setEmptyBack(unsigned int x, unsigned int y, bool emptyBack)
{
	if (_validateTile(x, y)) {
		_tileTemplates[y][x].setEmptyBack(emptyBack);
	}
}

bool worldTemplate::getEmptyBack(unsigned int x, unsigned int y)
{
	if (_validateTile(x, y)) {
		return _tileTemplates[y][x].getEmptyBack();
	}
	else {
		return false;
	}
}

definition * worldTemplate::getFrontMaterial(unsigned int x, unsigned int y)
{
	if (_validateTile(x, y)) {
		return _tileTemplates[y][x].getFrontMaterial();
	}
	else {
		return nullptr;
	}
}

material * worldTemplate::getBackMaterial(unsigned int x, unsigned int y)
{
	if (_validateTile(x, y)) {
		return _tileTemplates[y][x].getBackMaterial();
	}
	else {
		return nullptr;
	}
}

materialMod * worldTemplate::getFrontMaterialMod(unsigned int x, unsigned int y)
{
	if (_validateTile(x, y)) {
		return _tileTemplates[y][x].getFrontMaterialMod();
	}
	else {
		return nullptr;
	}
}

materialMod * worldTemplate::getBackMaterialMod(unsigned int x, unsigned int y)
{
	if (_validateTile(x, y)) {
		return _tileTemplates[y][x].getBackMaterialMod();
	}
	else {
		return nullptr;
	}
}

int worldTemplate::getWidth() {
	if (_tileTemplates.size() == 0) {
		return 0;
	}
	else {
		return _tileTemplates[0].size();
	}
}

int worldTemplate::getHeight() {
	return _tileTemplates.size();
}
 
bool worldTemplate::_validateTile(unsigned int x, unsigned int y) {
	return (x < getWidth() && y < getHeight());
}

