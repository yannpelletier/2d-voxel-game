#pragma once
#include "Lua\lua.hpp"
#include "SDL/SDL.h"
#include <string>
#include <vector>
#include <Gengine/errors.h>
#include <Gengine/audioEngine.h>
#include <map>
#include <boost/filesystem.hpp>
#include <iostream>
#include "definition.h"
	   
class world;
class definition;
class material;
class particle;
class placableItem;

enum luaVariableType { INTL, FLOATL, BOOLL, TIMERL, BODYL, PLATFORML };

class luaHandler
{
public:
	static void createLuaState();
	static void registerLuaFunctions();
	static void loadLua(); //Loads every Lua file in the file path
	static void setWorld(world* world);

	static lua_State* getLuaState();
	static world* getWorld();
	static definition* getDefinition(std::string ID);
	static bool validateDefinition(definition* def);

	static std::string getPath(std::string smallPath);
	static bool globalValueExists(std::string tableName);
	static int getTableInt(std::string tableName, std::string variableName);
	static float getTableFloat(std::string tableName, std::string variableName);
	static bool getTableBool(std::string tableName, std::string variableName);
	static std::string getTableString(std::string tableName, std::string variableName);
	static glm::vec2 luaHandler::getTableRange(std::string tableName, std::string variableName);
	static std::vector<float> getTableTableValues(std::string tableName, std::string variableName);
	static std::vector<std::string> luaHandler::getTableTableStrings(std::string tableName, std::string variableName);

	static SDL_Keycode LUA_INPUT_luaToSDL(std::string key);

private:
	static void _registerInputFunctions();
	static void _registerParticleFunction();
	static void _registerUsableFunction();
	static void _registerParticleEmitterFunction();
	static void _registerPlacableFunction();
	static void _registerLightFunction();
	static void _registerSoundFunction();
	static void _registerTimerFunction();
	static void _registerProjectileFunction();
	static void _registerMaterialModFunction();
	static void _registerWorldFunction();
	static void _registerTileFunction();
	static void _registerAgentFunction();
	static void _registerSolidBodyFunction();
	static void _registerEventFunction();
	static void _registerEffectFunction();

	static void _iterateDirectory(std::string folder, std::vector<std::string>* luaPaths);

	static bool _verifyLuaName(std::string name);

	static int LUA_MATERIAL_newMaterial(lua_State *S);
	static int LUA_MATERIAL_MOD_newMaterialMod(lua_State *S);
	static int LUA_USABLE_newUsable(lua_State *S);
	static int LUA_PLACABLE_newPlacableObject(lua_State *S);
	static int LUA_PARTICLE_newParticle(lua_State *S);
	static int LUA_PARTICLE_EMITTER_newParticleEmitter(lua_State *S);
	static int LUA_PROJECTILE_newProjectile(lua_State *S);
	static int LUA_WORLD_GENERATOR_newWorldGenerator(lua_State *S);
	static int LUA_AGENT_newNPC(lua_State *S);
	static int LUA_EVENT_newEvent(lua_State *S);
	static int LUA_EFFECT_newEffect(lua_State *S);

	static int LUA_TILE_getPosition(lua_State *S);
	static int LUA_TILE_setFrontMaterialMod(lua_State *S);
	static int LUA_TILE_setbackMaterialMod(lua_State *S);
	static int LUA_TILE_getFrontMaterial(lua_State *S);
	static int LUA_TILE_getBackMaterial(lua_State *S);
	static int LUA_TILE_getFrontMaterialMod(lua_State *S);
	static int LUA_TILE_getBackMaterialMod(lua_State *S);
	static int LUA_TILE_frontIsModdable(lua_State *S);
	static int LUA_TILE_backIsModdable(lua_State *S);
	static int LUA_TILE_hasFront(lua_State *S);
	static int LUA_TILE_hasBack(lua_State *S);
	static int LUA_TILE_isSolid(lua_State *S);
	static int LUA_TILE_drop(lua_State *S);

	static int LUA_AGENT_walkRight(lua_State *S);
	static int LUA_AGENT_walkLeft(lua_State *S);
	static int LUA_AGENT_isWalkingRight(lua_State *S);
	static int LUA_AGENT_isWalkingLeft(lua_State *S);
	static int LUA_AGENT_jump(lua_State *S);
	static int LUA_AGENT_getPosition(lua_State *S);
	static int LUA_AGENT_getDimensions(lua_State *S);
	static int LUA_AGENT_getVelocity(lua_State *S);
	static int LUA_AGENT_getJumpForce(lua_State *S);
	static int LUA_AGENT_enableBehaviour(lua_State *S);
	static int LUA_AGENT_disableBehaviour(lua_State *S);
	static int LUA_AGENT_doBehaviourOnce(lua_State *S);
	static int LUA_AGENT_damage(lua_State *S);
	static int LUA_AGENT_knockBack(lua_State *S);
	static int LUA_AGENT_setInt(lua_State *S);
	static int LUA_AGENT_setFloat(lua_State *S);
	static int LUA_AGENT_setBool(lua_State *S);
	static int LUA_AGENT_setTimer(lua_State *S);
	static int LUA_AGENT_getInt(lua_State *S);
	static int LUA_AGENT_getFloat(lua_State *S);
	static int LUA_AGENT_getBool(lua_State *S);
	static int LUA_AGENT_getTimer(lua_State *S);
	static int LUA_AGENT_playAnimation(lua_State *S);

