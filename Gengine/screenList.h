#pragma once

#include <vector>

namespace gengine {

	class iMainGame;
	class iGameScreen;

	class screenList
	{
	public:
		screenList(iMainGame* game);
		~screenList();

		iGameScreen* moveNext();
		iGameScreen* movePrevious();

		void setScreen(int nextScreen);
		void addScreen(iGameScreen* newScreen);

		void destroy();

		iGameScreen* getCurrent();
	protected:
		iMainGame* _game = nullptr;
		std::vector<iGameScreen*> _screens;
		int _currentScreenIndex = -1;
	};
}
