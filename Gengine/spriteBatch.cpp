#include "spriteBatch.h"
#include <math.h>

namespace gengine{
	glyph::glyph(const glm::vec4 &destRect, const glm::fvec4 &uvRect, std::string texturePath, float Depth, const color &color, bool centered) {
		texture = ressourceManager::getTexture(texturePath).id;
		depth = Depth;

		if (centered == false) {
			topLeft.color = color;
			topLeft.setPosition(destRect.x, destRect.y + destRect.w);
			topLeft.setUv(uvRect.x, uvRect.y + uvRect.w);

			bottomLeft.color = color;
			bottomLeft.setPosition(destRect.x, destRect.y);
			bottomLeft.setUv(uvRect.x, uvRect.y);

			topRight.color = color;
			topRight.setPosition(destRect.x + destRect.z, destRect.y + destRect.w);
			topRight.setUv(uvRect.x + uvRect.z, uvRect.y + uvRect.w);

			bottomRight.color = color;
			bottomRight.setPosition(destRect.x + destRect.z, destRect.y);
			bottomRight.setUv(uvRect.x + uvRect.z, uvRect.y);
		}
		else {
			topLeft.color = color;
			topLeft.setPosition(destRect.x - (destRect.z / 2), destRect.y + (destRect.w / 2));
			topLeft.setUv(uvRect.x, uvRect.y + uvRect.w);

			bottomLeft.color = color;
			bottomLeft.setPosition(destRect.x - (destRect.z / 2), destRect.y - (destRect.w / 2));
			bottomLeft.setUv(uvRect.x, uvRect.y);

			topRight.color = color;
			topRight.setPosition(destRect.x + (destRect.z / 2), destRect.y + (destRect.w / 2));
			topRight.setUv(uvRect.x + uvRect.z, uvRect.y + uvRect.w);

			bottomRight.color = color;
			bottomRight.setPosition(destRect.x + (destRect.z / 2), destRect.y - (destRect.w / 2));
			bottomRight.setUv(uvRect.x + uvRect.z, uvRect.y);
		}

	}

	glyph::glyph(const glm::vec4 &destRect, const glm::fvec4 &uvRect, GLuint textureID, float Depth, const color &color, bool centered) {
		texture = textureID;
		depth = Depth;

		if (centered == false) {
			topLeft.color = color;
			topLeft.setPosition(destRect.x, destRect.y + destRect.w);
			topLeft.setUv(uvRect.x, uvRect.y + uvRect.w);

			bottomLeft.color = color;
			bottomLeft.setPosition(destRect.x, destRect.y);
			bottomLeft.setUv(uvRect.x, uvRect.y);

			topRight.color = color;
			topRight.setPosition(destRect.x + destRect.z, destRect.y + destRect.w);
			topRight.setUv(uvRect.x + uvRect.z, uvRect.y + uvRect.w);

			bottomRight.color = color;
			bottomRight.setPosition(destRect.x + destRect.z, destRect.y);
			bottomRight.setUv(uvRect.x + uvRect.z, uvRect.y);
		}
		else {
			topLeft.color = color;
			topLeft.setPosition(destRect.x - (destRect.z / 2), destRect.y + (destRect.w / 2));
			topLeft.setUv(uvRect.x, uvRect.y + uvRect.w);

			bottomLeft.color = color;
			bottomLeft.setPosition(destRect.x - (destRect.z / 2), destRect.y - (destRect.w / 2));
			bottomLeft.setUv(uvRect.x, uvRect.y);

			topRight.color = color;
			topRight.setPosition(destRect.x + (destRect.z / 2), destRect.y + (destRect.w / 2));
			topRight.setUv(uvRect.x + uvRect.z, uvRect.y + uvRect.w);

			bottomRight.color = color;
			bottomRight.setPosition(destRect.x + (destRect.z / 2), destRect.y - (destRect.w / 2));
			bottomRight.setUv(uvRect.x + uvRect.z, uvRect.y);
		}
	}

