#include "GUI.h"
#include "inventoryItem.h"
#include <string>
#include <iostream>

gengine::GUI GUI::ingame;
CEGUI::DragContainer* GUI::_draggedSlot = nullptr;
CEGUI::Window* GUI::_playerTaskBar = nullptr;
CEGUI::FrameWindow* GUI::_playerInventoryWindow = nullptr;
CEGUI::FrameWindow* GUI::_worldInventoryWindow = nullptr;
const float GUI::SLOT_WIDTH = 40;
const float GUI::DRAGGEDSLOT_WIDTH = 25;
const float GUI::PLAYER_TASKBAR_WIDTH = SLOT_WIDTH * 10.25;

void GUI::init() {
	ingame.init("GUI");
	ingame.loadScheme("TaharezLook.scheme");
	ingame.setFont("DejaVuSans-10");
	ingame.getRoot()->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, &GUI::EVENT_droppedOn);
}

void GUI::initPlayerTaskbar(inventory* inventory){
	if (_playerTaskBar == nullptr) {
		_playerTaskBar = static_cast<CEGUI::Window*>(ingame.createWidget("DefaultWindow", glm::vec4(0.5, 0, 0, 0), glm::vec4(-(PLAYER_TASKBAR_WIDTH / 2), 0, PLAYER_TASKBAR_WIDTH, SLOT_WIDTH), "playerTaskBar"));
		CEGUI::GridLayoutContainer* inventoryGrid = openInventory("playerTaskBarInventory", inventory, 10, 0, 9);
		if (inventoryGrid != nullptr) {
			_playerTaskBar->addChild(inventoryGrid);
		}
	}
}
void GUI::openPlayerInventory(inventory* inventory) {
	if (_playerInventoryWindow == nullptr) {
		_playerInventoryWindow = static_cast<CEGUI::FrameWindow*>(ingame.createWidget("TaharezLook/FrameWindow", glm::vec4(0.15, 0.15, 0, 0), glm::vec4(0,0,SLOT_WIDTH * 10 + SLOT_WIDTH / 2.75,std::ceil(inventory->getSize() / 10) * SLOT_WIDTH), "playerInventoryWindow"));
		_playerInventoryWindow->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, &EVENT_droppedOn);
		_playerInventoryWindow->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, &EVENT_hideWindow);
		_playerInventoryWindow->setSizingEnabled(false);
		_playerInventoryWindow->getChildAtIdx(0)->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, &EVENT_droppedOn);
		CEGUI::GridLayoutContainer* inventoryGrid = openInventory("playerInventoryWindowInventory", inventory, 10, 10, inventory->getSize() - 1);
		if (inventoryGrid != nullptr) {
			_playerInventoryWindow->addChild(inventoryGrid);
			_playerInventoryWindow->performChildWindowLayout();
			ingame.setWidgetDestRect(inventoryGrid, glm::vec4(0, 0, 1, 1), glm::vec4(0, 0, 0, 0));
		}
	}
	else {
		if (_playerInventoryWindow->isVisible()){
			_playerInventoryWindow->hide();
		}
		else {
			_playerInventoryWindow->show();
		}

	}
}
void GUI::openWorldInventory(inventory* inventorys) {
	if (_worldInventoryWindow == nullptr) {
		inventorys->open();
		_worldInventoryWindow = static_cast<CEGUI::FrameWindow*>(ingame.createWidget("TaharezLook/FrameWindow", glm::vec4(0.15, 0.6, 0, 0), glm::vec4(0, 0, SLOT_WIDTH * inventorys->getWidth() + SLOT_WIDTH / 2.75, std::ceil(inventorys->getSize() / inventorys->getWidth()) * SLOT_WIDTH + SLOT_WIDTH / 1.5 ), "worldInventoryWindow"));
		_worldInventoryWindow->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, &EVENT_closeWorldInventoryWindow);
		_worldInventoryWindow->subscribeEvent(CEGUI::FrameWindow::EventDestructionStarted, &EVENT_destroyWorldInventoryWindow);
		_worldInventoryWindow->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, &EVENT_droppedOn);
		_worldInventoryWindow->setSizingEnabled(false);
		_worldInventoryWindow->getChildAtIdx(0)->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, &EVENT_droppedOn);
		CEGUI::GridLayoutContainer* inventoryGrid = openInventory("worldInventoryWindowInventory", inventorys, inventorys->getWidth(), 0, inventorys->getSize() - 1);
		if (inventoryGrid != nullptr) {
			_worldInventoryWindow->addChild(inventoryGrid);
			_worldInventoryWindow->performChildWindowLayout();
			ingame.setWidgetDestRect(inventoryGrid, glm::vec4(0, 0, 1, 1), glm::vec4(0, 0, 0, 0));
		}
	}
	else {
		if (static_cast<inventory*>(_worldInventoryWindow->getChildAtIdx(2)->getUserData()) == inventorys) {
			_worldInventoryWindow->destroy();
			_worldInventoryWindow = nullptr;
		}
		else {
			_worldInventoryWindow->destroy();
			inventorys->open();
			_worldInventoryWindow = static_cast<CEGUI::FrameWindow*>(ingame.createWidget("TaharezLook/FrameWindow", glm::vec4(0.15, 0.6, 0, 0), glm::vec4(0, 0, SLOT_WIDTH * inventorys->getWidth() + SLOT_WIDTH / 2.75, std::ceil(inventorys->getSize() / inventorys->getWidth()) * SLOT_WIDTH + SLOT_WIDTH / 1.5), "worldInventoryWindow"));
			_worldInventoryWindow->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, &EVENT_closeWorldInventoryWindow);
			_worldInventoryWindow->subscribeEvent(CEGUI::FrameWindow::EventDestructionStarted, &EVENT_destroyWorldInventoryWindow);
			_worldInventoryWindow->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, &EVENT_droppedOn);
			_worldInventoryWindow->getChildAtIdx(0)->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, &EVENT_droppedOn);
			CEGUI::GridLayoutContainer* inventoryGrid = openInventory("worldInventoryWindowInventory", inventorys, inventorys->getWidth(), 0, inventorys->getSize() - 1);
			if (inventoryGrid != nullptr) {
				_worldInventoryWindow->addChild(inventoryGrid);
				_worldInventoryWindow->performChildWindowLayout();
				ingame.setWidgetDestRect(inventoryGrid, glm::vec4(0, 0, 1, 1), glm::vec4(0, 0, 0, 0));
			}
		}

	}
}

