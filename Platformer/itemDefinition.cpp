#include "itemDefinition.h"



itemDefinition::itemDefinition()
{
}


itemDefinition::~itemDefinition()
{
}

int itemDefinition::getUseDelay()
{
	return _useDelay;
}

bool itemDefinition::isTwoHanded()
{
	return _twoHanded;
}

gengine::spriteSheet itemDefinition::getTexture()
{
	return _texture;
}

gengine::GLTexture itemDefinition::getIcon()
{
	return _icon;
}

bool itemDefinition::isUsable()
{
	return _usable;
}

void itemDefinition::makeAnimated(glm::ivec2 tileDims) {
	_texture.init(*_texture.getTexture(), tileDims);
}

void itemDefinition::addAnimation(std::string animationName, int beginIndex, int endIndex, int speedBetweenFrames) {
	_texture.addAnimation(animationName, beginIndex, endIndex, speedBetweenFrames);
}

bool itemDefinition::primaryUse(heldItem* heldItem, glm::vec2 mousePos, world* toChange) {
	return false;
}

bool itemDefinition::secondaryUse(heldItem* heldItem, glm::vec2 mousePos, world* toChange) {
	return false;
}

void itemDefinition::equipped(heldItem* heldItem, glm::vec2 mousePos, world* toChange) {
	return;
}
void itemDefinition::onEquip(heldItem* heldItem, glm::vec2 mousePos, world* toChange) {
	return;
}