	static int LUA_MATERIAL_MOD_setAnimation(lua_State *S);
	static int LUA_MATERIAL_MOD_addInt(lua_State *S);
	static int LUA_MATERIAL_MOD_addFloat(lua_State *S);
	static int LUA_MATERIAL_MOD_addBool(lua_State *S);
	static int LUA_MATERIAL_MOD_addTimer(lua_State *S);
	static int LUA_MATERIAL_MOD_setValue(lua_State *S);
	static int LUA_MATERIAL_MOD_getValue(lua_State *S);

	static int LUA_EVENT_getDuration(lua_State *S);
	static int LUA_EVENT_getOccuranceTime(lua_State *S);
	static int LUA_EVENT_addInt(lua_State *S);
	static int LUA_EVENT_addFloat(lua_State *S);
	static int LUA_EVENT_addBool(lua_State *S);
	static int LUA_EVENT_addTimer(lua_State *S);
	static int LUA_EVENT_getValue(lua_State *S);
	static int LUA_EVENT_setValue(lua_State *S);

	static int LUA_EFFECT_addInt(lua_State *S);
	static int LUA_EFFECT_addFloat(lua_State *S);
	static int LUA_EFFECT_addBool(lua_State *S);
	static int LUA_EFFECT_addTimer(lua_State *S);
	static int LUA_EFFECT_getValue(lua_State *S);
	static int LUA_EFFECT_setValue(lua_State *S);
	static int LUA_EFFECT_destroy(lua_State *S);

	static int LUA_SOLID_BODY_setSolid(lua_State *S);

	static int LUA_USABLE_setAnimation(lua_State *S);
	static int LUA_USABLE_createProjectile(lua_State *S);
	static int LUA_USABLE_addInt(lua_State *S);
	static int LUA_USABLE_addFloat(lua_State *S);
	static int LUA_USABLE_addBool(lua_State *S);
	static int LUA_USABLE_addTimer(lua_State *S);
	static int LUA_USABLE_getValue(lua_State *S);
	static int LUA_USABLE_setValue(lua_State *S);
	static int LUA_USABLE_consume(lua_State *S);
	static int LUA_USABLE_addLight(lua_State *S);
	static int LUA_USABLE_getLight(lua_State *S);
	static int LUA_USABLE_getEmitter(lua_State *S);
	static int LUA_USABLE_setEmitter(lua_State *S);
	static int LUA_USABLE_setOffset(lua_State *S);
	static int LUA_USABLE_setAngleOffset(lua_State *S);
	static int LUA_USABLE_getPosition(lua_State *S);
	static int LUA_USABLE_calculateOffset(lua_State *S);
	static int LUA_USABLE_getAngleDeg(lua_State *S);
	static int LUA_USABLE_getAngleRad(lua_State *S);
	static int LUA_USABLE_getDirection(lua_State *S);
	static int LUA_USABLE_getHolder(lua_State *S);
	static int LUA_USABLE_getUseButton(lua_State *S);
	static int LUA_USABLE_getArmAnimationName(lua_State *S);
	static int LUA_USABLE_getEventState(lua_State *S);
	static int LUA_USABLE_playAnimation(lua_State *S);

	static int LUA_PARTICLE_EMITTER_addEmittedParticle(lua_State *S);
	static int LUA_PARTICLE_EMITTER_enable(lua_State *S);
	static int LUA_PARTICLE_EMITTER_disable(lua_State *S);

	static int LUA_PARTICLE_emitParticle(lua_State *S);
	static int LUA_PARTICLE_setVelocity(lua_State *S);
	static int LUA_PARTICLE_setColor(lua_State *S);
	static int LUA_PARTICLE_getVelocity(lua_State *S);
	static int LUA_PARTICLE_getColor(lua_State *S);
	static int LUA_PARTICLE_getLifeTime(lua_State *S);

	static int LUA_PLACABLE_getCenterPosition(lua_State *S);
	static int LUA_PLACABLE_addLight(lua_State *S);
	static int LUA_PLACABLE_getLight(lua_State *S);
	static int LUA_PLACABLE_getEmitter(lua_State *S);
	static int LUA_PLACABLE_setEmitter(lua_State *S);
	static int LUA_PLACABLE_setAnimation(lua_State *S);
	static int LUA_PLACABLE_initInventory(lua_State *S);
	static int LUA_PLACABLE_openInventory(lua_State *S);
	static int LUA_PLACABLE_isInventoryClosed(lua_State *S);
	static int LUA_PLACABLE_addInt(lua_State *S);
	static int LUA_PLACABLE_addFloat(lua_State *S);
	static int LUA_PLACABLE_addBool(lua_State *S);
	static int LUA_PLACABLE_addBody(lua_State *S);
	static int LUA_PLACABLE_addPlatform(lua_State *S);
	static int LUA_PLACABLE_addTimer(lua_State *S);
	static int LUA_PLACABLE_setValue(lua_State *S);
	static int LUA_PLACABLE_getValue(lua_State *S);

