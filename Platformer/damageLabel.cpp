#include "damageLabel.h"
#include "world.h"

gengine::spriteFont damageLabel::_spriteFont = gengine::spriteFont("assets/fonts/Reitam Regular.otf", 16);

damageLabel::damageLabel() {
	this->_position = glm::vec2(0);
	this->_value = "DEFAULT";
	this->_color = gengine::color(255,0,0,255);
	this->_lifeTime = 0;
	this->_active = false;
}

void damageLabel::init() {
	_spriteFont = gengine::spriteFont("assets/fonts/Reitam Regular.otf", 32);
}

void damageLabel::activate(glm::vec2 position, int value, gengine::color color) {
	this->_position = position;
	this->_value = std::to_string(value);
	this->_color = color;
	this->_lifeTime = 1000;
	this->_active = true;
}

void damageLabel::activate(glm::vec2 position, std::string value, gengine::color color) {
	this->_position = position;
	this->_value = value;
	this->_color = color;
	this->_lifeTime = 1000;
	this->_active = true;
}

void damageLabel::update() {
	_position.y += 5 * gengine::timing::dt();
	_lifeTime -= 1000 * gengine::timing::dt();
	if (_lifeTime <= 0) {
		_active = false;
	}
}

void damageLabel::draw(gengine::spriteBatch* spriteBatch) {
	if (_active) {
		float fontWidth = _spriteFont.measure(_value.c_str()).x;
		glm::vec2 newPosition = _position - glm::vec2(globals::pixToMet(fontWidth) / 4, 0);
		_spriteFont.draw(*spriteBatch, _value.c_str(), newPosition, glm::vec2(0.0175, 0.0175), 100.f, _color);
	}
}

bool damageLabel::isActive() {
	return _active;
}