	glyph::glyph(const glm::vec4 &destRect, const glm::fvec4 &uvRect, std::string texturePath, float Depth, const color &color, float angle, bool centered) {
		texture = ressourceManager::getTexture(texturePath).id;
		depth = Depth;

		glm::vec2 halfDims(destRect.z / 2, destRect.w / 2);

		//Get points centered at origin
		glm::vec2 tl(-halfDims.x, halfDims.y);
		glm::vec2 tr(halfDims);
		glm::vec2 bl(-halfDims);
		glm::vec2 br(halfDims.x, -halfDims.y);

		if (centered == false) {
			//Rotate the points
			tl = rotatePoint(tl, angle) + halfDims;
			tr = rotatePoint(tr, angle) + halfDims;
			bl = rotatePoint(bl, angle) + halfDims;
			br = rotatePoint(br, angle) + halfDims;
		}
		else {
			//Rotate the points
			tl = rotatePoint(tl, angle);
			tr = rotatePoint(tr, angle);
			bl = rotatePoint(bl, angle);
			br = rotatePoint(br, angle);
		}

		topLeft.color = color;
		topLeft.setPosition(destRect.x + tl.x, destRect.y + tl.y);
		topLeft.setUv(uvRect.x, uvRect.y + uvRect.w);

		bottomLeft.color = color;
		bottomLeft.setPosition(destRect.x + bl.x, destRect.y + bl.y);
		bottomLeft.setUv(uvRect.x, uvRect.y);

		topRight.color = color;
		topRight.setPosition(destRect.x + tr.x, destRect.y + tr.y);
		topRight.setUv(uvRect.x + uvRect.z, uvRect.y + uvRect.w);

		bottomRight.color = color;
		bottomRight.setPosition(destRect.x + br.x, destRect.y + br.y);
		bottomRight.setUv(uvRect.x + uvRect.z, uvRect.y);
	}

	glyph::glyph(const glm::vec4 &destRect, const glm::fvec4 &uvRect, GLuint textureID, float Depth, const color &color, float angle, bool centered) {
		texture = textureID;
		depth = Depth;

		glm::vec2 halfDims(destRect.z / 2, destRect.w / 2);

		//Get points centered at origin
		glm::vec2 tl(-halfDims.x, halfDims.y);
		glm::vec2 tr(halfDims);
		glm::vec2 bl(-halfDims);
		glm::vec2 br(halfDims.x, -halfDims.y);

		if (centered == false) {
			//Rotate the points
			tl = rotatePoint(tl, angle) + halfDims;
			tr = rotatePoint(tr, angle) + halfDims;
			bl = rotatePoint(bl, angle) + halfDims;
			br = rotatePoint(br, angle) + halfDims;
		}
		else {
			//Rotate the points
			tl = rotatePoint(tl, angle);
			tr = rotatePoint(tr, angle);
			bl = rotatePoint(bl, angle);
			br = rotatePoint(br, angle);
		}

		topLeft.color = color;
		topLeft.setPosition(destRect.x + tl.x, destRect.y + tl.y);
		topLeft.setUv(uvRect.x, uvRect.y + uvRect.w);

		bottomLeft.color = color;
		bottomLeft.setPosition(destRect.x + bl.x, destRect.y + bl.y);
		bottomLeft.setUv(uvRect.x, uvRect.y);

		topRight.color = color;
		topRight.setPosition(destRect.x + tr.x, destRect.y + tr.y);
		topRight.setUv(uvRect.x + uvRect.z, uvRect.y + uvRect.w);

		bottomRight.color = color;
		bottomRight.setPosition(destRect.x + br.x, destRect.y + br.y);
		bottomRight.setUv(uvRect.x + uvRect.z, uvRect.y);
	}

	glyph::glyph(const glm::vec4 &destRect, const glm::fvec4 &uvRect, std::string texturePath, float Depth, const color &color, glm::vec2& direction, bool centered) {
		texture = ressourceManager::getTexture(texturePath).id;
		depth = Depth;

		glm::vec2 right(1.0f, 0.0f);
		float angle = acos(glm::dot(right, direction));
		if (direction.y < 0) angle = -angle;

		glm::vec2 halfDims(destRect.z / 2, destRect.w / 2);

		//Get points centered at origin
		glm::vec2 tl(-halfDims.x, halfDims.y);
		glm::vec2 tr(halfDims);
		glm::vec2 bl(-halfDims);
		glm::vec2 br(halfDims.x, -halfDims.y);

		if (centered == false) {
			//Rotate the points
			tl = rotatePoint(tl, angle) + halfDims;
			tr = rotatePoint(tr, angle) + halfDims;
			bl = rotatePoint(bl, angle) + halfDims;
			br = rotatePoint(br, angle) + halfDims;
		}
		else {
			//Rotate the points
			tl = rotatePoint(tl, angle);
			tr = rotatePoint(tr, angle);
			bl = rotatePoint(bl, angle);
			br = rotatePoint(br, angle);
		}

		topLeft.color = color;
		topLeft.setPosition(destRect.x + tl.x, destRect.y + tl.y);
		topLeft.setUv(uvRect.x, uvRect.y + uvRect.w);

		bottomLeft.color = color;
		bottomLeft.setPosition(destRect.x + bl.x, destRect.y + bl.y);
		bottomLeft.setUv(uvRect.x, uvRect.y);

		topRight.color = color;
		topRight.setPosition(destRect.x + tr.x, destRect.y + tr.y);
		topRight.setUv(uvRect.x + uvRect.z, uvRect.y + uvRect.w);

		bottomRight.color = color;
		bottomRight.setPosition(destRect.x + br.x, destRect.y + br.y);
		bottomRight.setUv(uvRect.x + uvRect.z, uvRect.y);
	}

