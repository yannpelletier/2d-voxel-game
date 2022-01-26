#pragma once
#include "Gengine/iMainGame.h"
#include "gameplayScreen.h"
#include "Lua/lua.hpp"
#include "luaHandler.h"

class app: public gengine::iMainGame
{
public:
	app();
	~app();

	virtual void onInit() override;
	virtual void addScreens() override;
	virtual void onExit() override;
private:
	std::unique_ptr<gameplayScreen> _gameplayScreen = nullptr;
};

