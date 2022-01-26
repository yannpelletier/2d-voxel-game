#pragma once
#include <glm/glm.hpp>
#include <boost/function.hpp>
#include "vertex.h"
#include "timing.h"
#include "spriteBatch.h"
#include "GLTexture.h"

namespace gengine {

	class particle2D {
	public:
		glm::vec2 position = glm::vec2(0.0f);
		glm::vec2 velocity = glm::vec2(0.0f);
		gengine::color color;
		float width;
		float life = 0.0f;
	};

	inline void defaultParticleUpdate(particle2D& particle) {
		particle.position += particle.velocity * gengine::timing::dt();
	}

	class particleBatch2D
	{
	public:
		particleBatch2D();
		~particleBatch2D();

		void init(int maxParticles, float decayRate, GLTexture texture, int drawLayer, boost::function1<void, particle2D&> updateFunc = defaultParticleUpdate);

		void update();

		void draw(spriteBatch& spriteBatch);

		void addParticle(const glm::vec2& position, const glm::vec2& velocity, const gengine::color& color, float width);

	private:
		int findFreeParticle();

		boost::function<void(particle2D&)> _updateFunc;
		float _decayRate;
		particle2D* _particles = nullptr;
		int _maxParticles = 0;
		int _lastInactiveParticle = 0;
		GLTexture _texture;
		int _drawLayer;
	};

}