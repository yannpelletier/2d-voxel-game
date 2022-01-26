#pragma once
#include "window.h"
#include "timing.h"
#include "gengine.h"
#include "screenList.h"

#include <memory>

namespace gengine {
	class iGameScreen;

	class iMainGame {
	public:
		iMainGame();
		virtual ~iMainGame();

		void run();
		void exitGame();

		virtual void onInit() = 0;
		virtual void addScreens() = 0;
		virtual void onExit() = 0;

		void onSDLEvent(SDL_Event& event);

		const int getFPS() const {
			return _fps;
		}
		
	protected:
		void update();
		void draw();

		bool _init();
		//Window creation takes care of error checking
		void _initSystems();

		std::shared_ptr<screenList> _screenList = nullptr;
		iGameScreen* _currentScreen = nullptr;
		bool _isRunning = false;
		int _fps;
		int _previousTicks;
		window _window;
	};

}