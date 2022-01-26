#include "window.h"


namespace gengine{

	window::window()
	{
	}


	window::~window()
	{
	}

	int window::create(std::string windowName, int screenWidth, int screenHeight, unsigned int currentFlags){
		_screenWidth = screenWidth;
		_screenHeight = screenHeight;

		Uint32 flags = SDL_WINDOW_OPENGL;
		if (currentFlags  & INVISIBLE){
			flags |= SDL_WINDOW_HIDDEN;
		}
		if (currentFlags & FULLSCREEN){
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;	
		}
		if (currentFlags & BORDERLESS){
			flags |= SDL_WINDOW_BORDERLESS;
		}

		//Create the SDL window
		_sdlWindow = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _screenWidth, _screenHeight, flags);

		if (currentFlags & FULLSCREEN) {
			SDL_GetWindowSize(_sdlWindow, &_screenWidth, &_screenHeight);
		}

		if (_sdlWindow == nullptr) {
			fatalError("SDL Window could not be created!");
		}

		//Set up our OpenGL context
		SDL_GLContext glContext = SDL_GL_CreateContext(_sdlWindow);
		if (glContext == nullptr) {
			fatalError("SDL_GL context could not be created!");
		}

		//Set up glew (optional but recommended)
		GLenum error = glewInit();
		if (error != GLEW_OK) {
			fatalError("Could not initialize glew!");
		}

		SDL_GL_SetSwapInterval(1);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		return 0;
	}

	void window::swapBuffer(){
		SDL_GL_SwapWindow(_sdlWindow);
	}

}