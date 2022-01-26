#include "tile.h"
#include "world.h"
#include "definition.h"
#include "GUI.h"
#include "largeTileBody.h"
#include "platform.h"
#include <Gengine/errors.h>

int tile::tileRepairTime;
const int tile::TIME_TO_REPAIR = 3000;
const glm::vec2 tile::PARTICLE_SPEED_RANGE = glm::vec2(0.05, 0.15);
const glm::vec2 tile::PARTICLE_WIDTH_RANGE = glm::vec2(0.10, 0.20);
const glm::vec2 tile::PARTICLE_NUMBER_RANGE = glm::vec2(8, 28);

tile::tile() {
	_type == tileType::INVALID;
}

tile* getTileFromContact() {
	return nullptr;
}

void tile::init(int gridX, int gridY, definition * definition, bool front, int autoTileCount)
{
	if (front) {
		_initFront(gridX, gridY, definition, autoTileCount);
	}
	else{
		_initBack(gridX, gridY, definition, autoTileCount);
	}
}

void tile::_initFront(int gridX, int gridY, definition * definition, int autoTileCount)
{
	return;
}

void tile::_initBack(int gridX, int gridY, definition * definition, int autoTileCount) {
	return;
}

void tile::addMaterialMod(materialMod* mod, bool front)
{
	return;
}

void tile::destroy(bool front, bool drop)
{
	if (front) {
		_destroyFront(drop);
	}
	else {
		_destroyBack(drop);
	}
}

void tile::draw(gengine::spriteBatch * spriteBatch)
{
	_drawFront(spriteBatch);
	_drawBack(spriteBatch);
}

void tile::update()
{
	_updateFront();
	_updateBack();
}

void tile::occupyBatch(tileHitbox * tileBatch)
{
	return;
}

void tile::use(agent * user)
{
	return;
}

void tile::damage(int damage, bool front)
{
	if (front) {
		_damageFront(damage);
	}
	else {
		_damageBack(damage);
	}
}

void tile::setAutoTileCount(bool front, int autoTileCount)
{
	return;
}

void tile::setPosition(int gridX, int gridY)
{
	return;
}

tileType tile::getType()
{
	return tileType::INVALID;
}

materialModInstance * tile::getFrontModInstance()
{
	return nullptr;
}

materialModInstance * tile::getBackModInstance()
{
	return nullptr;
}

tileHitbox * tile::getBatch()
{
	return nullptr;
}

glm::vec2 tile::getTilePosition() {
	return glm::vec2((int)(_position.x / globals::pixToMet(globals::TILE_SIZE)), (int)(_position.y / globals::pixToMet(globals::TILE_SIZE)));
}

bool tile::isInMesh()
{
	return false;
}

bool tile::isSolid()
{
	return false;
}

bool tile::hasFront()
{
	return false;
}

bool tile::hasBack()
{
	return false;
}

bool tile::isFloor()
{
	return false;
}

material * tile::getFrontMaterial()
{
	return nullptr;
}

material * tile::getBackMaterial()
{
	return nullptr;
}

int tile::getFrontHealth()
{
	return 0;
}

int tile::getBackHealth()
{
	return 0;
}


void tile::_damageFront(int damage)
{
	return;
}

void tile::_damageBack(int damage)
{
	return;
}

void tile::_drawFront(gengine::spriteBatch* spriteBatch) {
	return;
}

void tile::_drawBack(gengine::spriteBatch* spriteBatch) {
	return;
}

void tile::_updateFront()
{
	return;
}

void tile::_updateBack()
{
	return;
}

void tile::_destroyFront(bool drop)
{
	return;
}

void tile::_destroyBack(bool drop)
{
	return;
}

