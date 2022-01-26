#include "luaHandler.h"
#include "world.h"
#include "definition.h"
#include "material.h"
#include "particle.h"
#include "placableItem.h"
#include <iostream>
#include "boost/filesystem/operations.hpp"
#include <boost/algorithm/string.hpp>
#include "boost/filesystem/path.hpp"
#include "boost/progress.hpp"
#include <iostream>

using namespace boost::filesystem;

world* luaHandler::_world = nullptr;
lua_State* luaHandler::_S = nullptr;
std::map<std::string, definition*> luaHandler::_definitions;
std::map<std::string, SDL_Keycode> luaHandler::LUA_INPUT_codes;
std::string luaHandler::currentFolder;
definition* luaHandler::errorDefinition = new definition();

void luaHandler::createLuaState() {
	_S = luaL_newstate();
	luaL_openlibs(_S);
}

void luaHandler::_iterateDirectory(std::string folder, std::vector<std::string>* luaPaths) {
	path p(folder);

	directory_iterator end_itr;

	// cycle through the directory
	for (directory_iterator itr(p); itr != end_itr; ++itr)
	{
		// If it's not a directory, list it. If you want to list directories too, just remove this check.
		if (is_regular_file(itr->path())) {
			std::vector<std::string> strs;
			// assign current file name to current_file and echo it out to the console.
			string current_file = itr->path().string();
			boost::split(strs, current_file, boost::is_any_of("."));

			if (strs[strs.size() - 1] == "lua") {
				luaPaths->push_back(current_file);
			}
		}
		else if (is_directory(itr->path())) {
			string current_file = itr->path().string();
			_iterateDirectory(current_file, luaPaths);
		}
	}
}

