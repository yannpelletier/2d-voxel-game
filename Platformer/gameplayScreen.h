#pragma once
#include <Gengine/GLTexture.h>
#include <Gengine/iGameScreen.h>
#include <Gengine/spriteBatch.h>
#include <Gengine/meshBatch.h>
#include <Gengine/GLSLProgram.h>
#include <Gengine/camera2D.h>
#include <Gengine/ressourceManager.h>
#include <Gengine/window.h>
#include <Gengine/debugRenderer.h>
#include <Gengine/particleEngine2D.h>
#include <SDL/SDL.h>
#include <Box2D/Box2D.h>
#include <vector>

#include "world.h"
#include "GUI.h"
#include "lightSystem.h"

class gameplayScreen: public gengine::iGameScreen
{
public:
	gameplayScreen(gengine::window* window);
	~gameplayScreen();

	virtual int getNextScreenIndex() const override;
	virtual int getPreviousScreenIndex() const override;

	//Called at beginning and end of application
	virtual void build() override;
	virtual void destroy() override;

	//Called when a screen enters and exits focus
	virtual void onEntry() override;
	virtual void onExit() override;

	//Called in the main game loop
	virtual void update() override;
	virtual void draw() override;

	void initGUI();

	bool onExitClicked(const CEGUI::EventArgs& e);

private:	
	void checkInput();

	gengine::GLSLProgram _textureProgram;
	gengine::camera2D _camera;
	gengine::camera2D _staticCamera;
	gengine::spriteBatch _spriteBatch;
	gengine::meshBatch _meshBatch;
	gengine::window* _window;
	gengine::debugRenderer _debugRenderer;

	bool _renderDebug = false;
	bool _isRaining = false;
	bool _isThundering = false;

	world* _world;

	//GUI
	gengine::GUI _GUI;

	int lastTick = 0;
};

