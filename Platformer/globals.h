#include <Gengine\vertex.h>
#include <time.h>
#include "SDL/SDL.h"
#include <boost/random.hpp>

#pragma once
class globals {
public:
	static const int TILE_SIZE;

	static const int PIXELS_PER_METER;

	static const int TILE_BATCH_SIZE;

	static const float MAX_TILE_RANGE;

	static const float MAX_Y_SPEED;

	static float pixToMet(float pixels);

	static float metToPix(float meters);

	static const int LUA_NAMES_MAX_CHAR; //The space that occupies a single tile in the data (in bytes)

	static const int TILE_DATA_OFFSET; //The space that occupies a single tile in the data (in bytes)

	static const int PLACABLE_ITEM_DRAW_LAYER;

	static const int FRONT_TILE_DRAW_LAYER;

	static const int BACK_TILE_DRAW_LAYER;

	static const int ENTITY_DRAW_LAYER;

	static const int SKY_HEIGHT_OVER_PLANET;

	static const gengine::color BACK_TILE_COLOR;

	static boost::random::mt19937 rng;

	enum bitData {
		PLAYER_BIT = 0x0001,
		NPC_BIT = 0x0002,
		DROP_BIT = 0x0004,
		TILE_BIT = 0x0008,
		PROJECTILE_BIT = 0x0010,
		HELDITEM_BIT = 0x0020,
		SOLID_BODY_BIT = 0x0040,
		PLATFORM_BIT = 0x0080,
		FOOT_FIXTURE_BIT = 0x0100,
	};
};

