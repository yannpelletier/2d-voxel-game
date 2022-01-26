#include "entity.h"

void entity::draw(gengine::spriteBatch* spriteBatch) {
}

void entity::debugDraw(gengine::debugRenderer* debugRenderer) {
	_body->debugDraw(debugRenderer);
}

void entity::setActive(bool value) {
	_active = value;
}
bool entity::isActive() {
	return _active;
}

void entity::update() {
	return; 
}

void entity::destroy(world* world) {
	return;
}

void entity::startContact(userData* collidedWith, userData* thisFixture) {
	return;
}

void entity::endContact(userData* collidedWith, userData* thisFixture) {
	return;
}

body* entity::getBody() {
	return _body;
}