CEGUI::GridLayoutContainer* GUI::openInventory(std::string gridName, inventory* inventory, int inventoryWidth, int from, int to) {
	if ((to - from + 1) % inventoryWidth != 0) {
		gengine::fatalError("inventory size must be dividable by INVENTORY_WIDTH!");
	}
	//if there is not already a window with that name
	CEGUI::String name = gridName;
	if (ingame.getRoot()->isChild(name) == false) {
		if (inventory != nullptr) {
			//GridLayoutContainer
			CEGUI::GridLayoutContainer* grid = static_cast<CEGUI::GridLayoutContainer*>(ingame.createWidget("GridLayoutContainer", glm::vec4(0, 0, 1, 1), glm::vec4(0,0,0,0), gridName));
			grid->setDragDropTarget(false);
			grid->subscribeEvent(CEGUI::GridLayoutContainer::EventUpdated, &EVENT_GRID_update);
			grid->setAutoPositioning(CEGUI::GridLayoutContainer::AutoPositioning::AP_LeftToRight);
			float inventoryHeight = (float)(to - from + 1) / inventoryWidth;
			grid->setGridDimensions(inventoryWidth, std::ceil(inventoryHeight));
			grid->setUserData((void*)(inventory));
			grid->subscribeEvent(CEGUI::FrameWindow::EventDragDropItemDropped, &GUI::EVENT_droppedOn);


			for (int i = from; i <= to; i++) {
				CEGUI::Window *slot = static_cast<CEGUI::Window*>(ingame.createWidget("TaharezLook/StaticImage", gridName + " slot" + std::to_string(i)));
				slot->subscribeEvent(CEGUI::Window::EventMouseClick, &EVENT_SLOT_startDrag);
				slot->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, &EVENT_SLOT_endDrag);
				slot->setDragDropTarget(true);

				itemDuo* newDuo = new itemDuo;
				newDuo->item = inventory->getItem(i);
				newDuo->inventoryIndex = i;
				slot->setUserData((void*)newDuo);

				CEGUI::Window *quantity = static_cast<CEGUI::Window*>(ingame.createWidget("TaharezLook/StaticText", glm::vec4(0, 0, 1, 1), glm::vec4(0,0,0,0), gridName + " quantity" + std::to_string(i)));
				slot->addChild(quantity);
				quantity->setProperty("TextColours", "tl:FFFAFAFF tr:FFFAFAFF bl:FFFAFAFF br:FFFAFAFF");
				quantity->setProperty("BackgroundEnabled", "False");
				quantity->setProperty("FrameEnabled", "False");
				quantity->setProperty("VertFormatting", "TopAligned");
				quantity->setProperty("HorzFormatting", "LeftAligned"); // LeftAligned, RightAligned, HorzCentred
				quantity->disable();

				if (inventory->getName(i) != "") {
					if (!CEGUI::ImageManager::getSingletonPtr()->isDefined(inventory->getName(i) + "_image") && !CEGUI::ImageManager::getSingletonPtr()->isDefined(inventory->getName(i) + "_texture")) {
						quantity->setText(std::to_string(inventory->getQuantity(i)));


						gengine::GLTexture* itemIcon = inventory->getIcon(i);
						CEGUI::OpenGL3Renderer* renderer = static_cast<CEGUI::OpenGL3Renderer*>(CEGUI::System::getSingletonPtr()->getRenderer());
						CEGUI::OpenGLTexture* texture = static_cast<CEGUI::OpenGLTexture*>(&renderer->createTexture(inventory->getName(i) + "_texture", itemIcon->id, CEGUI::Sizef(itemIcon->width,itemIcon->height)));

						CEGUI::BasicImage* image = (CEGUI::BasicImage*)(&CEGUI::ImageManager::getSingleton().create("BasicImage", inventory->getName(i) + "_image"));

						image->setArea(CEGUI::Rectf(CEGUI::Vector2f(0.0f, 0.0f), texture->getOriginalDataSize()));
						image->setTexture(texture);
					}

					slot->setProperty("Image", inventory->getName(i) + "_image");
				}

				slot->setSize(CEGUI::USize(CEGUI::UDim(0, SLOT_WIDTH), CEGUI::UDim(0, SLOT_WIDTH)));
				grid->addChild(slot);
			}

			grid->layout();
			return grid;
		}
	}
	else {
		return nullptr;
	}
}

