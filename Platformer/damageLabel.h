#pragma once
#include "Gengine/spriteBatch.h"
#include "Gengine/spriteFont.h"
#include "Gengine/timing.h"
#include "luaHandler.h"

class world;

class damageLabel
{
public:
	damageLabel();

	static void init();

	void activate(glm::vec2 position, int value, gengine::color color = gengine::color(0, 0, 0, 255));
	void activate(glm::vec2 position, std::string value, gengine::color color = gengine::color(0, 0, 0, 255));

	void update();
	void draw(gengine::spriteBatch* spriteBatch);

	bool isActive();

private:
	static gengine::spriteFont _spriteFont;

	glm::vec2 _position;
	gengine::color _color;
	std::string _value;
	int _lifeTime;
	bool _active;
};