void luaHandler::registerLuaFunctions() {
	if (_S == nullptr) {
		gengine::fatalError("lua_State must be initialized to register lua functions");
	}
	errorDefinition->_definitionType = definitionType::ERR;
	errorDefinition->_nameID = "ERROR";

	lua_register(_S, "newMaterial", LUA_MATERIAL_newMaterial);
	lua_register(_S, "newMaterialMod", LUA_MATERIAL_MOD_newMaterialMod);
	lua_register(_S, "newPlacable", LUA_PLACABLE_newPlacableObject);
	lua_register(_S, "newParticle", LUA_PARTICLE_newParticle);
	lua_register(_S, "newParticleEmitter", LUA_PARTICLE_EMITTER_newParticleEmitter);
	lua_register(_S, "newUsableItem", LUA_USABLE_newUsable);
	lua_register(_S, "newProjectile", LUA_PROJECTILE_newProjectile);
	lua_register(_S, "newWorldGenerator", LUA_WORLD_GENERATOR_newWorldGenerator);
	lua_register(_S, "newNPC", LUA_AGENT_newNPC);
	lua_register(_S, "newEvent", LUA_EVENT_newEvent);
	lua_register(_S, "newEffect", LUA_EFFECT_newEffect);

	_registerParticleFunction();
	_registerParticleEmitterFunction();
	_registerPlacableFunction();
	_registerUsableFunction();
	_registerLightFunction();
	_registerSoundFunction();
	_registerTimerFunction();
	_registerInputFunctions();
	_registerProjectileFunction();
	_registerMaterialModFunction();
	_registerTileFunction();
	_registerWorldFunction();
	_registerAgentFunction();
	_registerSolidBodyFunction();
	_registerEventFunction();
	_registerEffectFunction();

	LUA_INPUT_codes["BACKSPACE"] = SDLK_BACKSPACE;
	LUA_INPUT_codes["TAB"] = SDLK_TAB;
	LUA_INPUT_codes["RETURN"] = SDLK_RETURN;
	LUA_INPUT_codes["PAUSE"] = SDLK_PAUSE;
	LUA_INPUT_codes["ESCAPE"] = SDLK_ESCAPE;
	LUA_INPUT_codes["SPACE"] = SDLK_SPACE;
	LUA_INPUT_codes["COMMA"] = SDLK_COMMA;
	LUA_INPUT_codes["MINUS"] = SDLK_MINUS;
	LUA_INPUT_codes["PERIOD"] = SDLK_PERIOD;
	LUA_INPUT_codes["SLASH"] = SDLK_SLASH;
	LUA_INPUT_codes["0"] = SDLK_0;
	LUA_INPUT_codes["1"] = SDLK_1;
	LUA_INPUT_codes["2"] = SDLK_2;
	LUA_INPUT_codes["3"] = SDLK_3;
	LUA_INPUT_codes["4"] = SDLK_4;
	LUA_INPUT_codes["5"] = SDLK_5;
	LUA_INPUT_codes["6"] = SDLK_6;
	LUA_INPUT_codes["7"] = SDLK_7;
	LUA_INPUT_codes["8"] = SDLK_8;
	LUA_INPUT_codes["9"] = SDLK_9;
	LUA_INPUT_codes["COLON"] = SDLK_COLON;
	LUA_INPUT_codes["SEMICOLON"] = SDLK_SEMICOLON;
	LUA_INPUT_codes["EQUALS"] = SDLK_EQUALS;
	LUA_INPUT_codes["LEFTBRACKET"] = SDLK_LEFTBRACKET;
	LUA_INPUT_codes["BACKSLASH"] = SDLK_BACKSLASH;
	LUA_INPUT_codes["RIGHTBRACKET"] = SDLK_RIGHTBRACKET;
	LUA_INPUT_codes["a"] = SDLK_a;
	LUA_INPUT_codes["b"] = SDLK_b;
	LUA_INPUT_codes["c"] = SDLK_c;
	LUA_INPUT_codes["d"] = SDLK_d;
	LUA_INPUT_codes["e"] = SDLK_e;
	LUA_INPUT_codes["f"] = SDLK_f;
	LUA_INPUT_codes["g"] = SDLK_g;
	LUA_INPUT_codes["h"] = SDLK_h;
	LUA_INPUT_codes["i"] = SDLK_i;
	LUA_INPUT_codes["j"] = SDLK_j;
	LUA_INPUT_codes["k"] = SDLK_k;
	LUA_INPUT_codes["l"] = SDLK_l;
	LUA_INPUT_codes["m"] = SDLK_m;
	LUA_INPUT_codes["n"] = SDLK_n;
	LUA_INPUT_codes["o"] = SDLK_o;
	LUA_INPUT_codes["p"] = SDLK_p;
	LUA_INPUT_codes["q"] = SDLK_q;
	LUA_INPUT_codes["r"] = SDLK_r;
	LUA_INPUT_codes["s"] = SDLK_s;
	LUA_INPUT_codes["t"] = SDLK_t;
	LUA_INPUT_codes["u"] = SDLK_u;
	LUA_INPUT_codes["v"] = SDLK_v;
	LUA_INPUT_codes["w"] = SDLK_w;
	LUA_INPUT_codes["x"] = SDLK_x;
	LUA_INPUT_codes["y"] = SDLK_y;
	LUA_INPUT_codes["z"] = SDLK_z;
	LUA_INPUT_codes["KP_DELETE"] = SDLK_DELETE;
	LUA_INPUT_codes["KP_PERIOD"] = SDLK_KP_PERIOD;
	LUA_INPUT_codes["KP_DIVIDE"] = SDLK_KP_DIVIDE;
	LUA_INPUT_codes["KP_MULTIPLY"] = SDLK_KP_MULTIPLY;
	LUA_INPUT_codes["KP_MINUS"] = SDLK_KP_MINUS;
	LUA_INPUT_codes["KP_PLUS"] = SDLK_KP_PLUS;
	LUA_INPUT_codes["KP_ENTER"] = SDLK_KP_ENTER;
	LUA_INPUT_codes["KP_EQUALS"] = SDLK_KP_EQUALS;
	LUA_INPUT_codes["UP"] = SDLK_UP;
	LUA_INPUT_codes["DOWN"] = SDLK_DOWN;
	LUA_INPUT_codes["RIGHT"] = SDLK_RIGHT;
	LUA_INPUT_codes["LEFT"] = SDLK_LEFT;
	LUA_INPUT_codes["INSERT"] = SDLK_INSERT;
	LUA_INPUT_codes["HOME"] = SDLK_HOME;
	LUA_INPUT_codes["END"] = SDLK_END;
	LUA_INPUT_codes["PAGEUP"] = SDLK_PAGEUP;
	LUA_INPUT_codes["PAGEDOWN"] = SDLK_PAGEDOWN;
	LUA_INPUT_codes["F1"] = SDLK_F1;
	LUA_INPUT_codes["F2"] = SDLK_F2;
	LUA_INPUT_codes["F3"] = SDLK_F3;
	LUA_INPUT_codes["F4"] = SDLK_F4;
	LUA_INPUT_codes["F5"] = SDLK_F5;
	LUA_INPUT_codes["F6"] = SDLK_F6;
	LUA_INPUT_codes["F7"] = SDLK_F7;
	LUA_INPUT_codes["F8"] = SDLK_F8;
	LUA_INPUT_codes["F9"] = SDLK_F9;
	LUA_INPUT_codes["F10"] = SDLK_F10;
	LUA_INPUT_codes["F11"] = SDLK_F11;
	LUA_INPUT_codes["F12"] = SDLK_F12;
	LUA_INPUT_codes["F13"] = SDLK_F13;
	LUA_INPUT_codes["F14"] = SDLK_F14;
	LUA_INPUT_codes["F15"] = SDLK_F15;
	LUA_INPUT_codes["RSHIFT"] = SDLK_RSHIFT;
	LUA_INPUT_codes["LSHIFT"] = SDLK_LSHIFT;
	LUA_INPUT_codes["RCTRL"] = SDLK_RCTRL;
	LUA_INPUT_codes["LCTRL"] = SDLK_LCTRL;
	LUA_INPUT_codes["RALT"] = SDLK_RALT;
	LUA_INPUT_codes["LALT"] = SDLK_LALT;
	LUA_INPUT_codes["MOUSE_LEFT"] = SDL_BUTTON_LEFT;
	LUA_INPUT_codes["MOUSE_RIGHT"] = SDL_BUTTON_RIGHT;
}