void tile::_emitParticles(gengine::color particleColor)
{
	boost::random::uniform_real_distribution<float> angleRNG(0, 2 * M_PI);
	boost::random::uniform_real_distribution<float> speedRNG(PARTICLE_SPEED_RANGE.x, PARTICLE_SPEED_RANGE.y);
	boost::random::uniform_real_distribution<float> numberRNG(PARTICLE_NUMBER_RANGE.x, PARTICLE_NUMBER_RANGE.y);
	boost::random::uniform_real_distribution<float> widthRNG(PARTICLE_WIDTH_RANGE.x, PARTICLE_WIDTH_RANGE.y);

	particle* materialParticle = static_cast<particle*>(luaHandler::getDefinition("material_p"));
	glm::vec2 velocity;
	float speed;
	float angle;

	for (int i = 0; i < numberRNG(globals::rng); i++) {
		speed = speedRNG(globals::rng);
		angle = angleRNG(globals::rng);

		velocity = glm::vec2(cos(angle) * speed, sin(angle) * speed);
		materialParticle->getParticleBatch()->addParticle(_position, velocity, particleColor, widthRNG(globals::rng));
	}
}


int luaHandler::LUA_TILE_getPosition(lua_State *S) {
	tile* p = *((tile**)lua_touserdata(S, 1));

	glm::vec2 pos = p->getTilePosition();

	lua_newtable(S);
	lua_pushnumber(S, 1);
	lua_pushnumber(S, globals::metToPix(pos.x));
	lua_settable(S, -3);
	lua_pushnumber(S, 2);
	lua_pushnumber(S, globals::metToPix(pos.y));
	lua_settable(S, -3);

	return 1;
}

int luaHandler::LUA_TILE_setFrontMaterialMod(lua_State *S) {
	tile* p = *((tile**)lua_touserdata(S, 1));

	p->addMaterialMod((materialMod*)getDefinition(lua_tostring(S, 2)),true);

	return 0;
}

int luaHandler::LUA_TILE_setbackMaterialMod(lua_State *S) {
	tile* p = *((tile**)lua_touserdata(S, 1));

	p->addMaterialMod((materialMod*)getDefinition(lua_tostring(S, 2)), false);

	return 0;
}

int luaHandler::LUA_TILE_getFrontMaterial(lua_State *S) {
	if (lua_isnoneornil(S, 1)) {
		lua_pushstring(S, "");
	}
	else {
		tile* p = *((tile**)lua_touserdata(S, 1));

		if (p->getFrontMaterial() != nullptr) {
			lua_pushstring(S, p->getFrontMaterial()->_nameID.c_str());
		}
		else {
			lua_pushstring(S, "");
		}
	}

	return 1;
}

int luaHandler::LUA_TILE_getBackMaterial(lua_State *S) {
	if (lua_isnoneornil(S, 1)) {
		lua_pushstring(S, "");
	}
	else {
		tile* p = *((tile**)lua_touserdata(S, 1));

		if (p->getBackMaterial() != nullptr) {
			lua_pushstring(S, p->getBackMaterial()->_nameID.c_str());
		}
		else {
			lua_pushstring(S, "");
		}
	}


	return 1;
}

int luaHandler::LUA_TILE_getFrontMaterialMod(lua_State *S) {
	if (lua_isnoneornil(S, 1)) {
		lua_pushstring(S, "");
	}
	else {
		tile* p = *((tile**)lua_touserdata(S, 1));

		if (p->getFrontModInstance() != nullptr) {
			lua_pushstring(S, p->getFrontModInstance()->getMaterialMod()->_nameID.c_str());
		}
		else {
			lua_pushstring(S, "");
		}
	}

	return 1;
}

int luaHandler::LUA_TILE_getBackMaterialMod(lua_State *S) {
	if (lua_isnoneornil(S, 1)) {
		lua_pushstring(S, "");
	}
	else {
		tile* p = *((tile**)lua_touserdata(S, 1));

		if (p->getBackModInstance() != nullptr) {
			lua_pushstring(S, p->getBackModInstance()->getMaterialMod()->_nameID.c_str());
		}
		else {
			lua_pushstring(S, "");
		}
	}


	return 1;
}