void GUI::refreshInventory(CEGUI::GridLayoutContainer* toUpdate) {
	inventory* gridInventory = static_cast<inventory*>(toUpdate->getUserData());
	for (int i = 0; i < toUpdate->getChildCount(); i++) {
		CEGUI::Window *slot = toUpdate->getChildAtIdx(i);

		itemDuo* slotDuo = static_cast<itemDuo*>(slot->getUserData());
		slotDuo->item = gridInventory->getItem(slotDuo->inventoryIndex);

		if (gridInventory->getName(slotDuo->inventoryIndex) != "") {
			CEGUI::Window *quantity = static_cast<CEGUI::Window*>(slot->getChildAtIdx(0));
			quantity->setText(std::to_string(gridInventory->getQuantity(slotDuo->inventoryIndex)));

			if (!CEGUI::ImageManager::getSingletonPtr()->isDefined(gridInventory->getName(i) + "_image") && !CEGUI::ImageManager::getSingletonPtr()->isDefined(gridInventory->getName(i) + "_texture")) {
				gengine::GLTexture* itemIcon = gridInventory->getIcon(i);
				if (itemIcon != nullptr) {
					quantity->setText(std::to_string(gridInventory->getQuantity(i)));

					CEGUI::OpenGL3Renderer* renderer = static_cast<CEGUI::OpenGL3Renderer*>(CEGUI::System::getSingletonPtr()->getRenderer());
					CEGUI::OpenGLTexture* texture = static_cast<CEGUI::OpenGLTexture*>(&renderer->createTexture(gridInventory->getName(i) + "_texture", itemIcon->id, CEGUI::Sizef(itemIcon->width, itemIcon->height)));

					CEGUI::BasicImage* image = (CEGUI::BasicImage*)(&CEGUI::ImageManager::getSingleton().create("BasicImage", gridInventory->getName(i) + "_image"));

					image->setArea(CEGUI::Rectf(CEGUI::Vector2f(0.0f, 0.0f), texture->getOriginalDataSize()));
					image->setTexture(texture);
				}
			}

			slot->setProperty("Image", gridInventory->getName(slotDuo->inventoryIndex) + "_image");
		}
		else {
			CEGUI::Window *quantity = static_cast<CEGUI::Window*>(slot->getChildAtIdx(0));
			quantity->setText("");

			slot->setProperty("Image", "");

		}
	}
}

void GUI::updateIngameGUI() {
	ingame.update();
}

void GUI::onEvent(SDL_Event& evnt) {
	ingame.onSDLEvent(evnt);
}

