#pragma once

#include "SDL/SDL.h"
#include <glm/glm.hpp>
#include <unordered_map>
#include <string>


namespace gengine{
	class inputManager
	{
	public:
		static void pressKey(unsigned int keyID);
		static void releaseKey(unsigned int keyID);
		static void moveMouseWheel(signed int yDirection);

		static void setMouseCoords(float x, float y);
		static glm::vec2 getMouseCoords();

		static void update();

		//Checks if the key is held down this frame
		static bool isKeyDown(unsigned int keyID);
		//Checks if the key was held down last frame
		static bool wasKeyDown(unsigned int keyID);

		//Check if the key was just pressed
		static bool keyPressed(unsigned int keyID);
		//Check if the key was just released
		static bool keyReleased(unsigned int keyID);

		static int numberPressed();

		static signed int isMouseWheelMoved();

	private:
		static std::unordered_map<unsigned int, bool> _keyStates;
		static std::unordered_map<unsigned int, bool> _previousKeyStates;
		static glm::vec2 _mouseCoords;

		static signed int _mouseWheelState;
	};


}