	glyph::glyph(const glm::vec4 &destRect, const glm::fvec4 &uvRect, GLuint textureID, float Depth, const color &color, glm::vec2& direction, bool centered) {
		texture = textureID;
		depth = Depth;

		glm::vec2 right(1.0f, 0.0f);
		float angle = acos(glm::dot(right, direction));
		if (direction.y < 0) angle = -angle;

		glm::vec2 halfDims(destRect.z / 2, destRect.w / 2);

		//Get points centered at origin
		glm::vec2 tl(-halfDims.x, halfDims.y);
		glm::vec2 tr(halfDims);
		glm::vec2 bl(-halfDims);
		glm::vec2 br(halfDims.x, -halfDims.y);

		if (centered == false) {
			//Rotate the points
			tl = rotatePoint(tl, angle) + halfDims;
			tr = rotatePoint(tr, angle) + halfDims;
			bl = rotatePoint(bl, angle) + halfDims;
			br = rotatePoint(br, angle) + halfDims;
		}
		else {
			//Rotate the points
			tl = rotatePoint(tl, angle);
			tr = rotatePoint(tr, angle);
			bl = rotatePoint(bl, angle);
			br = rotatePoint(br, angle);
		}

		topLeft.color = color;
		topLeft.setPosition(destRect.x + tl.x, destRect.y + tl.y);
		topLeft.setUv(uvRect.x, uvRect.y + uvRect.w);

		bottomLeft.color = color;
		bottomLeft.setPosition(destRect.x + bl.x, destRect.y + bl.y);
		bottomLeft.setUv(uvRect.x, uvRect.y);

		topRight.color = color;
		topRight.setPosition(destRect.x + tr.x, destRect.y + tr.y);
		topRight.setUv(uvRect.x + uvRect.z, uvRect.y + uvRect.w);

		bottomRight.color = color;
		bottomRight.setPosition(destRect.x + br.x, destRect.y + br.y);
		bottomRight.setUv(uvRect.x + uvRect.z, uvRect.y);
	}

	glyph::glyph(std::vector<glm::vec2>& vertices, const glm::fvec4 &uvRect, std::string texturePath, float Depth, const color &color) {
		texture = ressourceManager::getTexture(texturePath).id;
		depth = Depth;

		topLeft.color = color;
		topLeft.setPosition(vertices[0].x, vertices[0].y);
		topLeft.setUv(uvRect.x, uvRect.y + uvRect.w);

		bottomLeft.color = color;
		bottomLeft.setPosition(vertices[1].x, vertices[1].y);
		bottomLeft.setUv(uvRect.x, uvRect.y);

		topRight.color = color;
		topRight.setPosition(vertices[2].x, vertices[2].y);
		topRight.setUv(uvRect.x + uvRect.z, uvRect.y + uvRect.w);

		bottomRight.color = color;
		bottomRight.setPosition(vertices[3].x, vertices[3].y);
		bottomRight.setUv(uvRect.x + uvRect.z, uvRect.y);

	}

	glyph::glyph(std::vector<glm::vec2>& vertices, const glm::fvec4 &uvRect, GLuint textureID, float Depth, const color &color) {
		texture = textureID;
		depth = Depth;

		topLeft.color = color;
		topLeft.setPosition(vertices[0].x, vertices[0].y);
		topLeft.setUv(uvRect.x, uvRect.y + uvRect.w);

		bottomLeft.color = color;
		bottomLeft.setPosition(vertices[1].x, vertices[1].y);
		bottomLeft.setUv(uvRect.x, uvRect.y);

		topRight.color = color;
		topRight.setPosition(vertices[2].x, vertices[2].y);
		topRight.setUv(uvRect.x + uvRect.z, uvRect.y + uvRect.w);

		bottomRight.color = color;
		bottomRight.setPosition(vertices[3].x, vertices[3].y);
		bottomRight.setUv(uvRect.x + uvRect.z, uvRect.y);

	}

