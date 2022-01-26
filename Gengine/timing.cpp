#include "timing.h"
using namespace std;

namespace gengine {
	float timing::_dt;
	float timing::totalDeltaTime;
	float timing::frameDT;
	float timing::_fps;
	int timing::_maxFPS;
	int timing::_frameTime;
	int timing::startTick;
	bool timing::simulatingFrames = false;
	int timing::framesSimulated;


	void timing::calculateFPS() {
		static const int NU_SAMPLES = 10;
		static float frameTimes[NU_SAMPLES];
		static int currentFrame = 0;

		static int prevTick = SDL_GetTicks();
		int currentTick = SDL_GetTicks();

		_frameTime = currentTick - prevTick;
		frameTimes[currentFrame % NU_SAMPLES] = _frameTime;

		prevTick = currentTick;

		float frameTimeAverage = 0;

		int count;
		currentFrame++;
		if (currentFrame < NU_SAMPLES) {
			count = currentFrame;
		}
		else {
			count = NU_SAMPLES;
		}

		for (int i = 0; i < count; i++) {
			frameTimeAverage += frameTimes[i];
		}
		frameTimeAverage /= count;

		count++;

		_fps = 1000 / frameTimeAverage;
	}

	float timing::getFPS() {
		return _fps;
	}

	void timing::setMaxFPS(int maxFPS) {
		_maxFPS = maxFPS;
	}

	void timing::setDT(float dt) {
		_dt = dt;
	}

	void timing::limitFPSBegin() {
		if (!_maxFPS) {
			fatalError("You must set a max FPS");
		}
		startTick = SDL_GetTicks();
	}

	void timing::limitFPSEnd() {
		int frameTick = SDL_GetTicks() - startTick;
		if ((1000 / _maxFPS) > frameTick) {
			SDL_Delay(1000 / _maxFPS - frameTick);
		}
	}
}