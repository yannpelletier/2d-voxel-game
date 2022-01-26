#pragma once
#include "SDL/SDL.h"
#include "errors.h"
#include <iostream>
#include <algorithm>

namespace gengine{
	class timing
	{
	public:
		static float getFPS();
		static void setMaxFPS(int maxFPS);
		static void setDT(float dt);

		static void calculateFPS();

		//Returns true if frames are still being simulated

		static float dt() { return _dt; }

		static void limitFPSBegin();
		static void limitFPSEnd();

	private:
		static float totalDeltaTime;

		static float _fps;
		static float frameDT;
		static int _maxFPS;
		static int _frameTime;

		static float  _dt; //Delta time

		static bool simulatingFrames;
		static int framesSimulated;

		static int startTick;
	};
}