	glm::vec2 glyph::rotatePoint(glm::vec2 pos, float angle) {
		glm::vec2 newVector;
		newVector.x = pos.x * cos(angle) - pos.y * sin(angle);
		newVector.y = pos.x * sin(angle) + pos.y * cos(angle);
		return newVector;
	}

	spriteBatch::spriteBatch() : _vbo(0), _vao(0)
	{
	}


	spriteBatch::~spriteBatch()
	{
	}
	
	void spriteBatch::init(){
		_createVertexArray();
	}

	//Call before any draw statement
	void spriteBatch::begin(glyphSortType sortType){
		_sortType = sortType;
		_renderBatches.clear();
		_glyphs.clear();
	}
	//Call after any draw statement
	void spriteBatch::end(){
		_glyphPointers.resize(_glyphs.size());
		for (int i = 0; i < _glyphs.size(); i++) {
			_glyphPointers[i] = &_glyphs[i];
		}

		_sortGlyphs();
		createRenderBatches();
	}

	void spriteBatch::drawPoints(std::vector<glm::vec2>& vertices, const glm::fvec4 &uvRect, std::string texturePath, float depth, const color &color) {
		_glyphs.emplace_back(vertices, uvRect, texturePath, depth, color);
	}
	void spriteBatch::drawPoints(std::vector<glm::vec2>& vertices, const glm::fvec4 &uvRect, GLuint textureID, float depth, const color &color) {
		_glyphs.emplace_back(vertices, uvRect, textureID, depth, color);
	}
	//Call for each sprite you want to draw between begin and end function
	void spriteBatch::draw(const glm::vec4 &destRect, const glm::fvec4 &uvRect, std::string texturePath, float depth, const color &color){
		_glyphs.emplace_back(destRect, uvRect, texturePath, depth, color, false);
	}
	void spriteBatch::draw(const glm::vec4 &destRect, const glm::fvec4 &uvRect, GLuint textureID, float depth, const color &color) {
		_glyphs.emplace_back(destRect, uvRect, textureID, depth, color, false);
	}
	void spriteBatch::draw(const glm::vec4 &destRect, const glm::fvec4 &uvRect, std::string texturePath, float depth, const color &color, float angle) {
		_glyphs.emplace_back(destRect, uvRect, texturePath, depth, color, angle, false);
	}
	void spriteBatch::draw(const glm::vec4 &destRect, const glm::fvec4 &uvRect, GLuint textureID, float depth, const color &color, float angle) {
		_glyphs.emplace_back(destRect, uvRect, textureID, depth, color, angle, false);
	}
	void spriteBatch::draw(const glm::vec4 &destRect, const glm::fvec4 &uvRect, GLuint textureID, float depth, const color &color, glm::vec2& direction) {
		_glyphs.emplace_back(destRect, uvRect, textureID, depth, color, direction, false);
	}
	void spriteBatch::draw(const glm::vec4 &destRect, const glm::fvec4 &uvRect, std::string texturePath, float depth, const color &color, glm::vec2& direction) {
		_glyphs.emplace_back(destRect, uvRect, texturePath, depth, color, direction, false);
	}
	void spriteBatch::drawCentered(const glm::vec4 &destRect, const glm::fvec4 &uvRect, std::string texturePath, float depth, const color &color) {
		_glyphs.emplace_back(destRect, uvRect, texturePath, depth, color, true);
	}
	void spriteBatch::drawCentered(const glm::vec4 &destRect, const glm::fvec4 &uvRect, GLuint textureID, float depth, const color &color) {
		_glyphs.emplace_back(destRect, uvRect, textureID, depth, color, true);
	}
	void spriteBatch::drawCentered(const glm::vec4 &destRect, const glm::fvec4 &uvRect, std::string texturePath, float depth, const color &color, float angle) {
		_glyphs.emplace_back(destRect, uvRect, texturePath, depth, color, angle, true);
	}
	void spriteBatch::drawCentered(const glm::vec4 &destRect, const glm::fvec4 &uvRect, GLuint textureID, float depth, const color &color, float angle) {
		_glyphs.emplace_back(destRect, uvRect, textureID, depth, color, angle, true);
	}
	void spriteBatch::drawCentered(const glm::vec4 &destRect, const glm::fvec4 &uvRect, GLuint textureID, float depth, const color &color, glm::vec2& direction) {
		_glyphs.emplace_back(destRect, uvRect, textureID, depth, color, direction, true);
	}
	void spriteBatch::drawCentered(const glm::vec4 &destRect, const glm::fvec4 &uvRect, std::string texturePath, float depth, const color &color, glm::vec2& direction) {
		_glyphs.emplace_back(destRect, uvRect, texturePath, depth, color, direction, true);
	}