int luaHandler::LUA_SOUND_playSFX(lua_State *S) {
	if (lua_isnoneornil(S, 3)) {
		gengine::audioEngine::loadSoundEffect(lua_tostring(S, 1)).play(lua_tonumber(S, 2));
	}
	else {
		gengine::audioEngine::loadSoundEffect(lua_tostring(S, 1)).play(lua_tonumber(S, 2), lua_tonumber(S,3));
	}
	return 0;
}

int luaHandler::LUA_SOUND_playMusic(lua_State *S) {
	gengine::audioEngine::loadMusic(lua_tostring(S, 1)).play(lua_tonumber(S, 2));
	return 0;
}

int luaHandler::LUA_SOUND_stopMusic(lua_State *S) {
	gengine::music::stop();
	return 0;
}

int luaHandler::LUA_SOUND_pauseMusic(lua_State *S) {
	gengine::music::pause();
	return 0;
}

int luaHandler::LUA_SOUND_resumeMusic(lua_State *S) {
	gengine::music::pause();
	return 0;
}

int luaHandler::LUA_SOUND_setChannelVolume(lua_State *S) {
	gengine::audioEngine::setChannelVolume(lua_tointeger(S, 1), lua_tointeger(S, 2));
	return 0;
}

int luaHandler::LUA_SOUND_getChannelVolume(lua_State *S) {
	lua_pushinteger(S ,gengine::audioEngine::getChannelVolume(lua_tointeger(S, 1)));
	return 1;
}

void luaHandler::_registerSoundFunction() {
	luaL_Reg soundlib_m[] = {
		{ "playSFX", LUA_SOUND_playSFX },
		{ "playMusic", LUA_SOUND_playMusic },
		{ "stopMusic", LUA_SOUND_stopMusic },
		{ "pauseMusic", LUA_SOUND_pauseMusic },
		{ "resumeMusic", LUA_SOUND_resumeMusic },
		{ "setChannelVolume", LUA_SOUND_setChannelVolume },
		{ "getChannelVolume", LUA_SOUND_getChannelVolume },
		{ NULL, NULL }
	};
	lua_newtable(_S);
	luaL_setfuncs(_S, soundlib_m, 0);
	lua_setglobal(_S, "sound");

}

