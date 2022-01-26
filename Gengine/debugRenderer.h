#pragma once
#include <Gengine/vertex.h>
#include "Gengine/GLSLProgram.h"
#include "glm/glm.hpp"

namespace gengine {

	class debugRenderer
	{
	public:
		debugRenderer();
		~debugRenderer();

		void init();
		void end();

		glm::vec2 rotatePoint(const glm::vec2& pos, float angle);
		void drawBox(const glm::vec4& destRect, const color& color, float angle);
		void drawCircle(const glm::vec2& center, const color& color, float radius);
		void drawLine(const glm::vec2& begin, const glm::vec2 end, const color& color);
		void render(const glm::mat4& projectionMatrix, float lineWidth);
		void dispose();

		struct debugVertex{
			glm::vec2 position;
			color color;
		};
	private:
		gengine::GLSLProgram _program;
		std::vector<debugVertex> _verts;
		std::vector<GLuint> _indices;
		GLuint _vbo = 0, _vao = 0, _ibo = 0;
		int _numElements = 0;

	};

}