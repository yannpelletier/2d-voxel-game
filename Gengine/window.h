#pragma once
#include "SDL/SDL.h"
#include "GL/glew.h"
#include "errors.h"
#include <string>

namespace gengine{

	enum WindowFlags { INVISIBLE = 0x1, FULLSCREEN = 0x2, BORDERLESS = 0x4 };

	class window
	{
	public:
		window();
		~window();

		int create(std::string windowName, int screenWidth, int screenHeight, unsigned int currentFlags);

		void swapBuffer();

		int getScreenWidth(){ return _screenWidth; }
		int getScreenHeight(){ return _screenHeight; }
	private:
		SDL_Window* _sdlWindow;
		int _screenWidth, _screenHeight;

	};

}