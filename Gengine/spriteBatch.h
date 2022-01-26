#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "GLTexture.h"
#include <vector>
#include <algorithm>
#include "vertex.h"
#include "ressourceManager.h"

namespace gengine{

	enum  glyphSortType{
		NONE, 
		FRONT_TO_BACK,
		BACK_TO_FRONT,
		TEXTURE
	};

	class glyph{
	public:
		glyph(){}
		glyph(const glm::vec4 &destRect, const glm::fvec4 &uvRect, std::string texturePath, float Depth, const color &color, bool centered);
		glyph(const glm::vec4 &destRect, const glm::fvec4 &uvRect, GLuint textureID, float Depth, const color &color, bool centered);
		glyph(const glm::vec4 &destRect, const glm::fvec4 &uvRect, std::string texturePath, float Depth, const color &color, float angle, bool centered);
		glyph(const glm::vec4 &destRect, const glm::fvec4 &uvRect, GLuint textureID, float Depth, const color &color, float angle, bool centered);
		glyph(const glm::vec4 &destRect, const glm::fvec4 &uvRect, std::string texturePath, float Depth, const color &color, glm::vec2& direction, bool centered);
		glyph(const glm::vec4 &destRect, const glm::fvec4 &uvRect, GLuint textureID, float Depth, const color &color, glm::vec2& direction, bool centered);
		glyph(std::vector<glm::vec2>& vertices, const glm::fvec4 &uvRect, GLuint textureID, float Depth, const color &color);
		glyph(std::vector<glm::vec2>& vertices, const glm::fvec4 &uvRect, std::string texturePath, float Depth, const color &color);


		GLuint texture;
		float depth;

		gengine::vertex topLeft;
		gengine::vertex bottomLeft;
		gengine::vertex topRight;
		gengine::vertex bottomRight;
	private:
		glm::vec2 rotatePoint(glm::vec2 pos, float angle);
	};

	class renderBatch{
	public:
		renderBatch(GLuint offset, GLuint numVertices, GLuint texture) :
			offset(offset),
			numVertices(numVertices),
			texture(texture){
		}
		GLuint offset;
		GLuint numVertices;
		GLuint texture;
	};

	class spriteBatch
	{
	public:
		spriteBatch();
		~spriteBatch();

		void init();

		void begin(glyphSortType sortType = TEXTURE);
		void end();

		void drawPoints(std::vector<glm::vec2>& vertices, const glm::fvec4 &uvRect, std::string texturePath, float depth, const color &color);
		void drawPoints(std::vector<glm::vec2>& vertices, const glm::fvec4 &uvRect, GLuint textureID, float depth, const color &color);
		void draw(const glm::vec4 &destRect, const glm::fvec4 &uvRect, std::string texturePath, float depth, const color &color);
		void draw(const glm::vec4 &destRect, const glm::fvec4 &uvRect, GLuint textureID, float depth, const color &color);
		void draw(const glm::vec4 &destRect, const glm::fvec4 &uvRect, std::string texturePath, float depth, const color &color, float angle);
		void draw(const glm::vec4 &destRect, const glm::fvec4 &uvRect, GLuint textureID, float depth, const color &color, float angle);
		void draw(const glm::vec4 &destRect, const glm::fvec4 &uvRect, std::string texturePath, float depth, const color &color, glm::vec2& direction);
		void draw(const glm::vec4 &destRect, const glm::fvec4 &uvRect, GLuint textureID, float depth, const color &color, glm::vec2& direction);
		void drawCentered(const glm::vec4 &destRect, const glm::fvec4 &uvRect, std::string texturePath, float depth, const color &color);
		void drawCentered(const glm::vec4 &destRect, const glm::fvec4 &uvRect, GLuint textureID, float depth, const color &color);
		void drawCentered(const glm::vec4 &destRect, const glm::fvec4 &uvRect, std::string texturePath, float depth, const color &color, float angle);
		void drawCentered(const glm::vec4 &destRect, const glm::fvec4 &uvRect, GLuint textureID, float depth, const color &color, float angle);
		void drawCentered(const glm::vec4 &destRect, const glm::fvec4 &uvRect, std::string texturePath, float depth, const color &color, glm::vec2& direction);
		void drawCentered(const glm::vec4 &destRect, const glm::fvec4 &uvRect, GLuint textureID, float depth, const color &color, glm::vec2& direction);

		void renderBatches();
	private:
		void createRenderBatches();
		void _createVertexArray();
		void _sortGlyphs();

		static bool _compareBackToFront(glyph* a, glyph* b);
		static bool _compareFrontToBack(glyph* a, glyph* b);
		static bool _compareTexture(glyph* a, glyph* b);


		glyphSortType _sortType;

		GLuint _vbo;
		GLuint _vao;

		std::vector<glyph> _glyphs;
		std::vector<glyph*> _glyphPointers;
		std::vector<renderBatch> _renderBatches;
	};

}