void luaHandler::_registerInputFunctions() {
	luaL_Reg inputlib_m[] = {
		{ "isKeyDown", LUA_INPUT_isKeyDown },
		{ "wasKeyDown", LUA_INPUT_wasKeyDown },
		{ "keyPressed", LUA_INPUT_keyPressed },
		{ "keyReleased", LUA_INPUT_keyReleased },
		{ NULL, NULL }
	};
	lua_newtable(_S);
	luaL_setfuncs(_S, inputlib_m, 0);
	lua_setglobal(_S, "input");
}

int luaHandler::LUA_INPUT_isKeyDown(lua_State *S) {
	SDL_Keycode code = LUA_INPUT_luaToSDL(lua_tostring(S, 1));
	lua_pushboolean(S,gengine::inputManager::isKeyDown(code));
	return 1;
}
int luaHandler::LUA_INPUT_wasKeyDown(lua_State *S) {
	SDL_Keycode code = LUA_INPUT_luaToSDL(lua_tostring(S, 1));
	lua_pushboolean(S, gengine::inputManager::wasKeyDown(code));
	return 1;
}
int luaHandler::LUA_INPUT_keyPressed(lua_State *S) {
	SDL_Keycode code = LUA_INPUT_luaToSDL(lua_tostring(S, 1));
	lua_pushboolean(S, gengine::inputManager::keyPressed(code));
	return 1;
}
int luaHandler::LUA_INPUT_keyReleased(lua_State *S) {
	SDL_Keycode code = LUA_INPUT_luaToSDL(lua_tostring(S, 1));
	lua_pushboolean(S, gengine::inputManager::keyReleased(code));
	return 1;
}

SDL_Keycode luaHandler::LUA_INPUT_luaToSDL(std::string key)
{
	auto it = LUA_INPUT_codes.find(key);
	if (it != LUA_INPUT_codes.end()) {
		return it->second;
	}
	else {
		return SDLK_UNKNOWN;
	}
}

void luaHandler::loadLua() {
	std::vector<std::string> luaPaths;
	_iterateDirectory("assets", &luaPaths);

	for (int i = 0; i < luaPaths.size(); i++) {
		//Helps shorten the file paths in lua
		std::vector<std::string> strs;
		boost::split(strs, luaPaths[i], boost::is_any_of("\\"));
		currentFolder = "";
		for (int i = 0; i < strs.size() - 1; i++) {
			currentFolder += strs[i];
			currentFolder += "\\";
		}

		luaL_dofile(_S, luaPaths[i].c_str());
	}
}

void luaHandler::setWorld(world* world) {
	_world = world;
}

lua_State* luaHandler::getLuaState() {
	return _S;
}

world* luaHandler::getWorld() {
	return _world;
}

definition* luaHandler::getDefinition(std::string ID) {
	auto it = _definitions.find(ID);
	if (it != _definitions.end()) {
		return it->second;
	}
	else {
		return errorDefinition;
	}
}

bool luaHandler::validateDefinition(definition* def) {
	if (def->getDefinitionType() == definitionType::ERR || def->getNameID() == "ERROR") {
		return false;
	}
	else {
		return true;
	}
}

std::string luaHandler::getPath(std::string smallPath) {
	return "assets/" + smallPath;
}

int luaHandler::getTableInt(std::string tableName, std::string variableName) {
	lua_getglobal(_S, tableName.c_str());		// tableName
	lua_pushstring(_S, variableName.c_str());	// variableName, tableName
	lua_gettable(_S, -2);						// bool, 

	if (!lua_isinteger(_S, -1)) {
		lua_pop(_S, 2);
		return 0;
	}

	int returned = lua_tointeger(_S, -1);

	return returned;
}

bool luaHandler::globalValueExists(std::string tableName) {
	lua_getglobal(_S, tableName.c_str());		// tableName
	return !lua_isnoneornil(_S, -1);
}


float luaHandler::getTableFloat(std::string tableName, std::string variableName) {
	lua_getglobal(_S, tableName.c_str());		// tableName
	lua_pushstring(_S, variableName.c_str());	// variableName, tableName
	lua_gettable(_S, -2);						// bool, 

	if (!lua_isnumber(_S, -1)) {
		lua_pop(_S, 2);
		return 0;
	}

	float returned = lua_tonumber(_S, -1);
	lua_pop(_S, 2);

	return returned;
}

