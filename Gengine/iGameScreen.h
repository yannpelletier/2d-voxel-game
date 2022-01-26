#pragma once

#define SCREEN_INDEX_NO_SCREEN -1

namespace gengine {

	enum class screenState {
		NONE,
		RUNNING,
		EXIT_APPLICATION,
		CHANGE_NEXT,
		CHANGE_PREVIOUS
	};

	class iMainGame;

	class iGameScreen
	{
	public:
		friend class screenList;

		iGameScreen() {
			//Empty
		}
		virtual ~iGameScreen() {
			//Empty
		}

		//Returns the index of the next or previous screen when changing screens
		virtual int getNextScreenIndex() const = 0;
		virtual int getPreviousScreenIndex() const = 0;

		//Called at beginning and end of application
		virtual void build() = 0;
		virtual void destroy() = 0;

		//Called when a screen enters and exits focus
		virtual void onEntry() = 0;
		virtual void onExit() = 0;

		//Called in the main game loop
		virtual void update() = 0;
		virtual void draw() = 0;

		int getIndex() const {
			return _screenIndex;
		}

		void setState(screenState screenState) {
			_currentState = screenState;
		}

		screenState getState() { 
			return _currentState;  
		}
		void setRunning() {
			_currentState = screenState::RUNNING;
		}

		void setParentGame(iMainGame* game) { _game = game; }
	
	protected:
		screenState _currentState = screenState::NONE;
		iMainGame* _game = nullptr;
		
		int _screenIndex = -1;
	};

}