#pragma once

#include "particleBatch2D.h"
#include "spriteBatch.h"
#include <vector>

namespace gengine {

	class particleBatch2D;
	class spriteBatch;


	class particleEngine2D
	{
	public:
		static void addParticleBatch(particleBatch2D* particleBatch);

		static void update();

		static void draw(spriteBatch& spriteBatch);

		static void deleteAllParticles();

	private:
		static std::vector<particleBatch2D*> _particleBatches;
	};

}