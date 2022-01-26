#pragma once
#include <vector>
#include <Gengine/GLTexture.h>

class inventoryItem;

class inventory
{
public:
	inventory(int size);
	bool addItem(inventoryItem* toAdd);
	void removeItem(int i);
	void removeItem(inventoryItem* toDelete);
	void switchItems(inventory* inv1, inventory* inv2, int i1, int i2);
	void setItem(int i, inventoryItem* toSet);

	void printInventory();

	bool isFull();
	bool isFull(inventoryItem* toAdd);
	
	void resize(int size);

	void setWidth(int width);

	void open();
	void close();
	bool isClosed();

	int getSize();
	int getWidth();

	//Returns true if there is an item at the index given
	bool valid(int index);
	
	std::string getName(int index);
	int getQuantity(int index);
	int* getQuantityPtr(int index);
	gengine::GLTexture* getIcon(int index);
	inventoryItem* getItem(int index);
private:
	bool _closed = true;
	int _inventoryWidth;
	std::vector<inventoryItem*> _items;
};

