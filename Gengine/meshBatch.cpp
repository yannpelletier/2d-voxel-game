#include "meshBatch.h"
#include <iostream>

namespace gengine {

	void meshBatch::init() {
		_createVertexArray();
	}

	//Call before any draw statement
	void meshBatch::begin() {
		_renderVertices.clear();
	}
	//Call after any draw statement
	void meshBatch::end() {
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		//Orphan the buffer
		glBufferData(GL_ARRAY_BUFFER, _renderVertices.size() * sizeof(vertex), nullptr, GL_DYNAMIC_DRAW);
		//Upload the data
		glBufferSubData(GL_ARRAY_BUFFER, 0, _renderVertices.size() * sizeof(vertex), _renderVertices.data());

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(_vao);

		glDrawArrays(GL_TRIANGLES, 0, _renderVertices.size());

		glBindVertexArray(0);
	}

	bool meshBatch::compareMeshes(const meshVertex &lhs, const meshVertex &rhs) {
		return (lhs.angle < rhs.angle);
	}

	void meshBatch::draw(std::vector<glm::vec2>* points, glm::vec2& center, glm::vec2& dimensions/* will allow to find the uv of the vertices*/, gengine::color& color) {
		std::vector<meshVertex> verticesToOrder;
		glm::vec2 bottomLeft = glm::vec2(center.x - dimensions.x / 2, center.y - dimensions.y / 2);

		//Allows to order the vertices by angle
		for (int i = 0; i < points->size(); i++) {
			meshVertex newVertex;
			glm::vec2 vertPos = glm::vec2((*points)[i].x, (*points)[i].y);
			glm::vec2 vertUV = glm::vec2((vertPos.x - bottomLeft.x) / dimensions.x, (vertPos.y - bottomLeft.y) / dimensions.y);

			newVertex.setPosition(vertPos.x, vertPos.y);
			newVertex.setUv(vertUV.x,vertUV.y);
			newVertex.color = color;
			newVertex.angle = atan2(-(vertPos.y - center.y), -(vertPos.x - center.x)) / M_PI * 180 + 180;

			verticesToOrder.push_back(newVertex);
		}

		std::stable_sort(verticesToOrder.begin(), verticesToOrder.end(), compareMeshes);

		vertex centerVertex;
		centerVertex.setPosition(center.x, center.y);
		centerVertex.setUv(0.5, 0.5);
		centerVertex.color = color;

		int renderVerticesOffset = _renderVertices.size() - 1;

		//Make the first triangle
		vertex thisRenderVertex;
		thisRenderVertex.position = verticesToOrder[0].position;
		thisRenderVertex.color = color;
		thisRenderVertex.uv = verticesToOrder[0].uv;

		_renderVertices.push_back(thisRenderVertex);
		_renderVertices.push_back(centerVertex);

		thisRenderVertex.position = verticesToOrder[1].position;
		thisRenderVertex.color = color;
		thisRenderVertex.uv = verticesToOrder[1].uv;
		_renderVertices.push_back(thisRenderVertex);

		//Create triangles
		for (int i = 1; i < verticesToOrder.size() - 1; i++) {
			_renderVertices.push_back(_renderVertices[renderVerticesOffset + i * 3]); //Push back the last vertex of the previous rectangle
			_renderVertices.push_back(centerVertex);

			thisRenderVertex.position = verticesToOrder[i + 1].position;
			thisRenderVertex.color = color;
			thisRenderVertex.uv = verticesToOrder[i + 1].uv;
			_renderVertices.push_back(thisRenderVertex);
		}
		//Make the last triangles
		_renderVertices.push_back(_renderVertices[renderVerticesOffset + (verticesToOrder.size() - 1) * 3]); //Push back the last vertex of the previous rectangle
		_renderVertices.push_back(centerVertex);

		thisRenderVertex.position = verticesToOrder[0].position;
		thisRenderVertex.color = color;
		thisRenderVertex.uv = verticesToOrder[0].uv;
		_renderVertices.push_back(thisRenderVertex);
		
	}

	void meshBatch::_createVertexArray() {

		if (_vao == 0) {
			glGenVertexArrays(1, &_vao);
		}
		glBindVertexArray(_vao);

		if (_vbo == 0) {
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
}
