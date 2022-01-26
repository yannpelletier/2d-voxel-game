#include "camera2D.h"
#include <iostream>

namespace gengine{

	camera2D::camera2D() : _position(0.0f, 0.0f),
		_cameraMatrix(1.0f),
		_orthoMatrix(1.0f),
		_scale(1),
		_needsMatrixUpdate(true),
		_screenWidth(500),
		_screenHeight(500)
	{
	}


	camera2D::~camera2D()
	{
	}

	void camera2D::init(int screenWidth, int screenHeight){
		_screenWidth = screenWidth;
		_screenHeight = screenHeight;
		_orthoMatrix = glm::ortho(0.0f, (float)_screenWidth, 0.0f, (float)_screenHeight);
	}

	void camera2D::update(){

		if (_needsMatrixUpdate){
			glm::vec3 translate(-_position.x + _screenWidth / 2, -_position.y + _screenHeight / 2, 0.0f);
			_cameraMatrix = glm::translate(_orthoMatrix, translate);

			glm::vec3 scale(_scale, _scale, 0.0f);
			_cameraMatrix = glm::scale(glm::mat4(1.0f),scale) * _cameraMatrix;

			_needsMatrixUpdate = false;
		}

	}

	void camera2D::setPosition(const glm::vec2 &newPosition)
	{
		_position = newPosition;
		_needsMatrixUpdate = true;
	}

	glm::vec2 camera2D::convertScreenToWorld(glm::vec2 screenCoords){
		//Inverts the y coordinates
		screenCoords.y = _screenHeight - screenCoords.y;
		//Make it so that the center is 0
		screenCoords -= glm::vec2(_screenWidth / 2, _screenHeight / 2);
		//Scale the coordinates
		screenCoords /= _scale;
		//Translate with the camera position
		screenCoords += _position;

		return screenCoords;
	}

	bool camera2D::isInCameraView(glm::vec2 &position, glm::vec2 &dimension) {
		glm::vec2 scaledScreenDimensions;
		scaledScreenDimensions.x = _screenWidth / _scale;
		scaledScreenDimensions.y = _screenHeight / _scale;

		// The minimum distance before a collision occurs
		const float MIN_DISTANCE_X = dimension.x / 2.0f + scaledScreenDimensions.x / 2.0f;
		const float MIN_DISTANCE_Y = dimension.y / 2.0f + scaledScreenDimensions.y / 2.0f;

		// Center position of the parameters
		glm::vec2 centerPos;
		centerPos.x = position.x + dimension.x / 2.0f;
		centerPos.y = position.y + dimension.y / 2.0f;
		// Center position of the camera
		glm::vec2 centerCameraPos = _position + scaledScreenDimensions / glm::vec2(2);
		// Vector from the input to the camera
		glm::vec2 distVec = centerPos - centerCameraPos;

		// Get the depth of the collision
		float xDepth = MIN_DISTANCE_X - abs(distVec.x);
		float yDepth = MIN_DISTANCE_Y - abs(distVec.y);

		// If both the depths are > 0, then we collided
		if (xDepth > 0 - scaledScreenDimensions.x && yDepth > 0 - scaledScreenDimensions.y) {
			// There was a collision
			return true;
		}
		return false;
	}
}