void GUI::drawIngameGUI() {
	ingame.draw();
}

inventoryItem* GUI::getDraggedItem() {
	if (_draggedSlot != nullptr) {
		return static_cast<inventoryItem*>(_draggedSlot->getUserData());
	}
	else {
		return nullptr;
	}
}

void GUI::deleteDraggedSlot() {
	//Delete dragged container
	_draggedSlot->destroy();
	_draggedSlot = nullptr;
}

bool GUI::EVENT_SLOT_startDrag(const CEGUI::EventArgs& e) {
	CEGUI::Window* slot = static_cast<const CEGUI::WindowEventArgs&>(e).window;
	if (static_cast<itemDuo*>(slot->getUserData())->item != nullptr) {
		if (_draggedSlot == nullptr) {
			itemDuo* itemDuos = static_cast<itemDuo*>(slot->getUserData());

			//Set up dragged container
			CEGUI::Window* image = static_cast<CEGUI::DragContainer*>(ingame.createWidget("TaharezLook/StaticImage", glm::vec4(0, 0, 1, 1), glm::vec4(0), "draggedSlotImage"));
			image->setProperty("BackgroundEnabled", "False");
			image->setProperty("FrameEnabled", "False");
			image->setProperty("Image", itemDuos->item->getName() + "_image");
			_draggedSlot = static_cast<CEGUI::DragContainer*>(ingame.createWidget("DragContainer", "draggedSlot"));
			_draggedSlot->setSize(CEGUI::USize(CEGUI::UDim(0, DRAGGEDSLOT_WIDTH), CEGUI::UDim(0, DRAGGEDSLOT_WIDTH)));
			_draggedSlot->addChild(image);
			_draggedSlot->setAlwaysOnTop(true);
			_draggedSlot->pickUp(true);

			CEGUI::Window *quantity = static_cast<CEGUI::Window*>(ingame.createWidget("TaharezLook/StaticText", glm::vec4(0, 0, 1, 1), glm::vec4(0), "draggedSlotQuantity"));
			quantity->setProperty("TextColours", "tl:FFFAFAFF tr:FFFAFAFF bl:FFFAFAFF br:FFFAFAFF");
			quantity->setProperty("VertFormatting", "VertCentred"); // TopAligned, BottomAligned, VertCentred
			quantity->setProperty("HorzFormatting", "HorzCentred"); // LeftAligned, RightAligned, HorzCentred
			quantity->setProperty("BackgroundEnabled", "False");
			quantity->setProperty("FrameEnabled", "False");
			_draggedSlot->addChild(quantity);

			_draggedSlot->subscribeEvent(CEGUI::Window::EventUpdated, &EVENT_DRAGGEDSLOT_update);

			_draggedSlot->setUserData((void*)(static_cast<itemDuo*>(slot->getUserData())->item));
			static_cast<itemDuo*>(slot->getUserData())->item->drag();

			//Remove item from slot and from inventory
			CEGUI::GridLayoutContainer* slotGrid = static_cast<CEGUI::GridLayoutContainer*>(slot->getParent());
			inventory* slotInventory = static_cast<inventory*>(slotGrid->getUserData());
			slotInventory->removeItem(static_cast<itemDuo*>(slot->getUserData())->inventoryIndex);
			itemDuos->item = nullptr;
		}
	}

	return true;
}

