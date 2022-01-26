#include "globals.h"

const int globals::TILE_SIZE = 16;

const int globals::PIXELS_PER_METER = 32;

const int globals::TILE_BATCH_SIZE = 64;

const int globals::LUA_NAMES_MAX_CHAR = 25; //The space that occupies a single tile in the data (in bytes)

const int globals::TILE_DATA_OFFSET = (LUA_NAMES_MAX_CHAR * 4) + 3; //The space that occupies a single tile in the data (in bytes)

const int globals::PLACABLE_ITEM_DRAW_LAYER = 4;

const int globals::FRONT_TILE_DRAW_LAYER = 10;

const int globals::BACK_TILE_DRAW_LAYER = 0;

const int globals::ENTITY_DRAW_LAYER = 6;

const int globals::SKY_HEIGHT_OVER_PLANET = 100;

const float globals::MAX_TILE_RANGE = 15 * globals::pixToMet(TILE_SIZE);

const float globals::MAX_Y_SPEED = 50.0f;

float globals::pixToMet(float pixels) {
	return pixels / PIXELS_PER_METER;
}

float globals::metToPix(float meters) {
	return meters * PIXELS_PER_METER;
}

const gengine::color globals::BACK_TILE_COLOR(125, 125, 125, 255);

boost::random::mt19937 globals::rng(time(0) + SDL_GetTicks());

