#pragma once
#include "SDL/SDL.h"

class animation
{
public:
	void init(int beginIndex, int endIndex, int timeBetweenFrames);

	bool update(); //Returns true if it looped
	void reset();

	void setCurrentIndex(int index);

	int getIndex() { return _currentIndex; }

private:
	//_indexes[0] is beginning index and _indexes[1] is ending index
	int _indexes[2];
	int _currentIndex;

	int _sinceLastFrameChange;
	int _lastTick;

	int _speed;
};

