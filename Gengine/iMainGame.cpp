#include "iMainGame.h"
#include "screenList.h"
#include "iGameScreen.h"
#include "inputManager.h"

namespace gengine {

	iMainGame::iMainGame() {
		_screenList = std::make_unique<screenList>(this);
	}

	iMainGame::~iMainGame() {

	}

	void iMainGame::run() {
		if (!_init()) return;


		timing::setMaxFPS(120);

		_isRunning = true;
		
		int newTicks = 0;
		while (_isRunning != false) {
			timing::limitFPSBegin();
			timing::calculateFPS();

			_previousTicks = newTicks;
			newTicks = SDL_GetTicks();

			inputManager::update();

			update();
			if (_isRunning) {
				draw();

				_fps = timing::getFPS();

				_window.swapBuffer();

				timing::limitFPSEnd();
			}
		}
	}
	void iMainGame::exitGame() {
		_currentScreen->onExit();
		if (_screenList) {
			_screenList->destroy();
			_screenList.reset();
		}
		_isRunning = false;
	}

	bool iMainGame::_init() {
		gengine::init();

		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

		_initSystems();

		onInit();
		addScreens();

		_screenList->setScreen(0);
		_currentScreen = _screenList->getCurrent();
		_currentScreen->onEntry();
		_currentScreen->setRunning();

		return true;
	}

	void iMainGame::_initSystems() {
		_window.create("Default", 1080, 720, 0);
	}

	void iMainGame::onSDLEvent(SDL_Event& event) {
		switch (event.type) {
		case SDL_QUIT:
			_currentScreen->setState(screenState::EXIT_APPLICATION);
			break;
		case SDL_MOUSEMOTION:
			inputManager::setMouseCoords(event.motion.x, event.motion.y);
			break;
		case SDL_KEYDOWN:
			inputManager::pressKey(event.key.keysym.sym);
			break;
		case SDL_KEYUP:
			inputManager::releaseKey(event.key.keysym.sym);
			break;
		case SDL_MOUSEBUTTONDOWN:
			inputManager::pressKey(event.button.button);
			break;
		case SDL_MOUSEBUTTONUP:
			inputManager::releaseKey(event.button.button);
			break;
		case SDL_MOUSEWHEEL:
			inputManager::moveMouseWheel(event.wheel.y);
			break;
		}
	}

	void iMainGame::update() {
		if (_currentScreen) {
			switch (_currentScreen->getState()) {
			case screenState::RUNNING:
				_currentScreen->update();
				break;
			case screenState::CHANGE_NEXT:
				_currentScreen->onExit();
				_currentScreen = _screenList->moveNext();
				if (_currentScreen) {
					_currentScreen->setRunning();
					_currentScreen->onEntry();
				}
				break;
			case screenState::CHANGE_PREVIOUS:
				_currentScreen->onExit();
				_currentScreen = _screenList->movePrevious();
				if (_currentScreen) {
					_currentScreen->setRunning();
					_currentScreen->onEntry();
				}
				break;
			case screenState::EXIT_APPLICATION:
				exitGame();
				break;
			default:
				break;

			}
		}
		else {
			exitGame();
		}
	}

	void iMainGame::draw() {
		glViewport(0, 0, _window.getScreenWidth(), _window.getScreenHeight());
		if (_currentScreen && _currentScreen->getState() == screenState::RUNNING) {
			_currentScreen->draw();
		}
	}
}