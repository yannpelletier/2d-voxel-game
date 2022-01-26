#include "animation.h"

#include <iostream>

void animation::init(int beginIndex, int endIndex, int timeBetweenFrames) {
	_indexes[0] = beginIndex;
	_indexes[1] = endIndex;
	_speed = timeBetweenFrames;
	_sinceLastFrameChange = 0;
	_lastTick = 0;
	_currentIndex = _indexes[0];
}

bool animation::update() {
	if (_indexes[0] != _indexes[1]) {
		bool looped = false;
		int frameTime = SDL_GetTicks() - _lastTick;
		_sinceLastFrameChange += frameTime;

		//Checks 2 game loops in advance to see if it will loop
		if (_sinceLastFrameChange + frameTime * 2 >= _speed) {
			if (_currentIndex + 1 > _indexes[1]) {
				looped = true;
			}

		}
		if (_sinceLastFrameChange >= _speed) {
			_currentIndex += 1;
			if (_currentIndex > _indexes[1]) {
				_currentIndex = _indexes[0];

			}
			_sinceLastFrameChange = 0;
		}

		_lastTick = SDL_GetTicks();
		return looped;
	}
	else{
		_currentIndex = _indexes[0];
		return false;
	}
}

void animation::reset() {
	_sinceLastFrameChange = 0;
	_currentIndex = _indexes[0];
	_lastTick = SDL_GetTicks();
}

void animation::setCurrentIndex(int index) {
	if (index >= _indexes[0] && index <= _indexes[1]) {
		_currentIndex = index;
	}
}