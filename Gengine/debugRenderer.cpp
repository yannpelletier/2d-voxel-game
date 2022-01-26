#include "debugRenderer.h"

namespace {
	const char* VERT_SRC = R"(#version 140
//The vertex shader operates on each vertex
//input data from the VBO. Each vertex is 2 floats
in vec2 vertexPosition;
in vec4 vertexColor;
out vec2 fragmentPosition;
out vec4 fragmentColor;
uniform mat4 P;
void main() {
    //Set the x,y position on the screen
    gl_Position.xy = (P * vec4(vertexPosition, 0.0, 1.0)).xy;
    //the z position is zero since we are in 2D
    gl_Position.z = 0.0;
    
    //Indicate that the coordinates are normalized
    gl_Position.w = 1.0;
    
    fragmentPosition = vertexPosition;
    
    fragmentColor = vertexColor;
})";

	const char* FRAG_SRC = R"(#version 140
//The fragment shader operates on each pixel in a given polygon
in vec2 fragmentPosition;
in vec4 fragmentColor;
//This is the 3 component float vector that gets outputted to the screen
//for each pixel.
out vec4 color;
void main() {
    color = fragmentColor;
})";
}

namespace gengine {

	debugRenderer::debugRenderer() {
		// Empty
	}

	debugRenderer::~debugRenderer() {
		dispose();
	}

	void debugRenderer::init() {

		// Shader init
		_program.compileShadersFromSource(VERT_SRC, FRAG_SRC);
		_program.addAttribute("vertexPosition");
		_program.addAttribute("vertexColor");
		_program.linkShaders();

		// Set up buffers
		glGenVertexArrays(1, &_vao);
		glGenBuffers(1, &_vbo);
		glGenBuffers(1, &_ibo);

		glBindVertexArray(_vao);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(debugVertex), (void*)offsetof(debugVertex, position));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(debugVertex), (void*)offsetof(debugVertex, color));

		glBindVertexArray(0);
	}

	void debugRenderer::end() {
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		// Orphan the buffer
		glBufferData(GL_ARRAY_BUFFER, _verts.size() * sizeof(debugVertex), nullptr, GL_DYNAMIC_DRAW);
		// Upload the data
		glBufferSubData(GL_ARRAY_BUFFER, 0, _verts.size() * sizeof(debugVertex), _verts.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
		// Orphan the buffer
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
		// Upload the data
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, _indices.size() * sizeof(GLuint), _indices.data());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		_numElements = _indices.size();
		_indices.clear();
		_verts.clear();
	}

	glm::vec2 debugRenderer::rotatePoint(const glm::vec2& pos, float angle) {
		glm::vec2 newv;
		newv.x = pos.x * cos(angle) - pos.y * sin(angle);
		newv.y = pos.x * sin(angle) + pos.y * cos(angle);
		return newv;
	}

	void debugRenderer::drawBox(const glm::vec4& destRect, const color& color, float angle) {

		int i = _verts.size();
		_verts.resize(_verts.size() + 4);

		glm::vec2 halfDims(destRect.z / 2.0f, destRect.w / 2.0f);

		// Get points centered at origin
		glm::vec2 tl(-halfDims.x, halfDims.y);
		glm::vec2 bl(-halfDims.x, -halfDims.y);
		glm::vec2 br(halfDims.x, -halfDims.y);
		glm::vec2 tr(halfDims.x, halfDims.y);

		glm::vec2 positionOffset(destRect.x, destRect.y);

		// Rotate the points
		_verts[i].position = rotatePoint(tl, angle) + halfDims + positionOffset;
		_verts[i + 1].position = rotatePoint(bl, angle) + halfDims + positionOffset;
		_verts[i + 2].position = rotatePoint(br, angle) + halfDims + positionOffset;
		_verts[i + 3].position = rotatePoint(tr, angle) + halfDims + positionOffset;

		for (int j = i; j < i + 4; j++) {
			_verts[j].color = color;
		}

		_indices.reserve(_indices.size() + 8);

		_indices.push_back(i);
		_indices.push_back(i + 1);

		_indices.push_back(i + 1);
		_indices.push_back(i + 2);

		_indices.push_back(i + 2);
		_indices.push_back(i + 3);

		_indices.push_back(i + 3);
		_indices.push_back(i);
	}

	void debugRenderer::drawCircle(const glm::vec2& center, const color& color, float radius) {
		static const int NU_VERTS = 100;
		// Set up vertices
		int start = _verts.size();
		_verts.resize(_verts.size() + NU_VERTS);
		for (int i = 0; i < NU_VERTS; i++) {
			float angle = ((float)i / NU_VERTS) * M_PI * 2.0f;
			_verts[start + i].position.x = cos(angle) * radius + center.x;
			_verts[start + i].position.y = sin(angle) * radius + center.y;
			_verts[start + i].color = color;
		}

		// Set up indices for indexed drawing
		_indices.reserve(_indices.size() + NU_VERTS * 2);
		for (int i = 0; i < NU_VERTS - 1; i++) {
			_indices.push_back(start + i);
			_indices.push_back(start + i + 1);
		}
		_indices.push_back(start + NU_VERTS - 1);
		_indices.push_back(start);
	}

	void debugRenderer::drawLine(const glm::vec2& begin, const glm::vec2 end, const color& color) {

		int i = _verts.size();
		_verts.resize(_verts.size() + 2);

		_verts[i].position = begin;
		_verts[i + 1].position = end;

		for (int j = i; j < i + 2; j++) {
			_verts[j].color = color;
		}

		_indices.reserve(_indices.size() + 2);

		_indices.push_back(i);
		_indices.push_back(i + 1);
	}

	void debugRenderer::render(const glm::mat4& projectionMatrix, float lineWidth) {
		_program.use();

		GLint pUniform = _program.getUniformLocation("P");
		glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

		glLineWidth(lineWidth);
		glBindVertexArray(_vao);
		glDrawElements(GL_LINES, _numElements, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		_program.unuse();
	}

	void debugRenderer::dispose() {
		if (_vao) {
			glDeleteVertexArrays(1, &_vao);
		}
		if (_vbo) {
			glDeleteBuffers(1, &_vbo);
		}
		if (_ibo) {
			glDeleteBuffers(1, &_ibo);
		}
		_program.dispose();
	}
}