#include "particleEmitter.h"
#include "globals.h"

particleEmitter::particleEmitter(std::string nameID)
{
	_nameID = nameID;
	glm::vec2 frequence = luaHandler::getTableRange(_nameID, "frequence");
	minFrequence = (int)(frequence.x);
	maxFrequence = (int)(frequence.y);
	glm::vec2 width = luaHandler::getTableRange(_nameID, "width");
	minWidth = (float)(globals::pixToMet(width.x));
	maxWidth = (float)(globals::pixToMet(width.y));
	glm::vec2 speed = luaHandler::getTableRange(_nameID, "speed");
	minSpeed = (float)(globals::pixToMet(speed.x));
	maxSpeed = (float)(globals::pixToMet(speed.y));
	glm::vec2 angle = luaHandler::getTableRange(_nameID, "angle");
	minAngle = (float)(angle.x);
	maxAngle = (float)(angle.y);
}


particleEmitter::~particleEmitter()
{
}

void particleEmitter::addEmittedParticle(gengine::color color, gengine::particleBatch2D* batch) {
	_particles.push_back(std::make_pair(color, batch));
}

void particleEmitter::update(gengine::camera2D* camera) {
	if (nextDelay == 0) {
		nextDelay = getRandomTime();
	}
	if (enabled) {
		sinceLastEmission += (int)(gengine::timing::dt() * 1000);
		if (camera->isInCameraView(*emitterPosition, dimensions)) {
			if (sinceLastEmission >= nextDelay) {
				emitParticle();
			}
		}
	}
}

void particleEmitter::emitParticle() {
	glm::vec2 dir;
	float bearingDegree;
	if (emitterAngle != nullptr) {
		bearingDegree = *emitterAngle; //get bearing in radians
	}
	else {
		bearingDegree = 0;
	}
	float randomAngle = getRandomAngle() * 0.0174533;//Degree to radian
	float speed = getRandomSpeed();
	dir.x = cos(randomAngle) * speed;
	dir.y = sin(randomAngle) * speed;

	int randomParticle = getRandomParticle();
	float randomWidth = getRandomWidth();

	glm::vec2 randomPosition = getRandomPosition();
	randomPosition.x -= dimensions.x / 2;
	randomPosition.y -= dimensions.y / 2;
	glm::vec2 xVec;
	glm::vec2 yVec;

	xVec.x = cos(bearingDegree) * randomPosition.x;
	xVec.y = sin(bearingDegree) * randomPosition.x;

	yVec.x = cos(bearingDegree + (M_PI / 2)) * randomPosition.y;
	yVec.y = sin(bearingDegree + (M_PI / 2)) * randomPosition.y;

	randomPosition.x = xVec.x + yVec.x;
	randomPosition.y = xVec.y + yVec.y;

	_particles[randomParticle].second->addParticle(randomPosition + glm::vec2(emitterPosition->x, emitterPosition->y) - glm::vec2(randomWidth / 2, randomWidth / 2), dir, gengine::color(_particles[randomParticle].first.r, _particles[randomParticle].first.g, _particles[randomParticle].first.b, _particles[randomParticle].first.a), randomWidth);
	sinceLastEmission = 0;
	nextDelay = getRandomTime();
}

int particleEmitter::getRandomParticle() {
	if (_particles.size() > 1) {
		boost::random::uniform_int_distribution<int> x(0, _particles.size() - 1);
		return x(globals::rng);
	}
	else {
		return 0;
	}
}

glm::vec2 particleEmitter::getRandomPosition() {
	boost::random::uniform_real_distribution<float> x(0, dimensions.x);
	boost::random::uniform_real_distribution<float> y(0, dimensions.y);

	return glm::vec2(x(globals::rng), y(globals::rng));
}

int particleEmitter::getRandomTime() {
	if (minFrequence != maxFrequence) {
		boost::random::uniform_int_distribution<int> returnedTime(minFrequence, maxFrequence);

		return returnedTime(globals::rng);
	}
	else {
		return minFrequence;
	}
}

float particleEmitter::getRandomWidth() {
	if (minWidth != maxWidth) {
		boost::random::uniform_real_distribution<float> widths(minWidth, maxWidth);

		return widths(globals::rng);
	}
	else {
		return minWidth;
	}
}

float particleEmitter::getRandomSpeed() {
	if (minSpeed != maxSpeed) {
		boost::random::uniform_real_distribution<float> speeds(minSpeed, maxSpeed);

		return speeds(globals::rng);
	}
	else {
		return minSpeed;
	}
}

float particleEmitter::getRandomAngle() {
	if (minAngle != maxAngle) {
		boost::random::uniform_real_distribution<float> angles(minAngle, maxAngle);

		return angles(globals::rng);
	}
	else {
		return minAngle;
	}
}

int luaHandler::LUA_PARTICLE_EMITTER_newParticleEmitter(lua_State *S) {
	std::string particleEmitterName = lua_tostring(S, 1);

	if (!_verifyLuaName(particleEmitterName)) {
		return 0;
	}

	lua_getglobal(S, particleEmitterName.c_str());
	lua_pushstring(S, "setProperties");		
	lua_gettable(S, -2);					
	lua_pushvalue(S, -2);				
	lua_pcall(S, 1, 0, 0);
	lua_pop(S, 1);

	return 0;
}

int luaHandler::LUA_PARTICLE_EMITTER_addEmittedParticle(lua_State *S) {
	particleEmitter* p = *((particleEmitter**)lua_touserdata(S, 1));
	particle* p2 = (particle*)getDefinition(lua_tostring(S, 2));
	gengine::color c;

	lua_pushnumber(S, 1);		//1
	lua_gettable(S,3);			//table[1]

	lua_pushnumber(S, 2);		//2, table[1]
	lua_gettable(S, 3);			//table[2], table[1]

	lua_pushnumber(S, 3);
	lua_gettable(S, 3);

	lua_pushnumber(S, 4);
	lua_gettable(S, 3);

	c.r = lua_tointeger(S, -4);
	c.g = lua_tointeger(S, -3);
	c.b = lua_tointeger(S, -2);
	c.a = lua_tointeger(S, -1);
	
	p->addEmittedParticle(c, p2->getParticleBatch());
	return 0;
}

int luaHandler::LUA_PARTICLE_EMITTER_enable(lua_State *S) {
	particleEmitter* p = *((particleEmitter**)lua_touserdata(S, 1));
	p->enabled = true;

	return 0;
}

int luaHandler::LUA_PARTICLE_EMITTER_disable(lua_State *S) {
	particleEmitter* p = *((particleEmitter**)lua_touserdata(S, 1));
	p->enabled = false;

	return 0;
}

void luaHandler::_registerParticleEmitterFunction() {
	luaL_Reg emitterlib_m[] = {
		{ "addEmittedParticle", LUA_PARTICLE_EMITTER_addEmittedParticle },
		{ "enable", LUA_PARTICLE_EMITTER_enable },
		{ "disable", LUA_PARTICLE_EMITTER_disable },
		{ NULL, NULL }
	};

	luaL_newmetatable(_S, "particleEmitter_m");

	lua_pushstring(_S, "__index");
	lua_pushvalue(_S, -2);  /* pushes the metatable*/
	lua_settable(_S, -3);  /* metatable.__index = metatable */

	luaL_setfuncs(_S, emitterlib_m, 0);
}