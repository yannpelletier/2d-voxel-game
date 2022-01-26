#include "particle.h"

particle::particle() {
	_definitionType = definitionType::PARTICLE;
}


void particle::initParticleBatch(int maxParticles, float decayRate, gengine::GLTexture texture, boost::function1<void, gengine::particle2D&> updateFunc) {
	_particleBatch.init(maxParticles, decayRate, texture, 7, updateFunc);
	gengine::particleEngine2D::addParticleBatch(&_particleBatch);
}

void particle::update(gengine::particle2D& particle) {
	particle.position += particle.velocity;

	lua_State* S = luaHandler::getLuaState();
	lua_getglobal(S, _nameID.c_str());		//"_nameID"
	lua_pushstring(S, "update");			//"update", "_nameID"
	lua_gettable(S, -2);					//function update, "_nameID"
	lua_pushvalue(S, -2);					// "self", function update, "_nameID"
	lua_pushlightuserdata(S, &particle);	// particle, "self", function update, "_nameID"
	luaL_getmetatable(S, "particle_m");		// "particle_m" ,particle, "self", function update, "_nameID"
	lua_setmetatable(S, -2);				// "particle_m" ,particle, "self", function update, "_nameID"
	lua_pcall(S, 2, 0,0);
	lua_pop(S, 1);
}

int luaHandler::LUA_PARTICLE_newParticle(lua_State *S) {
	std::string particleName = lua_tostring(S, 1); //Get name of the table

	if (!_verifyLuaName(particleName)) {
		return 0;
	}

	lua_getglobal(S, particleName.c_str()); // "ground"
	lua_pushstring(S, "setProperties");		// "setProperties", "ground"
	lua_gettable(S, -2);					//	function setProperties, "ground"
	lua_pushvalue(S, -2);					//  "ground", function setProperties, "ground"
	lua_pcall(S, 1, 0, 0);
	lua_pop(S, 3);

	particle* newParticle = new particle();
	newParticle->setNameID(particleName);
	newParticle->initParticleBatch(getTableInt(particleName, "maxParticles"),
		getTableFloat(particleName, "decayRate"),
		gengine::ressourceManager::getTexture(getPath(getTableString(particleName, "texture"))),
		boost::bind(&particle::update, *newParticle, _1));
	_definitions.emplace(std::make_pair(particleName, newParticle));

	return 0;
}

int luaHandler::LUA_PARTICLE_setVelocity(lua_State *S) {
	gengine::particle2D* p = (gengine::particle2D*) lua_touserdata(S, 1);
	p->velocity = glm::vec2(globals::pixToMet(lua_tonumber(S, 2)), globals::pixToMet(lua_tonumber(S, 3)));
	return 0;
}

int luaHandler::LUA_PARTICLE_setColor(lua_State *S) {
	gengine::particle2D* p = (gengine::particle2D*) lua_touserdata(S, 1);
	p->color = gengine::color(lua_tonumber(S, 2), lua_tonumber(S, 3), lua_tonumber(S,4), lua_tonumber(S,5));
	return 0;
}

int luaHandler::LUA_PARTICLE_getVelocity(lua_State *S) {
	gengine::particle2D* p = (gengine::particle2D*) lua_touserdata(S, 1);
	lua_pushnumber(S, globals::metToPix(p->velocity.x));
	lua_pushnumber(S, globals::metToPix(p->velocity.y));
	return 2;
}

int luaHandler::LUA_PARTICLE_getColor(lua_State *S) {
	gengine::particle2D* p = (gengine::particle2D*) lua_touserdata(S, 1);
	lua_pushnumber(S, p->color.r);
	lua_pushnumber(S, p->color.g);
	lua_pushnumber(S, p->color.b);
	lua_pushnumber(S, p->color.a);
	return 4;
}

int luaHandler::LUA_PARTICLE_getLifeTime(lua_State *S) {
	gengine::particle2D* p = (gengine::particle2D*) lua_touserdata(S, 1);
	lua_pushnumber(S, p->life);
	return 1;
}

void luaHandler::_registerParticleFunction() {
	luaL_Reg particlelib_m[] = {
		{ "setVelocity", LUA_PARTICLE_setVelocity },
		{ "setColor", LUA_PARTICLE_setColor },
		{ "getVelocity", LUA_PARTICLE_getVelocity },
		{ "getColor", LUA_PARTICLE_getColor },
		{ "getLifeTime", LUA_PARTICLE_getLifeTime },
		{ NULL, NULL }
	};

	luaL_newmetatable(_S, "particle_m");

	lua_pushstring(_S, "__index");
	lua_pushvalue(_S, -2);  /* pushes the metatable*/
	lua_settable(_S, -3);  /* metatable.__index = metatable */

	luaL_setfuncs(_S, particlelib_m, 0);
}