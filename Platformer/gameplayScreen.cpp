#include "gameplayScreen.h"
#include "Gengine/iMainGame.h"
#include "placableItem.h"
#include "inventoryItem.h"
#include "usableItem.h"
#include "perlinNoise.h"
#include "npc.h"
#include <Gengine\GUI.h>

gameplayScreen::gameplayScreen(gengine::window* window)
{
	_window = window;
}


gameplayScreen::~gameplayScreen()
{
}

int gameplayScreen::getNextScreenIndex() const {
	return SCREEN_INDEX_NO_SCREEN;
}

int gameplayScreen::getPreviousScreenIndex() const {
	return SCREEN_INDEX_NO_SCREEN;
}

void gameplayScreen::build() {

}
void gameplayScreen::destroy() {

}

//Called when a screen enters and exits focus
void gameplayScreen::onEntry() {
	_world = new world(-35, &_camera);
	luaHandler::setWorld(_world);
	luaHandler::loadLua();

	_debugRenderer.init();

	player* newPlayer = new player;
	newPlayer->init(_world, glm::vec2(5.0f, 75.0f), glm::vec2(1.4, globals::pixToMet(globals::TILE_SIZE) * 1.25), glm::vec2(0.85, globals::pixToMet(globals::TILE_SIZE) * 3.6), gengine::color(255, 255, 255, 255), &_camera, glm::vec4(0, 0, 0.1, 0.5));
	_world->addEntity(newPlayer);

	npc* newNPC = new npc;
	newNPC->init(_world, glm::vec2(20.0f, 75.0f), glm::vec2(0.85, globals::pixToMet(globals::TILE_SIZE) * 3.5), npcType::humanoid, "skeleton_hu");
	_world->addEntity(newNPC);

	newPlayer->getInventory()->addItem(new inventoryItem(500, (material*)luaHandler::getDefinition("ground_m"), newPlayer, _world));
	newPlayer->getInventory()->addItem(new inventoryItem(500, (placableItem*)luaHandler::getDefinition("torch_pl"), newPlayer, _world));
	newPlayer->getInventory()->addItem(new inventoryItem(500, (usableItem*)luaHandler::getDefinition("throwable_spear_ui"), newPlayer, _world));
	newPlayer->getInventory()->addItem(new inventoryItem(25, (placableItem*)luaHandler::getDefinition("ceiling_light_pl"), newPlayer, _world));
	newPlayer->getInventory()->addItem(new inventoryItem(2, (usableItem*)luaHandler::getDefinition("flashlight_ui"), newPlayer, _world));
	newPlayer->getInventory()->addItem(new inventoryItem(1, (usableItem*)luaHandler::getDefinition("pickaxe_ui"), newPlayer, _world));
	newPlayer->getInventory()->addItem(new inventoryItem(2, (usableItem*)luaHandler::getDefinition("iron_sword_ui"), newPlayer, _world));
	newPlayer->getInventory()->addItem(new inventoryItem(2, (usableItem*)luaHandler::getDefinition("iron_spear_ui"), newPlayer, _world));
	newPlayer->getInventory()->addItem(new inventoryItem(50, (usableItem*)luaHandler::getDefinition("sticky_red_flare_ui"), newPlayer, _world));
	newPlayer->getInventory()->addItem(new inventoryItem(50, (usableItem*)luaHandler::getDefinition("red_flare_ui"), newPlayer, _world));
	newPlayer->getInventory()->addItem(new inventoryItem(2, (usableItem*)luaHandler::getDefinition("handgun_ui"), newPlayer, _world));

	_world->addEvent("dn_cycle_evnt");

	_world->generate((worldGenerator*)luaHandler::getDefinition("earthLike_wg"), 0, 500, time(0));

	worldTemplate structure = _world->getStructure("assets/structure/castle.json");
	_world->addTilesFromTemplate(structure, 50, 100);

	_spriteBatch.init();
	_meshBatch.init();

	_textureProgram.compileShaders("shaders/textureShading.vert", "shaders/textureShading.frag");
	_textureProgram.addAttribute("vertexPosition");
	_textureProgram.addAttribute("vertexColor");
	_textureProgram.addAttribute("vertexUV");
	_textureProgram.linkShaders();

	_camera.init(_window->getScreenWidth(), _window->getScreenHeight());
	_camera.setScale(34);

	GUI::init();
	GUI::initPlayerTaskbar(newPlayer->getInventory());

}

void gameplayScreen::onExit() {
	_debugRenderer.dispose();
}

//Called in the main game loop
void gameplayScreen::update() {
	checkInput();

	float frameDt = 0;

	for (int i = 0; i < 3; i++) {
		float frameTime = (SDL_GetTicks() - lastTick);
		frameTime /= 1000;
		frameDt += frameTime;

		lastTick = SDL_GetTicks();
		
		_world->step(frameTime, 8, 1);
	}
	gengine::timing::setDT(frameDt);

	_world->update(&_camera);

	_camera.update();

	GUI::updateIngameGUI();



	if (gengine::inputManager::keyPressed(SDLK_p)) {
		if (_renderDebug == false) {
			_renderDebug = true;
		}
		else {
			_renderDebug = false;
		}
	}

	if (gengine::inputManager::keyPressed(SDLK_r)) {
		if (_isRaining) {
			_world->deleteEvent("rain_evnt");
			_isRaining = false;
			std::cout << "rain stopped" << endl;
		}
		else {
			_world->addEvent("rain_evnt", 300000);
			_isRaining = true;
			std::cout << "rain started" << endl;
		}
	}
	if (gengine::inputManager::keyPressed(SDLK_t)) {
		if (_isThundering) {
			_world->deleteEvent("thunderstorm_evnt");
			_isThundering = false;
			std::cout << "thunder stopped" << endl;
		}
		else {
			_world->addEvent("thunderstorm_evnt", 300000);
			_isThundering = true;
			std::cout << "thunder started" << endl;
		}
	}
}

void gameplayScreen::draw() {
	_world->draw(&_spriteBatch, &_meshBatch, &_textureProgram, &_camera);

	//Debug rendering
	if (_renderDebug) {
		_world->debugDraw(&_debugRenderer, &_camera);
		_debugRenderer.end();
		_debugRenderer.render(_camera.getCameraMatrix(), 1.0f);
	}

	GUI::drawIngameGUI();
}

void gameplayScreen::checkInput() {
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt)) {
		_game->onSDLEvent(evnt);

		if (gengine::inputManager::keyPressed(SDL_BUTTON_RIGHT)) {
			if (GUI::ingame.hasClickPriority()) {
				gengine::inputManager::releaseKey(SDL_BUTTON_RIGHT);
			}
		}
		else if (gengine::inputManager::keyPressed(SDL_BUTTON_LEFT)) {
			if (GUI::ingame.hasClickPriority()) {
				gengine::inputManager::releaseKey(SDL_BUTTON_LEFT);
			}
		}

		GUI::onEvent(evnt);
	}
}
