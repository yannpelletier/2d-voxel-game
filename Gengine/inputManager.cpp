#include "inputManager.h"
#include <iostream>

namespace gengine{
	glm::vec2 inputManager::_mouseCoords;
	std::unordered_map<unsigned int, bool> inputManager::_keyStates;
	std::unordered_map<unsigned int, bool> inputManager::_previousKeyStates;
	int inputManager::_mouseWheelState;

	void inputManager::update(){
		for (auto& it : _keyStates){
			_previousKeyStates[it.first] = it.second;
		}

		_mouseWheelState = 0;
	}

	void inputManager::pressKey(unsigned int keyID){
		_keyStates[keyID] = true;
	}
	void inputManager::releaseKey(unsigned int keyID){
		_keyStates[keyID] = false;
	}

	//Return -1 if no number is pressed
	int inputManager::numberPressed() {
		for (int i = SDLK_0; i <= SDLK_9; i++) {
			if (_keyStates[i] == true) {
				if (i == SDLK_0) {
					return 10;
				}
				else {
					return i - 49; //The number pressed (SDLK_0 == 49)
				}
				
			}
		}

		return -1;
	}

	void inputManager::setMouseCoords(float x, float y){
		_mouseCoords.x = x;
		_mouseCoords.y = y;
	}

	glm::vec2 inputManager::getMouseCoords(){
		return _mouseCoords;
	}

	void inputManager::moveMouseWheel(signed int yDirection){
		_mouseWheelState = yDirection;
	}

	signed int inputManager::isMouseWheelMoved(){
		return _mouseWheelState;
	}


	bool inputManager::isKeyDown(unsigned int keyID) {
		auto it = _keyStates.find(keyID);
		if (it != _keyStates.end()) {
			return it->second;
		}
		else {
			return false;
		}
	}

	bool inputManager::wasKeyDown(unsigned int keyID){
		auto it = _previousKeyStates.find(keyID);
		if (it != _previousKeyStates.end()){
			return it->second;
		}
		else{
			return false;
		}
	}

	bool inputManager::keyPressed(unsigned int keyID){
		//Check if it is pressed this frame and wasn't pressed last frame
		if (isKeyDown(keyID) == true && wasKeyDown(keyID) == false){
			return true;
		}
		return false;
	}

	bool inputManager::keyReleased(unsigned int keyID){
		if (isKeyDown(keyID) == false && wasKeyDown(keyID) == true){
			return true;
		}
		return false;
	}
}