int luaHandler::LUA_TILE_frontIsModdable(lua_State *S) {
	if (lua_isnoneornil(S, 1)) {
		lua_pushboolean(S, false);
	}
	else {
		tile* p = *((tile**)lua_touserdata(S, 1));

		if (p->hasFront() && p->getType() == tileType::REGULAR_TILE) {
			lua_pushboolean(S, true);
		}
		else {
			lua_pushboolean(S, false);
		}
	}

	return 1;
}

int luaHandler::LUA_TILE_backIsModdable(lua_State *S) {
	if (lua_isnoneornil(S, 1)) {
		lua_pushboolean(S, false);
	}
	else {
		tile* p = *((tile**)lua_touserdata(S, 1));

		lua_pushboolean(S, p->hasBack());
	}

	return 1;
}

int luaHandler::LUA_TILE_hasFront(lua_State *S) {
	if (lua_isnoneornil(S, 1)) {
		lua_pushboolean(S, false);
	}
	else {
		tile* p = *((tile**)lua_touserdata(S, 1));

		lua_pushboolean(S, p->hasFront());
	}

	return 1;
}

int luaHandler::LUA_TILE_hasBack(lua_State *S) {
	if (lua_isnoneornil(S, 1)) {
		lua_pushboolean(S, false);
	}
	else {
		tile* p = *((tile**)lua_touserdata(S, 1));

		lua_pushboolean(S, p->hasBack());
	}

	return 1;
}

int luaHandler::LUA_TILE_isSolid(lua_State *S) {
	if (lua_isnoneornil(S, 1)) {
		lua_pushboolean(S, false);
	}
	else {
		tile* p = *((tile**)lua_touserdata(S, 1));

		if (p->isSolid() && p->hasFront()) {
			lua_pushboolean(S, true);
		}
		else {
			lua_pushboolean(S, false);
		}
	}

	return 1;
}

int luaHandler::LUA_TILE_drop(lua_State *S) {
	if (lua_isnoneornil(S, 1)) {
		return 0;
	}
	else {
		tile* p = *((tile**)lua_touserdata(S, 1));

		int quantity;
		if (lua_isnoneornil(S, 3)) {
			quantity = 1;
		}
		else {
			quantity = lua_tonumber(S, 3);
		}

		new inventoryItem(quantity, (itemDefinition*)getDefinition(lua_tostring(S, 2)), p->getTilePosition() * globals::pixToMet(globals::TILE_SIZE) + globals::pixToMet(globals::TILE_SIZE) / 2, _world);
	}

	return 0;

}


void luaHandler::_registerTileFunction() {
	luaL_Reg tilelib_m[] = {
		{ "getPosition",  LUA_TILE_getPosition },
		{ "setFrontMaterialMod",  LUA_TILE_setFrontMaterialMod },
		{ "setBackMaterialMod",  LUA_TILE_setbackMaterialMod },
		{ "getFrontMaterialMod",  LUA_TILE_getFrontMaterialMod },
		{ "getBackMaterialMod",  LUA_TILE_getBackMaterialMod },
		{ "getFrontMaterial",  LUA_TILE_getFrontMaterial },
		{ "getBackMaterial",  LUA_TILE_getBackMaterial },
		{ "frontIsModdable",  LUA_TILE_frontIsModdable },
		{ "backIsModdable",  LUA_TILE_backIsModdable },
		{ "hasFront",  LUA_TILE_hasFront },
		{ "hasBack",  LUA_TILE_hasBack },
		{ "isSolid",  LUA_TILE_isSolid },
		{ "drop",  LUA_TILE_drop },
		{ NULL, NULL }
	};

	luaL_newmetatable(_S, "tile_m");

	lua_pushstring(_S, "__index");
	lua_pushvalue(_S, -2);  /* pushes the metatable*/
	lua_settable(_S, -3);  /* metatable.__index = metatable */

	luaL_setfuncs(_S, tilelib_m, 0);
}