#pragma once
#include <Gengine/GLTexture.h>
#include <CEGUI/RendererModules/OpenGL/Texture.h>
#include <Gengine\GUI.h>
#include "inventory.h"
#include <CEGUI/CEGUI.h>

class itemDuo {
public:
	inventoryItem* item;
	int inventoryIndex;
};

class GUI
{
public:
	static void init();

	static void initPlayerTaskbar(inventory* inventory);
	static void openPlayerInventory(inventory* inventory);
	static void openWorldInventory(inventory* inventory);
	static CEGUI::GridLayoutContainer* openInventory(std::string gridName, inventory* inventory, int inventoryWidth, int from, int to);
	static void refreshInventory(CEGUI::GridLayoutContainer* toUpdate);

	static void updateIngameGUI();
	static void drawIngameGUI();
	static void onEvent(SDL_Event& evnt);

	static inventoryItem* getDraggedItem();
	static void deleteDraggedSlot();

	static bool EVENT_closeWindow(const CEGUI::EventArgs& e);
	static bool EVENT_hideWindow(const CEGUI::EventArgs& e);
	static bool EVENT_closeWorldInventoryWindow(const CEGUI::EventArgs& e);
	static bool EVENT_destroyWorldInventoryWindow(const CEGUI::EventArgs& e);

	static bool EVENT_SLOT_startDrag(const CEGUI::EventArgs& e);
	static bool EVENT_SLOT_endDrag(const CEGUI::EventArgs& e);

	static bool EVENT_DRAGGEDSLOT_update(const CEGUI::EventArgs& e);

	static bool EVENT_GRID_update(const CEGUI::EventArgs& e);

	static bool EVENT_droppedOn(const CEGUI::EventArgs& e);


	static gengine::GUI ingame;
private:
	static CEGUI::DragContainer* _draggedSlot;
	static CEGUI::Window* _playerTaskBar;
	static CEGUI::FrameWindow* _playerInventoryWindow;
	static CEGUI::FrameWindow* _worldInventoryWindow;

	static const float PLAYER_TASKBAR_WIDTH;
	static const float SLOT_WIDTH;
	static const float DRAGGEDSLOT_WIDTH;
};

