#include "screenList.h"
#include "iMainGame.h"
#include "iGameScreen.h"

namespace gengine {

	screenList::screenList(iMainGame* game)
	{
		_game = game;
		//Empty
	}


	screenList::~screenList()
	{
		destroy();
	}


	iGameScreen* screenList::moveNext() {
		iGameScreen* currentScreen = getCurrent();
		if (currentScreen->getNextScreenIndex() != SCREEN_INDEX_NO_SCREEN) {
			_currentScreenIndex = currentScreen->getNextScreenIndex();
		}

		return getCurrent();
	}

	iGameScreen* screenList::movePrevious() {
		iGameScreen* currentScreen = getCurrent();
		if (currentScreen->getPreviousScreenIndex() != SCREEN_INDEX_NO_SCREEN) {
			_currentScreenIndex = currentScreen->getPreviousScreenIndex();
		}

		return getCurrent();
	}

	void screenList::setScreen(int nextScreen) {
		if ((size_t) nextScreen >= _screens.size() || nextScreen < 0) {
			return;
		}
		_currentScreenIndex = nextScreen;
	}

	void screenList::addScreen(iGameScreen* newScreen) {
		newScreen->_screenIndex = _screens.size();
		_screens.push_back(newScreen);
		newScreen->build();
		newScreen->setParentGame(_game);
	}

	void screenList::destroy() {
		for (size_t i = 0; i < _screens.size(); i++) {
			_screens[i]->destroy();
		}
		_screens.resize(0);
		_currentScreenIndex = SCREEN_INDEX_NO_SCREEN;
	}

	iGameScreen* screenList::getCurrent() {
		if (_currentScreenIndex == SCREEN_INDEX_NO_SCREEN) {
			return nullptr;
		}
		return _screens[_currentScreenIndex];
	}
}