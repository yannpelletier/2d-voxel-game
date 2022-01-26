#include "inventory.h"
#include "inventoryItem.h"

inventory::inventory(int size){
	_items.resize(size, nullptr);
}

//Returns true if item was already in inventory
bool inventory::addItem(inventoryItem* toAdd) {
	//First loop too see if there is an item of the same type in the inventory
	for (size_t i = 0; i < _items.size(); i++) {
		if (valid(i)) {
			if (_items[i]->getItemDefinition() == toAdd->getItemDefinition()) {
				_items[i]->fuseDrop(toAdd);
				return true;
			}
		}
	}
	//Second loop is to find the first free spot if there is no other place to stack the added item
	for (size_t i = 0; i < _items.size(); i++) {
		if (_items[i] == nullptr) {
			_items[i] = toAdd;
			return false;
		}
	}
}

void inventory::switchItems(inventory* inv1, inventory* inv2, int i1, int i2) {
	inventoryItem* item1 = inv1->getItem(i1);
	inventoryItem* item2 = inv2->getItem(i2);

	inv1->setItem(i1, item2);
	inv2->setItem(i2, item1);

}

void inventory::setItem(int i, inventoryItem* item) {
	if (i < _items.size()) {
		_items[i] = item;
		item->_state = inInventory;
	}
}


void inventory::printInventory() {
	for (int i = 0; i < _items.size(); i++) {
		if (getName(i) != "") {
			std::cout << getName(i) << endl;
		}
		else {
			std::cout << "EMPTY" << endl;
		}
	}
	std::cout << std::endl;
}

//Checks if inventory is full
bool inventory::isFull() {
	for (size_t i = 0; i < _items.size(); i++) {
		if (_items[i] == nullptr) {
			return false;
		}
	}
	return true;
}

//Checks if inventory is full, but returns false if there is a spot to merge the item with
bool inventory::isFull(inventoryItem* toAdd) {
	for (size_t i = 0; i < _items.size(); i++) {
		if (_items[i] == nullptr) {
			return false;
		}
		else if (_items[i]->getItemDefinition() == toAdd->getItemDefinition()) {
			return false;
		}
	}
	return true;
}

void inventory::setWidth(int width) {
	_inventoryWidth = width;
}

void inventory::open() {
	_closed = false;
}
void inventory::close() {
	_closed = true;
}
bool inventory::isClosed() {
	return _closed;
}

void inventory::resize(int size) {
	if (size > _items.size()) {
		_items.resize(size, nullptr);
	}
}

int inventory::getSize() {
	return _items.size();
}

int inventory::getWidth() {
	return _inventoryWidth;
}

void inventory::removeItem(int i) {
	if (i < _items.size()) {
		_items[i] = nullptr;
	}
}

void inventory::removeItem(inventoryItem* toDelete) {
	for (size_t i = 0; i < _items.size(); i++) {
		if (toDelete == _items[i]) {
			_items[i] = nullptr;
			return;
		}
	}
}

bool inventory::valid(int index) {
	if (index < _items.size() && index >= 0 && _items[index] != nullptr) {
		return true;
	}
	else {
		return false;
	}
}

std::string inventory::getName(int index) {
	if (index < _items.size()) {
		if (_items[index] != nullptr) {
			return _items[index]->getName();
		}
		else {
			return "";
		}
	}
	else {
		return "";
	}
}

int inventory::getQuantity(int index) {
	if (index < _items.size()) {
		if (_items[index] != nullptr) {
			return _items[index]->getQuantity();
		}
		else {
			return 0;
		}
	}
	else {
		return 0;
	}

}

int* inventory::getQuantityPtr(int index) {
	if (_items[index] != nullptr) {
		return &_items[index]->_quantity;
	}
	else {
		return nullptr;
	}
}


gengine::GLTexture* inventory::getIcon(int index) {
	if (_items[index] != nullptr) {
		return &_items[index]->getItemDefinition()->getIcon();
	}
	else {
		return nullptr;
	}
}

inventoryItem* inventory::getItem(int index) {
	if (_items[index] != nullptr) {
		return _items[index];
	}
	else {
		return nullptr;
	}
	

}

