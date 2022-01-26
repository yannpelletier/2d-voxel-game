#include "particleBatch2D.h"

namespace gengine {
	particleBatch2D::particleBatch2D()
	{
	}

	particleBatch2D::~particleBatch2D()
	{
		delete[] _particles;
	}

	void particleBatch2D::init(int maxParticles, float decayRate, GLTexture texture, int drawLayer, boost::function1<void, particle2D&> updateFunc) {
		_updateFunc = updateFunc;
		_maxParticles = maxParticles;
		_decayRate = decayRate;
		_texture = texture;
		_drawLayer = drawLayer;

		_particles = new particle2D[_maxParticles];
	}

	void particleBatch2D::update() {
		for (int i = 0; i < _maxParticles; i++) {
			//Check if the particle is active
			if (_particles[i].life > 0.0f) {
				_particles[i].life -= _decayRate * gengine::timing::dt();
				_updateFunc(_particles[i]);
			}
		}
	}

	void particleBatch2D::draw(spriteBatch& spriteBatch) {
		glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
		for (int i = 0; i < _maxParticles; i++) {
			//Check if the particle is active
			if (_particles[i].life > 0.0f) {
				glm::vec4 destRect(_particles[i].position.x, _particles[i].position.y, _particles[i].width, _particles[i].width);
				spriteBatch.draw(destRect, uvRect, _texture.id, _drawLayer, _particles[i].color);
			}
		}
	}

	int particleBatch2D::findFreeParticle() {
		for (int i = _lastInactiveParticle; i < _maxParticles; i++) {
			if (_particles[i].life <= 0.0f) {
				_lastInactiveParticle = i;
				return i;
			}
		}
		for (int i = 0; i < _lastInactiveParticle; i++) {
			_lastInactiveParticle = i;
			return i;
		}

		//If no free particle is found, overwrite the first one
		_lastInactiveParticle = 0;
		return 0;
	}

	void particleBatch2D::addParticle(const glm::vec2& position, const glm::vec2& velocity, const gengine::color& color, float width) {
		findFreeParticle();

		int particleIndex = _lastInactiveParticle;

		_particles[particleIndex].life = 1.0f;
		_particles[particleIndex].position = position;
		_particles[particleIndex].velocity = velocity;
		_particles[particleIndex].color = color;
		_particles[particleIndex].width = width;
	}

}