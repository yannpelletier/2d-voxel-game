#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace gengine{

	class camera2D
	{
	public:
		camera2D();
		~camera2D();

		void init(int screenWidth, int screenHeight);

		void update();

		glm::vec2 convertScreenToWorld(glm::vec2 screenCoords);

		bool isInCameraView(glm::vec2& position, glm::vec2& dimensions);

		//Setters
		void setPosition(const glm::vec2 &newPosition);
		void setScale(float newScale) { _scale = newScale; _needsMatrixUpdate = true; }

		//Getters
		glm::vec2 getPosition(){ return _position; }
		glm::vec2 getDimensions() { return glm::vec2(_screenWidth, _screenHeight); }
		float getScale() { return _scale; }
		glm::mat4 getCameraMatrix(){ return _cameraMatrix; }


	private:
		int _screenWidth, _screenHeight;
		bool _needsMatrixUpdate;
		float _scale;
		glm::vec2 _position;
		glm::mat4 _cameraMatrix;
		glm::mat4 _orthoMatrix;
	};

}