bool GUI::EVENT_SLOT_endDrag(const CEGUI::EventArgs& e) {
	CEGUI::Window* slot = static_cast<const CEGUI::WindowEventArgs&>(e).window;
	itemDuo* slotDuo = static_cast<itemDuo*>(slot->getUserData());

	if (slotDuo->item == nullptr) {

		//Get slot's inventory
		CEGUI::GridLayoutContainer* slotGrid = static_cast<CEGUI::GridLayoutContainer*>(slot->getParent());
		inventory* slotInventory = static_cast<inventory*>(slotGrid->getUserData());

		//Add item to the inventory
		inventoryItem* draggedContainerItem = static_cast<inventoryItem*>(_draggedSlot->getUserData());
		slotInventory->setItem(slotDuo->inventoryIndex, draggedContainerItem);
		
		//Remove item from the dragged container
		_draggedSlot->setUserData(nullptr);

		//Delete dragged container
		_draggedSlot->destroy();
		_draggedSlot = nullptr;
	}
	else if(slotDuo->item != nullptr){
		if (slotDuo->item->getItemDefinition() != static_cast<inventoryItem*>(_draggedSlot->getUserData())->getItemDefinition()) {
			//Get slot's inventory
			CEGUI::GridLayoutContainer* slotGrid = static_cast<CEGUI::GridLayoutContainer*>(slot->getParent());
			inventory* slotInventory = static_cast<inventory*>(slotGrid->getUserData());

			//Add item to the inventory
			inventoryItem* draggedContainerItem = static_cast<inventoryItem*>(_draggedSlot->getUserData());
			_draggedSlot->setUserData((void*)slotDuo->item);

			slotInventory->setItem(slotDuo->inventoryIndex, draggedContainerItem);

			_draggedSlot->getChildAtIdx(0)->setProperty("Image", slotDuo->item->getName() + "_image");

			CEGUI::DragContainer* toDelete = _draggedSlot;
			toDelete->setName("deletedDraggedSlot");
			_draggedSlot = static_cast<CEGUI::DragContainer*>(ingame.createWidget("DragContainer", "draggedSlot"));
			toDelete->cloneChildWidgetsTo(*_draggedSlot);
			_draggedSlot->setSize(CEGUI::USize(CEGUI::UDim(0, DRAGGEDSLOT_WIDTH), CEGUI::UDim(0, DRAGGEDSLOT_WIDTH)));
			_draggedSlot->setAlwaysOnTop(true);
			_draggedSlot->pickUp(true);
			_draggedSlot->subscribeEvent(CEGUI::Window::EventUpdated, &EVENT_DRAGGEDSLOT_update);

			toDelete->destroy();

			_draggedSlot->setUserData((void*)(static_cast<itemDuo*>(slot->getUserData())->item));
		}
		else {
			//Get slot's item
			inventoryItem* slotItem = slotDuo->item;

			inventoryItem* draggedContainerItem = static_cast<inventoryItem*>(_draggedSlot->getUserData());
				
			slotItem->fuseDrop(draggedContainerItem);

			_draggedSlot = nullptr;
		}

	}
	return true;
}

bool GUI::EVENT_DRAGGEDSLOT_update(const CEGUI::EventArgs& e) {
	CEGUI::DragContainer* window = static_cast<CEGUI::DragContainer*>(static_cast<const CEGUI::WindowEventArgs&>(e).window);
	CEGUI::Window *quantity = static_cast<CEGUI::Window*>(window->getChildAtIdx(1));
	inventoryItem* item = static_cast<inventoryItem*>(window->getUserData());
	quantity->setText(std::to_string(item->getQuantity()));
	return true;
}

bool GUI::EVENT_GRID_update(const CEGUI::EventArgs& e) {
	refreshInventory(static_cast<CEGUI::GridLayoutContainer*>(static_cast<const CEGUI::WindowEventArgs&>(e).window));
	return true;
}

bool GUI::EVENT_hideWindow(const CEGUI::EventArgs& e) {
	static_cast<const CEGUI::WindowEventArgs&>(e).window->hide();
	return true;

}

bool GUI::EVENT_closeWindow(const CEGUI::EventArgs& e) {
	static_cast<const CEGUI::WindowEventArgs&>(e).window->destroy();
	return true;
}

bool GUI::EVENT_closeWorldInventoryWindow(const CEGUI::EventArgs& e) {
	_worldInventoryWindow->destroy();
	_worldInventoryWindow = nullptr;
	return true;
}

bool GUI::EVENT_destroyWorldInventoryWindow(const CEGUI::EventArgs& e) {
	static_cast<inventory*>(_worldInventoryWindow->getChildAtIdx(2)->getUserData())->close();
	return true;
}

bool GUI::EVENT_droppedOn(const CEGUI::EventArgs& e) {
	CEGUI::DragContainer* toDelete = _draggedSlot;
	toDelete->setName("deletedDraggedSlot");
	_draggedSlot = static_cast<CEGUI::DragContainer*>(ingame.createWidget("DragContainer", "draggedSlot"));
	toDelete->cloneChildWidgetsTo(*_draggedSlot);
	_draggedSlot->setSize(CEGUI::USize(CEGUI::UDim(0, DRAGGEDSLOT_WIDTH), CEGUI::UDim(0, DRAGGEDSLOT_WIDTH)));
	_draggedSlot->setAlwaysOnTop(true);
	_draggedSlot->pickUp(true);
	_draggedSlot->subscribeEvent(CEGUI::Window::EventUpdated, &EVENT_DRAGGEDSLOT_update);


	toDelete->destroy();

	_draggedSlot->setUserData((void*)(toDelete->getUserData()));
	return true;
}