	static int LUA_PROJECTILE_setEmitter(lua_State *S);
	static int LUA_PROJECTILE_getEmitter(lua_State *S);
	static int LUA_PROJECTILE_addLight(lua_State *S);
	static int LUA_PROJECTILE_getLight(lua_State *S);
	static int LUA_PROJECTILE_setSide(lua_State *S);
	static int LUA_PROJECTILE_setAngleDeg(lua_State *S);
	static int LUA_PROJECTILE_setAngleRad(lua_State *S);
	static int LUA_PROJECTILE_setDirection(lua_State *S);
	static int LUA_PROJECTILE_setVelocity(lua_State *S);
	static int LUA_PROJECTILE_setGravityScale(lua_State *S);
	static int LUA_PROJECTILE_setAngularVelocity(lua_State *S);
	static int LUA_PROJECTILE_setAnimation(lua_State *S);
	static int LUA_PROJECTILE_getAngle(lua_State *S);
	static int LUA_PROJECTILE_getDirection(lua_State *S);
	static int LUA_PROJECTILE_getVelocity(lua_State *S);
	static int LUA_PROJECTILE_getAngularVelocity(lua_State *S);
	static int LUA_PROJECTILE_getPosition(lua_State *S);
	static int LUA_PROJECTILE_destroy(lua_State *S);
	static int LUA_PROJECTILE_addInt(lua_State *S);
	static int LUA_PROJECTILE_addFloat(lua_State *S);
	static int LUA_PROJECTILE_addBool(lua_State *S);
	static int LUA_PROJECTILE_addTimer(lua_State *S);
	static int LUA_PROJECTILE_setValue(lua_State *S);
	static int LUA_PROJECTILE_getValue(lua_State *S);

	static int LUA_LIGHT_setColor(lua_State *S);
	static int LUA_LIGHT_setIntensity(lua_State *S);
	static int LUA_LIGHT_setRadius(lua_State *S);
	static int LUA_LIGHT_setDirection(lua_State *S);
	static int LUA_LIGHT_setOffset(lua_State *S);
	static int LUA_LIGHT_setConeAngle(lua_State *S);
	static int LUA_LIGHT_getColor(lua_State *S);
	static int LUA_LIGHT_getIntensity(lua_State *S);
	static int LUA_LIGHT_getRadius(lua_State *S);
	static int LUA_LIGHT_getDirection(lua_State *S);
	static int LUA_LIGHT_getConeAngle(lua_State *S);

	static int LUA_TIMER_getTime(lua_State *S);
	static int LUA_TIMER_stop(lua_State *S);
	static int LUA_TIMER_start(lua_State *S);
	static int LUA_TIMER_reset(lua_State *S);
	static int LUA_TIMER_isStopped(lua_State *S);

	static int LUA_WORLD_addToAmbientLightColor(lua_State *S);
	static int LUA_WORLD_getAmbientLightColor(lua_State *S);
	static int LUA_WORLD_damageFrontTile(lua_State *S);
	static int LUA_WORLD_damageBackTile(lua_State *S);
	static int LUA_WORLD_getTile(lua_State *S);
	static int LUA_WORLD_raycast(lua_State *S);
	static int LUA_WORLD_createProjectile(lua_State *S);
	static int LUA_WORLD_createEffect(lua_State *S);
	static int LUA_WORLD_getCameraPosition(lua_State *S);
	static int LUA_WORLD_getCameraDimensions(lua_State *S);

	static int LUA_SOUND_playSFX(lua_State *S);
	static int LUA_SOUND_playMusic(lua_State *S);
	static int LUA_SOUND_stopMusic(lua_State *S);
	static int LUA_SOUND_pauseMusic(lua_State *S);
	static int LUA_SOUND_resumeMusic(lua_State *S);
	static int LUA_SOUND_setChannelVolume(lua_State *S);
	static int LUA_SOUND_getChannelVolume(lua_State *S);

	static int LUA_INPUT_isKeyDown(lua_State *S);
	static int LUA_INPUT_wasKeyDown(lua_State *S);
	static int LUA_INPUT_keyPressed(lua_State *S);
	static int LUA_INPUT_keyReleased(lua_State *S);
	static std::map<std::string, SDL_Keycode> LUA_INPUT_codes;

	static std::string currentFolder;

	static definition* errorDefinition;

	static world* _world;
	static std::map<std::string,definition*> _definitions;
	static lua_State *_S;
};