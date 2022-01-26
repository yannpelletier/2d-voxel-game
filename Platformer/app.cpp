#include "app.h"
#include <list>

app::app()
{
}


app::~app()
{
	
}

void app::onInit() {
	luaHandler::createLuaState();
	luaHandler::registerLuaFunctions();
	gengine::audioEngine::init();
}

void app::addScreens() {
	_gameplayScreen = std::make_unique<gameplayScreen>(&_window);

	_screenList->addScreen(_gameplayScreen.get());
}

void app::onExit() {
	_screenList->destroy();
}