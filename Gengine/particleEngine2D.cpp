#include "particleEngine2D.h"

namespace gengine{

	std::vector<particleBatch2D*> particleEngine2D::_particleBatches;

	void particleEngine2D::addParticleBatch(particleBatch2D* particleBatch) {
		_particleBatches.push_back(particleBatch);
	}

	void particleEngine2D::draw(spriteBatch& spriteBatch) {
		for (auto& it: _particleBatches) {
			it->draw(spriteBatch);
		}
	}

	void particleEngine2D::deleteAllParticles() {
		for (auto& it : _particleBatches) {
			delete it;
		}
	}

	void particleEngine2D::update() {
		for (auto& it : _particleBatches) {
			it->update();
		}
	}
}