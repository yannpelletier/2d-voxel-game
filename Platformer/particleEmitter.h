#pragma once
#include <Gengine/vertex.h>
#include <Gengine/camera2D.h>
#include <Gengine/particleBatch2D.h>
#include <boost/random.hpp>
#include <vector>
#include <random>
#include "luaHandler.h"
#include "particle.h"

class particleEmitter
{
public:
	particleEmitter(std::string nameID);
	~particleEmitter();

	void update(gengine::camera2D* camera);

	void addEmittedParticle(gengine::color color, gengine::particleBatch2D* batch);
	void emitParticle();

	int particleEmitter::getRandomParticle();
	int getRandomTime();
	float getRandomWidth();
	float getRandomSpeed();
	float getRandomAngle();
	glm::vec2 getRandomPosition();

	glm::vec2* emitterPosition;
	float* emitterAngle = nullptr;
	glm::vec2 dimensions;
	int minFrequence;
	int maxFrequence;
	float minSpeed;
	float maxSpeed;
	float minWidth;
	float maxWidth;
	float minAngle;
	float maxAngle;
	int sinceLastEmission = 0;
	int nextDelay = 0;

	std::vector<std::pair<gengine::color, gengine::particleBatch2D*>> _particles;

	std::string _nameID;
	bool enabled = true;

};

