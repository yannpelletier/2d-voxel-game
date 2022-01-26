#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <Gengine/vertex.h>
#include <GL/glew.h>
#include <algorithm>
#include <math.h>

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

namespace gengine {

	struct meshVertex {
		coords position;

		color color;

		UV uv;

		int depth;

		void setPosition(float x, float y) {
			position.x = x;
			position.y = y;
		}

		void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
			color.r = r;
			color.g = g;
			color.b = b;
			color.a = a;
		}

		void setUv(float u, float v) {
			uv.u = u;
			uv.v = v;
		}

		double angle;
	};

	class meshBatch
	{
	public:
		void init();

		void begin();
		void end();

		void draw(std::vector<glm::vec2>* points, glm::vec2& center, glm::vec2& dimensions/* will allow to find the uv of the vertices*/, gengine::color& color);

		void _createVertexArray();

	private:
		GLuint _vbo;
		GLuint _vao;

		static bool compareMeshes(const meshVertex &lhs, const meshVertex &rhs);

		std::vector<vertex> _renderVertices;
	};
}

