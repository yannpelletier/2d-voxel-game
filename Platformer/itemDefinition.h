#pragma once
#include "definition.h"
#include "lightSystem.h"
#include "heldItem.h"

class itemDefinition: public definition
{
public:
	itemDefinition();
	~itemDefinition();

	virtual bool primaryUse(heldItem* heldItem, glm::vec2 mousePos, world* toChange);
	virtual bool secondaryUse(heldItem* heldItem, glm::vec2 mousePos, world* toChange);
	virtual void equipped(heldItem* heldItem, glm::vec2 mousePos, world* toChange);
	virtual void onEquip(heldItem* heldItem, glm::vec2 mousePos, world* toChange);

	virtual void draw(gengine::spriteBatch* spriteBatch, heldItem* heldItem, glm::vec2 mousePos, world* toChange) { return; }

	int getUseDelay();
	bool isTwoHanded();

	gengine::spriteSheet getTexture();
	gengine::GLTexture getIcon();

	bool isUsable();

	void makeAnimated(glm::ivec2 tileDims);
	void addAnimation(std::string animationName, int beginIndex, int endIndex, int speedBetweenFrames);

	glm::vec2 getItemSize() { return _itemSize; }
	glm::vec2 getDrawScale() { return _drawScale; }
	glm::fvec2 getOffset() { return _offset; }
	float getAngleOffset() { return _angleOffset; }
	float getArmAngleOffset() { return _armAngleOffset; }
protected:
	std::string _showcaseName;
	gengine::GLTexture _icon;
	gengine::spriteSheet _texture;

	bool _usable;
	int _useDelay;

	glm::vec2 _itemSize;
	glm::vec2 _drawScale;
	bool _twoHanded;
	glm::fvec2 _offset;
	float _angleOffset = 0;
	float _armAngleOffset = 0;
};