	void spriteBatch::renderBatches(){
		glBindVertexArray(_vao);

		for (int i = 0; i < _renderBatches.size(); i++){
			glBindTexture(GL_TEXTURE_2D, _renderBatches[i].texture);

			glDrawArrays(GL_TRIANGLES, _renderBatches[i].offset, _renderBatches[i].numVertices);
		}

		glBindVertexArray(0);
	}

	void spriteBatch::createRenderBatches(){
		std::vector<vertex> vertices;
		vertices.resize(_glyphPointers.size() * 6);
		if (_glyphPointers.empty()){
			return;
		}

		int offset = 0;
		int cv = 0; //Current vertex

		_renderBatches.emplace_back(offset, 6, _glyphPointers[0]->texture);
		vertices[cv++] = _glyphPointers[0]->topLeft;
		vertices[cv++] = _glyphPointers[0]->bottomLeft;
		vertices[cv++] = _glyphPointers[0]->bottomRight;
		vertices[cv++] = _glyphPointers[0]->bottomRight;
		vertices[cv++] = _glyphPointers[0]->topRight;
		vertices[cv++] = _glyphPointers[0]->topLeft;
		offset += 6;

		//Current glyph
		for (int cg = 1; cg < _glyphPointers.size(); cg++){
			//If the texture of the last glyph is not the same as the current texture, create a new renderBatch
			if (_glyphPointers[cg]->texture != _glyphPointers[cg - 1]->texture){
				_renderBatches.emplace_back(offset, 6, _glyphPointers[cg]->texture);
			}
			//Simply add vertices to render
			else{
				_renderBatches.back().numVertices += 6;
			}
			vertices[cv++] = _glyphPointers[cg]->topLeft;
			vertices[cv++] = _glyphPointers[cg]->bottomLeft;
			vertices[cv++] = _glyphPointers[cg]->bottomRight;
			vertices[cv++] = _glyphPointers[cg]->bottomRight;
			vertices[cv++] = _glyphPointers[cg]->topRight;
			vertices[cv++] = _glyphPointers[cg]->topLeft;
			offset += 6;
		}


		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		//Orphan the buffer
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), nullptr, GL_DYNAMIC_DRAW);
		//Upload the data
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(vertex), vertices.data());

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void spriteBatch::_createVertexArray(){

		if (_vao == 0){
			glGenVertexArrays(1, &_vao);
		}
		glBindVertexArray(_vao);

		if (_vbo == 0){
			glGenBuffers(1, &_vbo);
		}
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);


		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		//This is the position attribute pointer
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, position));
		//This is the color attribute pointer
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(vertex), (void*)offsetof(vertex, color));
		//This is the UV attribute pointer
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, uv));

		glBindVertexArray(0);
	}

	void spriteBatch::_sortGlyphs(){
		switch (_sortType){
		case NONE:
			break;
		case BACK_TO_FRONT:
			std::stable_sort(_glyphPointers.begin(),_glyphPointers.end(),_compareBackToFront);
			break;
		case FRONT_TO_BACK:
			std::stable_sort(_glyphPointers.begin(),_glyphPointers.end(),_compareFrontToBack);
			break;
		case TEXTURE:
			std::stable_sort(_glyphPointers.begin(),_glyphPointers.end(),_compareTexture);
			break;
		}
	}
	bool spriteBatch::_compareBackToFront(glyph* a, glyph* b){
		return(a->depth > b->depth);
	}
	bool spriteBatch::_compareFrontToBack(glyph* a, glyph* b){
		return(a->depth < b->depth);
	}
	bool spriteBatch::_compareTexture(glyph* a, glyph* b){
		return(a->texture < b->texture);
	}
}