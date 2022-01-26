 #pragma once
#include <Gengine/particleEngine2D.h>
#include <Gengine/particleBatch2D.h>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "definition.h"
#include "globals.h"

class particle: public definition
{
public:
	particle();

	void initParticleBatch(int maxParticles, float decayRate, gengine::GLTexture texture, boost::function1<void, gengine::particle2D&> updateFunc);
	void update(gengine::particle2D& particle);

	gengine::particleBatch2D* getParticleBatch() { return &_particleBatch; }
private:
	gengine::particleBatch2D _particleBatch;
};