bool luaHandler::getTableBool(std::string tableName, std::string variableName) {
	lua_getglobal(_S, tableName.c_str());		// tableName
	lua_pushstring(_S, variableName.c_str());	// variableName, tableName
	lua_gettable(_S, -2);						// bool, 

	if (!lua_isboolean(_S, -1)) {
		lua_pop(_S, 2);
		return 0;
	}

	bool returned = lua_toboolean(_S, -1);
	lua_pop(_S, 2);

	return returned;
}

std::string luaHandler::getTableString(std::string tableName, std::string variableName) {
	lua_getglobal(_S, tableName.c_str());		// tableName
	lua_pushstring(_S, variableName.c_str());	// variableName, tableName
	lua_gettable(_S, -2);						// bool, 

	if (!lua_isstring(_S, -1)) {
		lua_pop(_S, 2);
		return "";
	}

	std::string returned = lua_tostring(_S, -1);
	lua_pop(_S, 2);

	return returned;
}

glm::vec2 luaHandler::getTableRange(std::string tableName, std::string variableName) {
	lua_getglobal(_S, tableName.c_str());		// tableName
	lua_pushstring(_S, variableName.c_str());	// variableName, tableName
	lua_gettable(_S, -2);						// table, tableName
	if (lua_istable(_S, -1)) {				
		lua_pushnumber(_S, 1);					// 1, table, tableName
		lua_gettable(_S, -2);					// table[1], table, tableName
		lua_pushnumber(_S, 2);					// 2, table[1], table, tableName
		lua_gettable(_S, -3);					// table[2], table[1], table, tableName
		
		glm::vec2 returned = glm::vec2(lua_tonumber(_S, -2), lua_tonumber(_S, -1));
		lua_pop(_S, 1);
		return returned;
	}
	else if(lua_isnumber(_S, -1)){
		float number = lua_tonumber(_S, -1);
		lua_pop(_S, 1);
		return glm::vec2(number, number);
	}
	else if(lua_isnil(_S, -1)){
		return glm::vec2(-1, -1);
	}
	else {
		return glm::vec2(-1, -1);
	}
}

std::vector<float> luaHandler::getTableTableValues(std::string tableName, std::string variableName) {
	lua_getglobal(_S, tableName.c_str());		// tableName
	lua_pushstring(_S, variableName.c_str());	// variableName, tableName
	lua_gettable(_S, -2);						// table, tableName
	if (lua_istable(_S, -1)) {
		std::vector<float> returned;
		lua_pushnil(_S);  /* first key */
		while (lua_next(_S, -2) != 0) {
			returned.push_back(lua_tonumber(_S, -1));
			lua_pop(_S, 1);
		}
		lua_pop(_S, 2);
		return returned;
	}
	else {
		std::vector<float> returned;
		returned.push_back(lua_tonumber(_S, -1));
		lua_pop(_S, 2);
		return returned;
	}
}

std::vector<std::string> luaHandler::getTableTableStrings(std::string tableName, std::string variableName) {
	lua_getglobal(_S, tableName.c_str());		// tableName
	lua_pushstring(_S, variableName.c_str());	// variableName, tableName
	lua_gettable(_S, -2);						// table, tableName
	if (lua_istable(_S, -1)) {
		std::vector<std::string> returned;
		lua_pushnil(_S);  /* first key */
		while (lua_next(_S, -2) != 0) {
			returned.push_back(lua_tostring(_S, -1));
			lua_pop(_S, 1);
		}
		lua_pop(_S, 2);
		return returned;
	}
	else {
		std::vector<std::string> returned;
		returned.push_back(lua_tostring(_S, -1));
		lua_pop(_S, 2);
		return returned;
	}
}

bool luaHandler::_verifyLuaName(std::string name) {
	if (name.size() <= globals::LUA_NAMES_MAX_CHAR) {
		for (auto &it : _definitions) {
			if (name == it.first) {
				return false;
			}
		}
		return true;
	}
	else {
		return false;
	}
}