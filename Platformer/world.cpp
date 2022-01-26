#include "world.h"
#include "projectile.h"
#include <random>
#include <time.h>
#include <math.h>
#include <algorithm>
#include "perlinNoise.h"
#include "luaHandler.h"
#include "json/json.h"
#include "tileSpike.h"
#include "fatherTile.h"
#include "json/json-forwards.h"
#include <boost/algorithm/string.hpp>
#include <streambuf>


world::world(float gravity, gengine::camera2D* camera)
{
	_gravity = gravity;
	_Box2DWorld = new b2World(b2Vec2(0, gravity));
	_Box2DWorld->SetAllowSleeping(false);

	_contactListener = new contactListener;
	_contactListener->world = this;
	_Box2DWorld->SetContactListener(_contactListener);

	_dimensions = glm::ivec2(0, 0);

	_camera = camera;

	_lightSystem.initGLSLPrograms();
	_lightSystem.createBuffers();

	_damageLabels = new damageLabel[NUM_DAMAGE_LABELS];

	damageLabel::init();
}

world::~world()
{
	for (auto it : _entities) {
		it->destroy(this);
	}
	for (size_t i = 0; i < _tileGroups.size(); i++) {
		_deleteBatch(_tileGroups[i]);
	}
	for (int y = 0; y < _tiles.size(); y++) {
		for (int x = 0; x < _tiles[y].size(); x++) {
			delete _tiles[y][x];
		}
	}
	_tiles.clear();
}

void world::setGravity(float gravity) {
	_gravity = gravity;
	_Box2DWorld->SetGravity(b2Vec2(0, gravity));
}

void world::addEntity(entity* newEntity) {
	_entitiesToAdd.push_back(newEntity);
}

void world::deleteEntity(entity* toDelete) {
	for (size_t i = 0; i < _entitiesToDestroy.size(); i++) {
		if (toDelete == _entitiesToDestroy[i]) {
			return;
		}
	}
	toDelete->setActive(false);
	_entitiesToDestroy.push_back(toDelete);
}

void world::_checkForAddedEntities() {
	if (!_entitiesToAdd.empty()) {
		for (size_t i = 0; i < _entitiesToAdd.size(); i++) {
			_entities.push_back(_entitiesToAdd[i]);
			_entities.back()->update();
		}
		_entitiesToAdd.clear();
	}
}

void world::_checkForDeletedEntities() {
	if (!_entitiesToDestroy.empty()) {
		for (size_t i = 0; i < _entitiesToDestroy.size(); i++) {

			for (int y = 0; y < _entities.size(); y++) {

				if (_entitiesToDestroy[i] == _entities[y]) {
					_entities.erase(_entities.begin() + y);
				}
			}

			_entitiesToDestroy[i]->destroy(this);
			delete _entitiesToDestroy[i];
		}
		_entitiesToDestroy.clear();
	}
}

void world::createBody(b2BodyDef bodyDef, b2FixtureDef fixture, b2Body** bodyPointer, std::vector<b2Fixture*>* fixturesPointers, b2Vec2 beginImpulse) {
	bodyTemplate newBodyInfo;
	newBodyInfo._bodyDefinition = bodyDef;
	newBodyInfo._fixtures.push_back(fixture);

	//Get the shape of the fixture
	const b2Shape* newShape = newBodyInfo._fixtures[0].shape;

	//Find the type of the shape and copy the shape over in the vector
	if (newShape->m_type == b2Shape::e_circle) {
		const b2CircleShape* circleShape;
		circleShape = static_cast<const b2CircleShape*>(newShape);

		b2CircleShape* newBodyShape = new b2CircleShape;
		*newBodyShape = *circleShape;

		newBodyInfo._shapes.push_back(newBodyShape);
	}
	else if (newShape->m_type == b2Shape::e_edge) {
		const b2EdgeShape* edgeShape;
		edgeShape = static_cast<const b2EdgeShape*>(newShape);

		b2EdgeShape* newBodyShape = new b2EdgeShape;
		*newBodyShape = *edgeShape;

		newBodyInfo._shapes.push_back(newBodyShape);
	}
	else if (newShape->m_type == b2Shape::e_polygon) {
		const b2PolygonShape* polygonShape;
		polygonShape = static_cast<const b2PolygonShape*>(newShape);

		b2PolygonShape* newBodyShape = new b2PolygonShape;
		*newBodyShape = *polygonShape;

		newBodyInfo._shapes.push_back(newBodyShape);
	}
	else if (newShape->m_type == b2Shape::e_chain) {
		const b2ChainShape* chainShape;
		chainShape = static_cast<const b2ChainShape*>(newShape);

		b2ChainShape* newBodyShape = new b2ChainShape;
		*newBodyShape = *chainShape;

		newBodyInfo._shapes.push_back(newBodyShape);
	}


	newBodyInfo._objectsBody = bodyPointer;
	newBodyInfo._objectsFixtures = fixturesPointers;
	newBodyInfo._beginImpulse = beginImpulse;
	_toCreate.push_back(newBodyInfo);
}

void world::createBody(b2BodyDef bodyDef, std::vector<b2FixtureDef> fixtures, b2Body** bodyPointer, std::vector<b2Fixture*>* fixturesPointers, b2Vec2 beginImpulse) {
	bodyTemplate newBodyInfo;
	newBodyInfo._bodyDefinition = bodyDef;
	newBodyInfo._fixtures = fixtures;

	const b2Shape* newShape;

	for (b2FixtureDef it : newBodyInfo._fixtures) {
		//Get the shape of the fixture
		newShape = it.shape;

		//Find the type of the shape and copy the shape over in the vector
		if (newShape->m_type == b2Shape::e_circle) {
			const b2CircleShape* circleShape;
			circleShape = static_cast<const b2CircleShape*>(it.shape);

			b2CircleShape* newBodyShape = new b2CircleShape;
			*newBodyShape = *circleShape;

			newBodyInfo._shapes.push_back(newBodyShape);
		}
		else if (newShape->m_type == b2Shape::e_edge) {
			const b2EdgeShape* edgeShape;
			edgeShape = static_cast<const b2EdgeShape*>(it.shape);

			b2EdgeShape* newBodyShape = new b2EdgeShape;
			*newBodyShape = *edgeShape;

			newBodyInfo._shapes.push_back(newBodyShape);
		}
		else if (newShape->m_type == b2Shape::e_polygon) {
			const b2PolygonShape* polygonShape;
			polygonShape = static_cast<const b2PolygonShape*>(it.shape);

			b2PolygonShape* newBodyShape = new b2PolygonShape;
			*newBodyShape = *polygonShape;

			newBodyInfo._shapes.push_back(newBodyShape);
		}
		else if (newShape->m_type == b2Shape::e_chain) {
			const b2ChainShape* chainShape;
			chainShape = static_cast<const b2ChainShape*>(it.shape);

			b2ChainShape* newBodyShape = new b2ChainShape;
			*newBodyShape = *chainShape;

			newBodyInfo._shapes.push_back(newBodyShape);
		}
	}

	newBodyInfo._objectsBody = bodyPointer;
	newBodyInfo._objectsFixtures = fixturesPointers;
	newBodyInfo._beginImpulse = beginImpulse;
	_toCreate.push_back(newBodyInfo);
}

void world::deleteBody(b2Body* toDelete) {
	_bodiesToDestroy.push_back(toDelete);
}

void world::_checkForAddedBodies() {
	if (!_toCreate.empty()) {
		for (bodyTemplate it : _toCreate) {
			it._createdBody = _Box2DWorld->CreateBody(&it._bodyDefinition);
			for (size_t i = 0; i < it._fixtures.size(); i++) {
				it._fixtures[i].shape = it._shapes[i];
				if (it._objectsFixtures != nullptr) {
					it._objectsFixtures->push_back(it._createdBody->CreateFixture(&(it._fixtures[i])));
				}
				delete it._shapes[i];
			}
			if (it._objectsBody != nullptr) {
				*it._objectsBody = it._createdBody;
			}
			it._createdBody->ApplyLinearImpulse(it._beginImpulse,b2Vec2(0,0), false);
			it._createdBody->SetAngularVelocity(0);
		}
		_toCreate.clear();
	}
}

void world::_checkForDeletedBodies() {
	if (!_bodiesToDestroy.empty()) {
		for (b2Body* it : _bodiesToDestroy) {
			for (b2Fixture* f = it->GetFixtureList(); f; f = f->GetNext())
			{
				if (f->GetUserData() != nullptr) {
					delete f->GetUserData();
					f->SetUserData(nullptr);
				}
			}
			_Box2DWorld->DestroyBody(it);
		}
		_bodiesToDestroy.clear();
	}
}

bool world::addTileIngame(definition * definition, bool front, int gridX, int gridY)
{
	if (_insideMap(gridX, gridY)) {
		int worldX = modulo(gridX, _dimensions.x);
		int worldY = gridY;

		if (definition->getDefinitionType() == definitionType::MATERIAL) {
			return _addRegularTileIngame((material*)definition,front, worldX, worldY);
		}
		else if (definition->getDefinitionType() == definitionType::PLACABLE_ITEM) {
			return _addPlacableItemIngame((placableItem*)definition, worldX, worldY);
		}
	}
	else {
		return false;
	}
}

bool world::_addRegularTileIngame(material * definition, bool front, int gridX, int gridY)
{
	if (_regularTileCanBePlaced(front, gridX, gridY)) {
		if (_tiles[gridY][gridX] == nullptr) {
			_tiles[gridY][gridX] = new regularTile();
		}
		_tiles[gridY][gridX]->init(gridX, gridY, definition, front, _getAutoTileCount(front, gridX, gridY));
		_setAutoTileCountAround(front, gridX, gridY);

		if (front) {
			_generateMeshesOfRegion(gridX, gridY);
		}
		return true;
	}
	else {
		return false;
	}
}

bool world::_addPlacableItemIngame(placableItem* definition, int gridX, int gridY) {
	replaceForFatherTile(gridX, gridY);
	_tiles[gridY][gridX]->init(gridX, gridY, definition, true, _getAutoTileCount(true, gridX, gridY));

	return true;
}

void world::deleteTileIngame(bool front, int gridX, int gridY)
{
	if (_insideMap(gridX, gridY)) {
		int worldX = modulo(gridX, _dimensions.x);
		int worldY = gridY;

		if (_tiles[worldY][worldX] != nullptr) {
			_tiles[worldY][worldX]->destroy(front, true);

			_checkSupportAround(worldX, worldY);

			if (front) {
				_generateMeshesOfRegion(worldX, worldY);
			}
		}
	}
}

void world::addTileModIngame(materialMod * definition, bool front, int gridX, int gridY)
{
	tile* observedTile = _tiles[gridY][gridX];
	if (observedTile != nullptr) {
		if (front) {
			if (observedTile->hasFront()) {
				if (observedTile->getType() == REGULAR_TILE) {
					observedTile->addMaterialMod(definition, true);
				}
			}
		}
		else {
			if (observedTile->hasBack()) {
				observedTile->addMaterialMod(definition, false);
			}
		}
	}
}

void world::_addTileGen(definition * definition, bool front, int gridX, int gridY)
{
	if (_insideMap(gridX, gridY)) {
		int worldX = modulo(gridX, _dimensions.x);
		int worldY = gridY;

		if (definition->getDefinitionType() == definitionType::MATERIAL) {
			_addRegularTileGen((material*)definition, front, worldX, worldY);
		}
		else if (definition->getDefinitionType() == definitionType::PLACABLE_ITEM) {
			_addPlacableItemGen((placableItem*)definition, worldX, worldY);
		}
	}
}

void world::_addRegularTileGen(material * definition, bool front, int gridX, int gridY)
{
	if (_regularTileCanBePlaced(front, gridX, gridY)) {
		if (_tiles[gridY][gridX] == nullptr) {
			_tiles[gridY][gridX] = new regularTile();
		}

		_tiles[gridY][gridX]->init(gridX, gridY, definition, front, 0);
	}
}

void world::_addPlacableItemGen(placableItem* definition, int gridX, int gridY) {
	replaceForFatherTile(gridX, gridY);
	_tiles[gridY][gridX]->init(gridX, gridY, definition, true, _getAutoTileCount(true, gridX, gridY));
}

void world::_addTileModGen(materialMod* definition, bool front, int gridX, int gridY) {
	tile* observedTile = _tiles[gridY][gridX];
	if (observedTile != nullptr) {
		if (front) {
			if (observedTile->hasFront()) {
				if (observedTile->getType() == REGULAR_TILE) {
					observedTile->addMaterialMod(definition, true);
				}
			}
		}
		else {
			if (observedTile->hasBack()) {
				observedTile->addMaterialMod(definition, false);
			}

		}
	}
}

void world::_deleteTileGen(bool front, int gridX, int gridY)
{
	if (_insideMap(gridX, gridY)) {
		int worldX = modulo(gridX, _dimensions.x);
		int worldY = gridY;

		if (_tiles[worldY][worldX] != nullptr) {
			_tiles[worldY][worldX]->destroy(front, false);
		}
	}
}

void world::_handlePostGen() {
	/*for (int y = 0; y < _dimensions.y; y++) {
		for (int x = 0; x < _dimensions.x; x++) {
			if (!_isSupported(x,y)) {
				_deleteTileGen(true, x, y);
			}
		}
	}*/

	for (int y = 0; y < _dimensions.y; y++) {
		for (int x = 0; x < _dimensions.x; x++) {
			if (_tiles[y][x] != nullptr) {
				if (_tiles[y][x]->getType() == tileType::REGULAR_TILE) {
					_tiles[y][x]->setAutoTileCount(true, _getAutoTileCount(true, x, y));
				}
				_tiles[y][x]->setAutoTileCount(false, _getAutoTileCount(false, x, y));
			}
		}
	}

	_generateMeshes();
}

bool world::_regularTileCanBePlaced(bool front, int gridX, int gridY)
{
	if (front) {
		return (_tiles[gridY][gridX] == nullptr || _tiles[gridY][gridX]->getFrontMaterial() == nullptr);
	}
	else {
		return (_tiles[gridY][gridX] == nullptr || _tiles[gridY][gridX]->getBackMaterial() == nullptr);
	}
}

void world::_setAutoTileCountAround(bool front, int gridX, int gridY) {
	tile* observedTile = getTile(gridX, gridY + 1);
	if (observedTile != nullptr) {
		observedTile->setAutoTileCount(front,_getAutoTileCount(front, gridX, gridY + 1));
	}
	observedTile = getTile(gridX + 1, gridY);
	if (observedTile != nullptr) {
		observedTile->setAutoTileCount(front, _getAutoTileCount(front, gridX + 1, gridY));
	}
	observedTile = getTile(gridX, gridY - 1);
	if (observedTile != nullptr) {
		observedTile->setAutoTileCount(front, _getAutoTileCount(front, gridX, gridY - 1));
	}
	observedTile = getTile(gridX - 1, gridY);
	if (observedTile != nullptr) {
		observedTile->setAutoTileCount(front, _getAutoTileCount(front, gridX - 1, gridY));
	}
}

worldTemplate world::getStructure(std::string filePath) {

	//Definition pointed / index
	std::map<int, definition*> materialList;

	std::ifstream in(filePath, std::ifstream::binary);

	Json::Value root;
	in >> root;

	unsigned int width = root["width"].asInt();
	unsigned int height = root["height"].asInt();

	worldTemplate returnedTemplate(width,height);

	//Loop trough the tilesets and give a list of materials associated to the tile ID
	for (int i = 0; root["tilesets"][i] != Json::nullValue; i++) {

		int firstgid = root["tilesets"][i]["firstgid"].asInt();
		//For each material
		for (Json::ValueIterator itr = root["tilesets"][i]["tileproperties"].begin(); itr != root["tilesets"][i]["tileproperties"].end(); itr++) {
			definition* def = luaHandler::getDefinition((*itr)["nameID"].asString());
			int index = std::stoi(itr.key().asString()) + firstgid;

			if (luaHandler::validateDefinition(def)) {
				materialList.emplace(std::make_pair(index, def));
			}
		}
	}

	//Set the materials for the tileTemplates
	for (int i = 0; root["layers"][i] != Json::nullValue; i++) {
		Json::Value layer = root["layers"][i];
		if (layer["name"] == "front") {
			for (int p = 0; p < width * height; p++) {
				int x = p % width;
				int y = height - (int)(p / width) - 1;

				if (layer["data"][p] != 0) {
					definition* frontMaterial = materialList[layer["data"][p].asInt()];

					if (frontMaterial->getDefinitionType() == definitionType::MATERIAL || frontMaterial->getDefinitionType() == definitionType::PLACABLE_ITEM) {
						returnedTemplate.setFrontMaterial(x,y,frontMaterial);
					}
				}
			}
		}
		else if (layer["name"] == "back") {
			for (int p = 0; p < width * height; p++) {
				int x = p % width;
				int y = height - (int)(p / width) - 1;

				if (layer["data"][p] != 0) {
					definition* backMaterial = materialList[layer["data"][p].asInt()];

					if (backMaterial->getDefinitionType() == definitionType::MATERIAL) {
						returnedTemplate.setBackMaterial(x, y, (material*)backMaterial);
					}
				}
			}
		}
		else if (layer["name"] == "front_material_mods") {
			for (int p = 0; p < width * height; p++) {
				int x = p % width;
				int y = height - (int)(p / width) - 1;

				if (layer["data"][p] != 0) {
					definition* frontMaterialMod = materialList[layer["data"][p].asInt()];

					if (frontMaterialMod->getDefinitionType() == definitionType::MATERIAL_MOD) {
						returnedTemplate.setFrontMaterialMod(x, y, (materialMod*)frontMaterialMod);
					}
				}
			}
		}
		else if (layer["name"] == "back_material_mods") {
			for (int p = 0; p < width * height; p++) {
				int x = p % width;
				int y = height - (int)(p / width) - 1;

				if (layer["data"][p] != 0) {
					definition* backMaterialMod = materialList[layer["data"][p].asInt()];

					if (backMaterialMod->getDefinitionType() == definitionType::MATERIAL_MOD) {
						returnedTemplate.setBackMaterialMod(x, y, (materialMod*)backMaterialMod);
					}
				}
			}
		}
		else if (layer["name"] == "empty_front") {
			for (int p = 0; p < width * height; p++) {
				int x = p % width;
				int y = height - (int)(p / width) - 1;

				if (layer["data"][p] != 0) {
					returnedTemplate.setEmptyFront(x, y,true);
				}
			}
		}
		else if (layer["name"] == "empty_back") {
			for (int p = 0; p < width * height; p++) {
				int x = p % width;
				int y = height - (int)(p / width) - 1;

				if (layer["data"][p] != 0) {
					returnedTemplate.setEmptyBack(x, y, true);
				}
			}
		}
	}

	return returnedTemplate;
}

void world::expandWorldToTile(unsigned int x, unsigned int y)
{
	if (y > _dimensions.y) {
		_dimensions.y = y;

		std::vector<tile*> newVec;
		_tiles.resize(y, newVec);
	}
	if (x > _dimensions.x) {
		_dimensions.x = x;

		for (int y = 0; y < _tiles.size(); y++) {
			_tiles[y].resize(x, nullptr);
		}
	}

}

int world::modulo(int a, unsigned int b)
{
	if (a >= 0) {
		return a%b;
	}
	else{
		int div = (int)(std::floor((float)a / (float)b) * -1);
		int newA = a + div * b;
		return newA%b;
	}
}

tile* world::getTile(int gridX, int gridY) {
	if (gridY >= 0 && gridY < _dimensions.y) {
		int worldX = modulo(gridX, _dimensions.x);
		int worldY = gridY;

		return _tiles[worldY][worldX];
	}
	else {
		return nullptr;
	}
}

tile* world::getTile(glm::vec2 point) {
	if (point.x >= 0 && point.y >= 0 && point.x <= _dimensions.x * globals::pixToMet(globals::TILE_SIZE) && point.y <= _dimensions.y * globals::pixToMet(globals::TILE_SIZE)) {
		int y = (int)(point.y / globals::pixToMet(globals::TILE_SIZE));
		int x = (int)(point.x / globals::pixToMet(globals::TILE_SIZE));
		return _tiles[y][x];
	}
	else {
		return nullptr;
	}
}

glm::vec2 world::getWorldCoordsDimensions()
{
	return glm::vec2(_dimensions.x * globals::pixToMet(globals::TILE_SIZE), _dimensions.y * globals::pixToMet(globals::TILE_SIZE));
}


void world::addEvent(std::string nameID, int duration) {
	if (!luaHandler::globalValueExists(nameID)) {
		return;
	}
	event* newEvent = new event(nameID, duration);
	_events.push_back(newEvent);
}

void world::addEvent(std::string nameID) {
	if (!luaHandler::globalValueExists(nameID)) {
		return;
	}
	for (int i = 0; i < _events.size(); i++) {
		if (_events[i]->getNameID() == nameID) {
			return;
		}
	}
	event* newEvent = new event(nameID, 0);
	_events.push_back(newEvent);
}

void world::deleteEvent(std::string nameID) {
	for (int i = 0; i < _events.size(); i++) {
		if (_events[i]->getNameID() == nameID) {
			_events[i]->LUA_EVENT_whenDisactivated();
			delete _events[i];
			_events.erase(_events.begin() + i);
			break;
		}
	}
}

void world::updateEvents() {
	for (int i = 0; i < _events.size(); i++) {
		_events[i]->update();
	}
}

void world::addDamageLabel(glm::vec2 position, int value, gengine::color color) {
	for (int i = 0; i < NUM_DAMAGE_LABELS; i++) {
		if (!_damageLabels[i].isActive()) {
			_damageLabels[i].activate(position, value, color);
			return;
		}
	}
}

void world::addDamageLabel(glm::vec2 position, std::string value, gengine::color color) {
	for (int i = 0; i < NUM_DAMAGE_LABELS; i++) {
		if (!_damageLabels[i].isActive()) {
			_damageLabels[i].activate(position, value, color);
			return;
		}
	}
}

void world::addTilesFromTemplate(worldTemplate& worldTemplate, int gridX, int gridY) {
	expandWorldToTile(gridX + worldTemplate.getWidth(), gridY + worldTemplate.getHeight());

	for (int y = 0; y < worldTemplate.getHeight(); y++) {
		for (int x = 0; x < worldTemplate.getWidth(); x++) {
			if (worldTemplate.getEmptyFront(x, y)) {
				_deleteTileGen(true, x + gridX, y + gridY);
			}
			if (worldTemplate.getEmptyBack(x, y)) {
				_deleteTileGen(false, x + gridX, y + gridY);
			}
		}
	}

	for (int y = 0; y < worldTemplate.getHeight(); y++) {
		for (int x = 0; x < worldTemplate.getWidth(); x++) {
			if (worldTemplate.getFrontMaterial(x,y) != nullptr) {
				_addTileGen(worldTemplate.getFrontMaterial(x, y), true, x + gridX, y + gridY);

				if (worldTemplate.getFrontMaterialMod(x, y) != nullptr) {
					_addTileModGen(worldTemplate.getFrontMaterialMod(x, y), true, x + gridX, y + gridY);
				}
			}
			if (worldTemplate.getBackMaterial(x,y) != nullptr) {
				_addTileGen(worldTemplate.getBackMaterial(x, y), false, x + gridX, y + gridY);

				if (worldTemplate.getBackMaterialMod(x, y) != nullptr) {
					_addTileModGen(worldTemplate.getBackMaterialMod(x, y), false, x + gridX, y + gridY);
				}
			}
		}
	}

	_handlePostGen();
}

regularTile* world::replaceForRegularTile(int gridX, int gridY){
	if (_insideMap(gridX, gridY)) {
		int worldX = modulo(gridX, _dimensions.x);
		int worldY = gridY;

		regularTile* newTile = new regularTile();

		if (_tiles[gridY][gridX] != nullptr) {
			newTile->init(worldX, worldY, _tiles[worldY][worldX]->getBackMaterial(), false, _getAutoTileCount(false, worldX, worldY));
			newTile->setBackHealth(_tiles[worldY][worldX]->getBackHealth());
			if (_tiles[gridY][gridX]->getBackModInstance() != nullptr) {
				newTile->addMaterialMod(_tiles[worldY][worldX]->getBackModInstance()->getMaterialMod(), false);
			}
			delete _tiles[worldY][worldX];
		}

		_tiles[worldY][worldX] = newTile;
		return newTile;
	}
}

fatherTile* world::replaceForFatherTile(int gridX, int gridY)
{
	if (_insideMap(gridX, gridY)) {
		int worldX = modulo(gridX, _dimensions.x);
		int worldY = gridY;

		fatherTile* newTile = new fatherTile();

		if (_tiles[gridY][gridX] != nullptr) {
			newTile->init(worldX, worldY, _tiles[worldY][worldX]->getBackMaterial(), false, _getAutoTileCount(false, worldX, worldY));
			newTile->setBackHealth(_tiles[worldY][worldX]->getBackHealth());
			if (_tiles[gridY][gridX]->getBackModInstance() != nullptr) {
				newTile->addMaterialMod(_tiles[worldY][worldX]->getBackModInstance()->getMaterialMod(), false);
			}
			delete _tiles[worldY][worldX];
		}

		_tiles[worldY][worldX] = newTile;
		return newTile;
	}
}

childTile* world::replaceForChildTile(int gridX, int gridY)
{
	if (_insideMap(gridX, gridY)) {
		int worldX = modulo(gridX, _dimensions.x);
		int worldY = gridY;

		childTile* newTile = new childTile();

		if (_tiles[gridY][gridX] != nullptr) {
			newTile->init(worldX, worldY, _tiles[worldY][worldX]->getBackMaterial(), false, _getAutoTileCount(false, worldX, worldY));
			newTile->setBackHealth(_tiles[worldY][worldX]->getBackHealth());
			if (_tiles[gridY][gridX]->getBackModInstance() != nullptr) {
				newTile->addMaterialMod(_tiles[worldY][worldX]->getBackModInstance()->getMaterialMod(), false);
			}
			delete _tiles[worldY][worldX];
		}

		_tiles[worldY][worldX] = newTile;
		return newTile;
	}
}

void world::replaceForEmptyTile(int gridX, int gridY)
{
	if (_insideMap(gridX, gridY)) {
		if (_tiles[gridY][gridX] != nullptr) {
			int worldX = modulo(gridX, _dimensions.x);
			int worldY = gridY;

			delete _tiles[worldY][worldX];
			_tiles[worldY][worldX] = nullptr;
		}
	}
}

void world::generate(worldGenerator* worldGenerator, int fromX, int toX, int seed) {
	worldTemplate generatedTemplate = worldGenerator->getGeneratedWorldPortion(toX - fromX, 100, seed);

	addTilesFromTemplate(generatedTemplate, fromX, 0);
}

bool world::_insideMap(int gridX, int gridY) {
	return (gridY >= 0 && gridY < _dimensions.y);
}

void world::addDrop(inventoryItem* toAdd) {
	_drops.push_back(toAdd);
}

void world::deleteDrop(inventoryItem* toDelete) {
	for (size_t i = 0; i < _drops.size(); i++) {
		if (toDelete == _drops[i]) {
			_drops[i] = _drops.back();
			_drops.pop_back();
			return;
		}
	}
}

void world::_deleteBatch(tileHitbox* batchToDelete) {
	if (batchToDelete != nullptr) {
		for (size_t i = 0; i < _tileGroups.size(); i++) {
			if (_tileGroups[i] == batchToDelete) {
				_tileGroups[i] = _tileGroups.back();
				_tileGroups.pop_back();
				break;
			}
		}

		batchToDelete->destroy(this);
	}
}

bool world::_meshable(int gridX, int gridY) {
	if (gridY < 0 || gridY > _dimensions.y - 1 || gridX < 0 || gridX > _dimensions.x - 1) {
		return false;
	}
	else if (_tiles[gridY][gridX] == nullptr) {
		return false;
	}
	else if (_tiles[gridY][gridX]->isInMesh() == false && _tiles[gridY][gridX]->isSolid() && _tiles[gridY][gridX]->hasFront()) {
		return true;
	}
	else {
		return false;
	}
}

bool world::_frontEmpty(int gridX, int gridY) {
	tile* observedTile = getTile(gridX, gridY);

	if (observedTile != nullptr) {
		if (observedTile->hasFront() && observedTile->getType() == tileType::REGULAR_TILE) {
			return false;
		}
		else {
			return true;
		}
	}
	else {
		return true;
	}
}

//Generates the meshes for the entire world
void world::_generateMeshes() {
	for (int y = 0; y < _dimensions.y; y += globals::TILE_BATCH_SIZE) {
		for (int x = 0; x < _dimensions.x; x += globals::TILE_BATCH_SIZE) {
			glm::ivec2 bl;
			glm::ivec2 tr;
			_getMaxBatchSquare(x, y, bl, tr);
			_generateMeshes(bl, tr);
		}
	}
}
void world::_generateMeshes(glm::ivec2 bottomLeft, glm::ivec2 topRight) {
	glm::ivec2 tl;
	glm::ivec2 br;

	for (int y = bottomLeft.y; y <= topRight.y; y++) {
		for (int x = bottomLeft.x; x <= topRight.x; x++) {
			tile* observedTile = getTile(x, y);
			if (observedTile != nullptr) {
				if (observedTile->isInMesh()) {
					_deleteBatch(observedTile->getBatch());
				}
			}
		}
	}

	//Create tile spikes
	for (int y = bottomLeft.y; y <= topRight.y; y++) {
		for (int x = bottomLeft.x; x <= topRight.x; x++) {
			tile* thisTile = getTile(x, y);

			if (thisTile != nullptr) {
				if (_meshable(x, y) == true) {
					if (_frontEmpty(x, y + 1)) {
						int x1 = x;
						int y1 = y;

						//Check for the need for a tilespike
						if (_meshable(x - 1, y -1) && _meshable(x + 1, y - 1) && _frontEmpty(x1 - 1, y1) && _frontEmpty(x1 + 1, y1)) {
							//If it cannot form a slope upwards
							if ((!_meshable(x - 1, y + 1) && !_meshable(x + 1, y + 1)) || (_meshable(x - 1, y + 1) && !_frontEmpty(x1 - 1, y1 + 2)) || (_meshable(x + 1, y + 1) && !_frontEmpty(x1 + 1, y1 + 2))) {
								glm::vec2 leftStartingPoint;
								glm::vec2 rightStartingPoint;

								while (getTile(x1 - 1, y1 - 1) != nullptr && _meshable(x1 - 1, y1 - 1) && _frontEmpty(x1 - 1, y1)) {
									x1 -= 1;
									y1 -= 1;
								}
								leftStartingPoint = glm::vec2(x1 * globals::pixToMet(globals::TILE_SIZE), (y1 + 1) * globals::pixToMet(globals::TILE_SIZE));

								x1 = x;
								y1 = y;
								while (getTile(x1 + 1, y1 - 1) != nullptr && _meshable(x1 + 1, y1 - 1) && _frontEmpty(x1 + 1, y1)) {
									x1 += 1;
									y1 -= 1;
								}
								rightStartingPoint = glm::vec2((x1 + 1) * globals::pixToMet(globals::TILE_SIZE), (y1 + 1) * globals::pixToMet(globals::TILE_SIZE));

								tileSpike* newSpike = new tileSpike;
								newSpike->init(this, glm::ivec2(x, y), leftStartingPoint, rightStartingPoint, getTile(x, y));
								_tileGroups.push_back(newSpike);
							}
						}
					}

				}
			}
		}
	}

	//Create slopes and put them in a mesh
	for (int y = bottomLeft.y; y <= topRight.y; y++) {
		for (int x = bottomLeft.x; x <= topRight.x; x++) {

			tile* thisTile = getTile(x, y);

			if (thisTile != nullptr) {
				if (_meshable(x, y) == true) {

					if (_frontEmpty(x, y + 1)) {
						int x1 = x;
						int y1 = y;

						std::vector<tile*> meshedTiles;

						//Top right slope
						{
							while (_meshable(x1 + 1, y1 + 1) && _frontEmpty(x1 + 1, y1 + 2)) {
								x1 += 1;
								y1 += 1;
								meshedTiles.push_back(getTile(x1, y1));
							}

							if (!_meshable(x1 + 1, y1 + 1) && !_frontEmpty(x1 + 1, y1 + 1) && getTile(x1 + 1, y1 + 1)->getBatch() != nullptr && typeid(*getTile(x1 + 1, y1 + 1)->getBatch()).name() == typeid(tileSpike).name()) {
								y1 += 1;
							}

							if (!meshedTiles.empty()) {
								glm::vec2 startingPoint(x * globals::pixToMet(globals::TILE_SIZE), y * globals::pixToMet(globals::TILE_SIZE) + globals::pixToMet(globals::TILE_SIZE));
								glm::vec2 endingPoint(x1 * globals::pixToMet(globals::TILE_SIZE) + globals::pixToMet(globals::TILE_SIZE), y1 * globals::pixToMet(globals::TILE_SIZE) + globals::pixToMet(globals::TILE_SIZE));

								for (int i = 0; i < meshedTiles.size(); i++) {
									tileSlope* newSlope = new tileSlope;
									newSlope->init(this, meshedTiles[i]->getTilePosition(), startingPoint, endingPoint, meshedTiles[i]);
									_tileGroups.push_back(newSlope);
								}
								meshedTiles.clear();
							}

						}

						x1 = x;
						y1 = y;

						//Top left slope
						{
							while (_meshable(x1 - 1, y1 + 1) && _frontEmpty(x1 - 1, y1 + 2)) {
								x1 -= 1;
								y1 += 1;
								meshedTiles.push_back(getTile(x1, y1));
							}

							if (!_meshable(x1 - 1, y1 + 1) && !_frontEmpty(x1 - 1, y1 + 1) && getTile(x1 - 1, y1 + 1)->getBatch() != nullptr && typeid(*getTile(x1 - 1, y1 + 1)->getBatch()).name() == typeid(tileSpike).name()) {
								y1 += 1;
							}

							if (!meshedTiles.empty()) {
								glm::vec2 startingPoint(x * globals::pixToMet(globals::TILE_SIZE) + globals::pixToMet(globals::TILE_SIZE), y * globals::pixToMet(globals::TILE_SIZE) + globals::pixToMet(globals::TILE_SIZE));
								glm::vec2 endingPoint(x1 * globals::pixToMet(globals::TILE_SIZE), y1 * globals::pixToMet(globals::TILE_SIZE) + globals::pixToMet(globals::TILE_SIZE));

								for (int i = 0; i < meshedTiles.size(); i++) {
									tileSlope* newSlope = new tileSlope;
									newSlope->init(this, meshedTiles[i]->getTilePosition(), startingPoint, endingPoint, meshedTiles[i]);
									_tileGroups.push_back(newSlope);
								}
								meshedTiles.clear();
							}							
						}
					}

				}
			}
		}
	}

	//Create tile batches
	for (int y = bottomLeft.y; y <= topRight.y; y++) {
		for (int x = bottomLeft.x; x <= topRight.x; x++) {
			if (_meshable(x, y)) {
				//The two tiles that represent the top left and bottom right end of the mesh
				tl = glm::vec2(x, y);
				br = glm::vec2(x, y);

				//Expand top left tile in the x (-)
				while (_meshable(tl.x - 1, tl.y) && tl.x > bottomLeft.x) {
					tl.x -= 1;
				}

				//Expand bottom right tile in the x (+)
				while (_meshable(br.x + 1, br.y) && br.x < topRight.x) {
					br.x += 1;
				}

				bool end = false;
				bool completeLine = true;

				//Expand the rectangle upwards until there is no rectangle to be formed with the found x coordinates
				while (end == false) {
					for (int nx = tl.x; nx <= br.x; nx++) {
						if (!_meshable(nx, tl.y + 1)) {
							end = true;
							completeLine = false;
							break;
						}
					}

					if (completeLine) {
						if (tl.y + 1 <= topRight.y) {
							tl.y += 1;
						}
						else {
							end = true;
						}
					}

				}

				end = false;

				//Add the tileBatch
				tileBatch* newBatch = new tileBatch;

				std::vector<tile*> occupied;

				for (int ny = br.y; ny <= tl.y; ny++) {
					for (int nx = tl.x; nx <= br.x; nx++) {
						occupied.push_back(_tiles[ny][nx]);
					}
				}
				newBatch->init(this, tl, br, occupied);

				_tileGroups.push_back(newBatch);


			}

		}

	}
}

void world::_generateMeshesOfRegion(int gridX, int gridY)
{
	glm::ivec2 bottomLeft;
	glm::ivec2 topRight;
	_getMaxBatchSquare(gridX, gridY, bottomLeft, topRight);
	_generateMeshes(bottomLeft, topRight);
	_modified = true;
}

void world::setChanged()
{
	_modified = true;
}

bool world::hasChanged()
{
	return _modified;
}

void world::addToAmbientLightColor(glm::vec4& color) {
	_ambientLightColor.r += color.r;
	_ambientLightColor.g += color.g;
	_ambientLightColor.b += color.b;
}

glm::vec4 world::getAmbientLightColor()
{
	return _ambientLightColor;
}

void world::_getMaxBatchSquare(int gridX, int gridY, glm::ivec2 &bottomLeft, glm::ivec2 &topRight) {
	bottomLeft.x = (int)(gridX / globals::TILE_BATCH_SIZE) * globals::TILE_BATCH_SIZE;
	bottomLeft.y = (int)(gridY / globals::TILE_BATCH_SIZE) * globals::TILE_BATCH_SIZE;
	topRight.x = (int)((gridX / globals::TILE_BATCH_SIZE + 1) * globals::TILE_BATCH_SIZE) - 1;
	topRight.y = (int)((gridY / globals::TILE_BATCH_SIZE + 1) * globals::TILE_BATCH_SIZE) - 1;

	if (topRight.x >= _dimensions.x) {
		topRight.x = _dimensions.x - 1;
	}
	if (topRight.y >= _dimensions.y) {
		topRight.y = _dimensions.y - 1;
	}
}

//Returns the tile position from a point. If the point is outside of the tile map, it returns the extremity tile
glm::ivec2 world::_getTilePosition(float x, float y) {
	glm::ivec2 returnedPos;
	returnedPos.x = x / globals::pixToMet(globals::TILE_SIZE);
	if (returnedPos.x < 0) {
		returnedPos.x = 0;
	}
	else if (returnedPos.x > _dimensions.x - 1) {
		returnedPos.x = _dimensions.x - 1;
	}
	returnedPos.y = y / globals::pixToMet(globals::TILE_SIZE);
	if (returnedPos.y < 0) {
		returnedPos.y = 0;
	}
	else if (returnedPos.y > _dimensions.y - 1) {
		returnedPos.y = _dimensions.y - 1;
	}

	return returnedPos;
}

int world::_getAutoTileCount(bool front, int gridX, int gridY) {
	int count = 0;
	if (front) {
		tile* observedTile = getTile(gridX, gridY + 1);
		if (observedTile != nullptr) {
			if (observedTile->getType() == tileType::REGULAR_TILE && observedTile->hasFront()) {
				count += 1;
			}
		}

		observedTile = getTile(gridX + 1, gridY);
		if (observedTile != nullptr) {
			if (observedTile->getType() == tileType::REGULAR_TILE && observedTile->hasFront()) {
				count += 2;
			}
		}

		observedTile = getTile(gridX, gridY - 1);
		if (observedTile != nullptr) {
			if (observedTile->getType() == tileType::REGULAR_TILE && observedTile->hasFront()) {
				count += 4;
			}
		}

		observedTile = getTile(gridX - 1, gridY);
		if (observedTile != nullptr) {
			if (observedTile->getType() == tileType::REGULAR_TILE && observedTile->hasFront()) {
				count += 8;
			}
		}
	}
	else {
		tile* observedTile = getTile(gridX, gridY + 1);
		if (observedTile != nullptr) {
			if (observedTile->hasBack()) {
				count += 1;
			}
		}

		observedTile = getTile(gridX + 1, gridY);
		if (observedTile != nullptr) {
			if (observedTile->hasBack()) {
				count += 2;
			}
		}

		observedTile = getTile(gridX, gridY - 1);
		if (observedTile != nullptr) {
			if (observedTile->hasBack()) {
				count += 4;
			}
		}

		observedTile = getTile(gridX - 1, gridY);
		if (observedTile != nullptr) {
			if (observedTile->hasBack()) {
				count += 8;
			}
		}
	}
	return count;
}

void world::_checkSupportAround(int gridX, int gridY) {
	if (!_isSupported(gridX, gridY)) {
		deleteTileIngame(true, gridX, gridY);
	}
	if (!_isSupported(gridX + 1, gridY)) {
		deleteTileIngame(true, gridX + 1, gridY);
	}
	if (!_isSupported(gridX - 1, gridY)) {
		deleteTileIngame(true, gridX - 1, gridY);
	}
	if (!_isSupported(gridX, gridY + 1)) {
		deleteTileIngame(true, gridX, gridY + 1);
	}
	if (!_isSupported(gridX, gridY - 1)) {
		deleteTileIngame(true, gridX, gridY - 1);
	}
}

bool world::_isSupported(int gridX, int gridY) {
	tile* observedTile = getTile(gridX, gridY);
	//If there is no tile, it is supported
	if (observedTile == nullptr) {
		return true;
	}
	else {
		//If it is a child tile or a father tile, it might not be supported anymore
		if (observedTile->getType() == tileType::CHILD_TILE) {
			childTile* castTile = static_cast<childTile*>(observedTile);
			fatherTile* childsFather = castTile->getFatherTile();

			//If the item was not already deleted
			if (childsFather != nullptr) {
				return childsFather->getFrontItem()->canBePlaced(this, childsFather->getTilePosition().x, childsFather->getTilePosition().y);
			}
			else {
				return true;
			}
		}
		else if (observedTile->getType() == tileType::FATHER_TILE) {
			fatherTile* castTile = static_cast<fatherTile*>(observedTile);

			//If the item was not already deleted
			if (castTile->getFrontItem() != nullptr) {
				return castTile->getFrontItem()->canBePlaced(this, castTile->getTilePosition().x, castTile->getTilePosition().y);
			}
			else {
				return true;
			}
		}
		else {
			return true;
		}
	}
}

void world::step(float dt, int velocityIterations, int positionIterations) {
	_Box2DWorld->Step(dt, velocityIterations, positionIterations);
}

void world::update(gengine::camera2D* camera) {
	glm::vec2 cameraDimensions = camera->getDimensions() / glm::vec2(camera->getScale());
	_ambientLightColor = glm::vec4(0, 0, 0, 0);

	_checkForAddedBodies();
	_checkForDeletedBodies();
	_checkForAddedEntities();
	_checkForDeletedEntities();

	for (size_t i = 0; i < _entities.size(); i++) {
		if (_entities[i]->isActive()) {
			_entities[i]->update();
		}
	}

	_checkForAddedBodies();
	_checkForDeletedBodies();
	_checkForAddedEntities();
	_checkForDeletedEntities();

	if (camera->getPosition().y < globals::pixToMet(camera->getDimensions().y / 2)) {
		camera->setPosition(glm::vec2(camera->getPosition().x, globals::pixToMet(camera->getDimensions().y / 2)));
	}
	else if (camera->getPosition().y > (_dimensions.y * globals::pixToMet(globals::TILE_SIZE)) - globals::pixToMet(camera->getDimensions().y / 2)) {
		camera->setPosition(glm::vec2(camera->getPosition().x, (_dimensions.y * globals::pixToMet(globals::TILE_SIZE)) - globals::pixToMet(camera->getDimensions().y / 2)));
	}

	//Center position of the parameter
	glm::vec2 cameraPosition(camera->getPosition());
	glm::ivec2 topLeftCameraCorner;
	glm::ivec2 bottomRightCameraCorner;

	topLeftCameraCorner = _getTilePosition(cameraPosition.x - cameraDimensions.x / 2 * globals::pixToMet(globals::TILE_SIZE), cameraPosition.y + cameraDimensions.y * globals::pixToMet(globals::TILE_SIZE));
	bottomRightCameraCorner = _getTilePosition(cameraPosition.x + cameraDimensions.x * globals::pixToMet(globals::TILE_SIZE), cameraPosition.y - cameraDimensions.y / 2 * globals::pixToMet(globals::TILE_SIZE));

	for (int y = bottomRightCameraCorner.y; y <= topLeftCameraCorner.y; y++) {
		for (int x = topLeftCameraCorner.x; x <= bottomRightCameraCorner.x; x++) {
			if (getTile(x,y) != nullptr) {
				_tiles[y][x]->update();
			}
		}
	}

	if (SDL_GetTicks() - tile::tileRepairTime >= tile::TIME_TO_REPAIR) {
		tile::tileRepairTime += tile::TIME_TO_REPAIR;
	}

	updateEvents();

	for (size_t i = 0; i < _drops.size(); i++) {
		_drops[i]->updateDrop();
	}

	for (int i = 0; i < NUM_DAMAGE_LABELS; i++) {
		_damageLabels[i].update();
	}


	gengine::particleEngine2D::update();


	if (_ambientLightColor.r >= 255) {
		_ambientLightColor.r = 255;
	}
	if (_ambientLightColor.g >= 255) {
		_ambientLightColor.g = 255;
	}
	if (_ambientLightColor.b >= 255) {
		_ambientLightColor.b = 255;
	}

	if (_ambientLightColor.r < 0) {
		_ambientLightColor.r = 0;
	}
	if (_ambientLightColor.g < 0) {
		_ambientLightColor.g = 0;
	}
	if (_ambientLightColor.b < 0) {
		_ambientLightColor.b = 0;
	}
}

void world::draw(gengine::spriteBatch* spriteBatch, gengine::meshBatch* meshBatch, gengine::GLSLProgram* textureProgram, gengine::camera2D* camera) {
	//***********************Draw world*********************
	textureProgram->use();

	_lightSystem.renderToRenderBuffer();

	GLint textureUniform = textureProgram->getUniformLocation("mySampler");
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(textureUniform, 0);

	//Camera matrix
	GLint pLocation = textureProgram->getUniformLocation("P");
	glm::mat4 cameraMatrix = camera->getCameraMatrix();
	glUniformMatrix4fv(pLocation, 1, GL_FALSE, &(cameraMatrix[0][0]));

	spriteBatch->begin(gengine::glyphSortType::FRONT_TO_BACK);

	_drawWorld(spriteBatch, camera);

	spriteBatch->end();
	spriteBatch->renderBatches();

	_lightSystem.renderToTileBuffer();

	spriteBatch->begin(gengine::glyphSortType::FRONT_TO_BACK);

	_drawTiles(spriteBatch, camera);

	spriteBatch->end();
	spriteBatch->renderBatches();

	textureProgram->unuse();

	//***********************Draw lights*********************
	_lightSystem.useProgram();

	_lightSystem.renderToShadowBuffer();

	//Camera matrix
	GLint plocation = _lightSystem.getUniformLocation("P");
	glUniformMatrix4fv(plocation, 1, GL_FALSE, &(cameraMatrix[0][0]));

	_drawLights(spriteBatch, meshBatch, camera);

	_lightSystem.unuseProgram();

	gengine::color ambiantColor;
	ambiantColor.r = (unsigned char)_ambientLightColor.x;
	ambiantColor.g = (unsigned char)_ambientLightColor.y;
	ambiantColor.b = (unsigned char)_ambientLightColor.z;
	ambiantColor.a = (unsigned char)_ambientLightColor.w;

	_lightSystem.addAmbiantLightToShadowMap(spriteBatch, ambiantColor, textureProgram, camera);

	_lightSystem.blurShadows(spriteBatch);

	_lightSystem.renderProduct(spriteBatch, textureProgram);

	if (_modified) {
		_modified = false;
	}
}

void world::_drawWorld(gengine::spriteBatch* spriteBatch, gengine::camera2D* camera) {
	_drawBackground(spriteBatch, camera);

	glm::vec2 cameraDimensions = camera->getDimensions() / glm::vec2(camera->getScale());

	//Center position of the parameter
	glm::vec2 cameraPosition(camera->getPosition());
	glm::ivec2 topLeftCameraCorner;
	glm::ivec2 bottomRightCameraCorner;

	topLeftCameraCorner = _getTilePosition(cameraPosition.x - (cameraDimensions.x / 1.75), cameraPosition.y + (cameraDimensions.y / 1.75));
	bottomRightCameraCorner = _getTilePosition(cameraPosition.x + (cameraDimensions.x / 1.75), cameraPosition.y - (cameraDimensions.y / 1.75));

	for (int y = bottomRightCameraCorner.y; y <= topLeftCameraCorner.y; y++) {
		for (int x = topLeftCameraCorner.x; x <= bottomRightCameraCorner.x; x++) {
			if (getTile(x, y) != nullptr) {
				_tiles[y][x]->draw(spriteBatch);
			}
		}
	}

	for (size_t i = 0; i < _entities.size(); i++) {
		_entities[i]->draw(spriteBatch);
	}

	for (auto it = _particleEmitters.begin(); it != _particleEmitters.end(); ++it) {
		it->second->update(camera);
	}

	for (int i = 0; i < _drops.size(); i++) {
		_drops[i]->draw(spriteBatch);
	}

	for (int i = 0; i < NUM_DAMAGE_LABELS; i++) {
		_damageLabels[i].draw(spriteBatch);
	}

	gengine::particleEngine2D::draw(*spriteBatch);
}

void world::_drawTiles(gengine::spriteBatch* spriteBatch, gengine::camera2D* camera) {
	glm::vec2 cameraDimensions = camera->getDimensions() / glm::vec2(camera->getScale());

	//Center position of the parameter
	glm::vec2 cameraPosition(camera->getPosition());
	glm::ivec2 topLeftCameraCorner;
	glm::ivec2 bottomRightCameraCorner;

	topLeftCameraCorner = _getTilePosition(cameraPosition.x - (cameraDimensions.x / 1.75), cameraPosition.y + (cameraDimensions.y / 1.75));
	bottomRightCameraCorner = _getTilePosition(cameraPosition.x + (cameraDimensions.x / 1.75), cameraPosition.y - (cameraDimensions.y / 1.75));


	tile* currentTile;
	for (int y = bottomRightCameraCorner.y; y <= topLeftCameraCorner.y; y++) {
		for (int x = topLeftCameraCorner.x; x <= bottomRightCameraCorner.x; x++) {
			currentTile = getTile(x, y);
			if (currentTile != nullptr) {
				currentTile->draw(spriteBatch);
			}
		}
	}
}

void world::_drawBackground(gengine::spriteBatch* spriteBatch, gengine::camera2D* camera) {
	int surfaceTileOffset = 102;
	int colorConstant = 30;

	gengine::color backgroundColor = gengine::color(_ambientLightColor.x,_ambientLightColor.y,_ambientLightColor.z,255);

	if (backgroundColor.r + colorConstant >= 255) {
		backgroundColor.r = 255;
	}
	else {
		backgroundColor.r += colorConstant;
	}

	if (backgroundColor.g + colorConstant >= 255) {
		backgroundColor.g = 255;
	}
	else {
		backgroundColor.g += colorConstant;
	}

	if (backgroundColor.b + colorConstant >= 255) {
		backgroundColor.b = 255;
	}
	else {
		backgroundColor.b += colorConstant;
	}

	{
		glm::vec2 layerScale = glm::vec2(0.75, 1);
		glm::vec2 layerSpeed = glm::vec2(0.95, 0);
		glm::vec2 centerPosition = glm::vec2((_dimensions.x * globals::pixToMet(globals::TILE_SIZE)) / 2, (_dimensions.x * globals::pixToMet(globals::TILE_SIZE)) / 2);
		glm::vec2 cameraPosition = camera->getPosition();
		glm::vec2 cameraWidth = camera->getDimensions() / camera->getScale();

		float layerInstanceWidth = cameraWidth.x * layerScale.x;

		float leftCornerPosition = cameraPosition.x - (cameraWidth.x / 2);
		float rightCornerPosition = cameraPosition.x + (cameraWidth.x / 2);

		for (float i = leftCornerPosition - (cameraPosition.x * layerSpeed.x); i <= rightCornerPosition + layerInstanceWidth - (cameraPosition.x * layerSpeed.x); i += layerInstanceWidth) {
			glm::vec4 destRect;
			destRect.x = std::floor(i / layerInstanceWidth) * layerInstanceWidth;
			destRect.x += (cameraPosition.x * layerSpeed.x);
			destRect.y = cameraPosition.y - cameraWidth.y / 2;
			destRect.y -= ((cameraPosition.y - surfaceTileOffset * globals::pixToMet(globals::TILE_SIZE))  * layerSpeed.y);
			destRect.z = cameraWidth.x * layerScale.x;
			destRect.w = cameraWidth.y;
			spriteBatch->draw(destRect, glm::vec4(0, 0, 1, 1), "assets/biomes/earth-like/backgrounds/sky.png", -4, backgroundColor);
		}
	}

	{
		glm::vec2 layerScale = glm::vec2(1.5, 0.5);
		glm::vec2 layerSpeed = glm::vec2(0.9, 0.3);
		glm::vec2 centerPosition = glm::vec2((_dimensions.x * globals::pixToMet(globals::TILE_SIZE)) / 2, (_dimensions.x * globals::pixToMet(globals::TILE_SIZE)) / 2);
		glm::vec2 cameraPosition = camera->getPosition();
		glm::vec2 cameraWidth = camera->getDimensions() / camera->getScale();

		float layerInstanceWidth = cameraWidth.x * layerScale.x;

		float leftCornerPosition = cameraPosition.x - (cameraWidth.x / 2);
		float rightCornerPosition = cameraPosition.x + (cameraWidth.x / 2);

		for (float i = leftCornerPosition - (cameraPosition.x * layerSpeed.x); i <= rightCornerPosition + layerInstanceWidth - (cameraPosition.x * layerSpeed.x); i += layerInstanceWidth) {
			glm::vec4 destRect;
			destRect.x = std::floor(i / layerInstanceWidth) * layerInstanceWidth;
			destRect.x += (cameraPosition.x * layerSpeed.x);
			destRect.y = cameraPosition.y - cameraWidth.y / 2;
			destRect.y -= ((cameraPosition.y - surfaceTileOffset * globals::pixToMet(globals::TILE_SIZE))  * layerSpeed.y);
			destRect.z = cameraWidth.x * layerScale.x;
			destRect.w = cameraWidth.y;
			spriteBatch->draw(destRect, glm::vec4(0, 0, 1, 1), "assets/biomes/earth-like/backgrounds/mountains.png", -3, backgroundColor);
		}
	}

	{
		glm::vec2 layerScale = glm::vec2(1.5, 0.5);
		glm::vec2 layerSpeed = glm::vec2(0.75, 0.4);
		glm::vec2 centerPosition = glm::vec2((_dimensions.x * globals::pixToMet(globals::TILE_SIZE)) / 2, (_dimensions.x * globals::pixToMet(globals::TILE_SIZE)) / 2);
		glm::vec2 cameraPosition = camera->getPosition();
		glm::vec2 cameraWidth = camera->getDimensions() / camera->getScale();

		float layerInstanceWidth = cameraWidth.x * layerScale.x;

		float leftCornerPosition = cameraPosition.x - (cameraWidth.x / 2);
		float rightCornerPosition = cameraPosition.x + (cameraWidth.x / 2);

		for (float i = leftCornerPosition - (cameraPosition.x * layerSpeed.x); i <= rightCornerPosition + layerInstanceWidth - (cameraPosition.x * layerSpeed.x); i += layerInstanceWidth) {
			glm::vec4 destRect;
			destRect.x = std::floor(i / layerInstanceWidth) * layerInstanceWidth;
			destRect.x += (cameraPosition.x * layerSpeed.x);
			destRect.y = cameraPosition.y - cameraWidth.y / 2;
			destRect.y -= ((cameraPosition.y - surfaceTileOffset * globals::pixToMet(globals::TILE_SIZE))  * layerSpeed.y);
			destRect.z = cameraWidth.x * layerScale.x;
			destRect.w = cameraWidth.y;
			spriteBatch->draw(destRect, glm::vec4(0, 0, 1, 1), "assets/biomes/earth-like/backgrounds/trees.png", -2, backgroundColor);
		}
	}
	
	{
		glm::vec2 layerScale = glm::vec2(1.5, 0.5);
		glm::vec2 layerSpeed = glm::vec2(0.65, 0.5);
		glm::vec2 centerPosition = glm::vec2((_dimensions.x * globals::pixToMet(globals::TILE_SIZE)) / 2, (_dimensions.x * globals::pixToMet(globals::TILE_SIZE)) / 2);
		glm::vec2 cameraPosition = camera->getPosition();
		glm::vec2 cameraWidth = camera->getDimensions() / camera->getScale();

		float layerInstanceWidth = cameraWidth.x * layerScale.x;

		float leftCornerPosition = cameraPosition.x - (cameraWidth.x / 2);
		float rightCornerPosition = cameraPosition.x + (cameraWidth.x / 2);

		for (float i = leftCornerPosition - (cameraPosition.x * layerSpeed.x); i <= rightCornerPosition + layerInstanceWidth - (cameraPosition.x * layerSpeed.x); i += layerInstanceWidth) {
			glm::vec4 destRect;
			destRect.x = std::floor(i / layerInstanceWidth) * layerInstanceWidth;
			destRect.x += (cameraPosition.x * layerSpeed.x);
			destRect.y = cameraPosition.y - cameraWidth.y / 2;
			destRect.y -= ((cameraPosition.y - surfaceTileOffset * globals::pixToMet(globals::TILE_SIZE))  * layerSpeed.y);
			destRect.z = cameraWidth.x * layerScale.x;
			destRect.w = cameraWidth.y;
			spriteBatch->draw(destRect, glm::vec4(0, 0, 1, 1), "assets/biomes/earth-like/backgrounds/lake.png", -1, backgroundColor);
		}
	}

	{
		glm::vec2 layerScale = glm::vec2(1.5, 0.5);
		glm::vec2 layerSpeed = glm::vec2(0.5, 0.65);
		glm::vec2 centerPosition = glm::vec2((_dimensions.x * globals::pixToMet(globals::TILE_SIZE)) / 2, (_dimensions.x * globals::pixToMet(globals::TILE_SIZE)) / 2);
		glm::vec2 cameraPosition = camera->getPosition();
		glm::vec2 cameraWidth = camera->getDimensions() / camera->getScale();

		float layerInstanceWidth = cameraWidth.x * layerScale.x;

		float leftCornerPosition = cameraPosition.x - (cameraWidth.x / 2);
		float rightCornerPosition = cameraPosition.x + (cameraWidth.x / 2);

		for (float i = leftCornerPosition - (cameraPosition.x * layerSpeed.x); i <= rightCornerPosition + layerInstanceWidth - (cameraPosition.x * layerSpeed.x); i += layerInstanceWidth) {
			glm::vec4 destRect;
			destRect.x = std::floor(i / layerInstanceWidth) * layerInstanceWidth;
			destRect.x += (cameraPosition.x * layerSpeed.x);
			destRect.y = cameraPosition.y - cameraWidth.y / 2;
			destRect.y -= ((cameraPosition.y - surfaceTileOffset * globals::pixToMet(globals::TILE_SIZE))  * layerSpeed.y);
			destRect.z = cameraWidth.x * layerScale.x;
			destRect.w = cameraWidth.y;
			spriteBatch->draw(destRect, glm::vec4(0, 0, 1, 1), "assets/biomes/earth-like/backgrounds/front trees.png", -1, backgroundColor);
		}
	}
}


int world::addLight(glm::vec2* position, light lightToCopy) {
	light newLight = lightToCopy;
	newLight.setPositionSetter(position);

	mt19937_64 generator(time(0));
	uniform_int_distribution<int> idRange{ 0, MAX_LIGHT_ID_NUMBER };

	int ID = idRange(generator);
	bool ok = false;

	while (ok == false) {
		if (_lights.count(ID) == 0) {
			_lights.emplace(std::make_pair(ID, newLight));
			_lightOrder.push_back(&_lights[ID]);
			ok = true;
		}
		else {
			ID = idRange(generator);
		}
	}
	return ID;
}
int world::addLightAndOrder(glm::vec2* position, light lightToCopy) {
	light newLight = lightToCopy;
	newLight.setPositionSetter(position);

	mt19937_64 generator(time(0));
	uniform_int_distribution<int> idRange{ 0, MAX_LIGHT_ID_NUMBER };

	int ID = idRange(generator);
	bool ok = false;

	while (ok == false) {
		if (_lights.count(ID) == 0) {
			_lights.emplace(std::make_pair(ID, newLight));
			_lightOrder.push_back(&_lights[ID]);
			ok = true;
		}
		else {
			ID = idRange(generator);
		}
	}
	_lights[ID].calculateTypeID();
	//If they have the same intensity, the color sets the priority
	_sortLights();

	//Adds the indexes at which the lights aren't the same as the next light
	_lightBatchesOffsets.clear();
	for (int i = 0; i < _lightOrder.size() - 1; i++) {
		if (!_lightOrder[i]->isTheSame(*_lightOrder[i + 1])) {
			_lightBatchesOffsets.push_back(i);
		}
	}
	_lightBatchesOffsets.push_back(_lightOrder.size() - 1);
	return ID;
}

void world::orderLights() {
	_lightOrder.clear();
	for (auto it = _lights.begin(); it != _lights.end(); ++it) {
		_lightOrder.push_back(&it->second);
	}

	_sortLights();

	//Adds the indexes at which the lights aren't the same as the next light
	_lightBatchesOffsets.clear();
	if (!_lightOrder.empty()) {
		for (int i = 0; i < _lightOrder.size() - 1; i++) {
			if (!_lightOrder[i]->isTheSame(*_lightOrder[i + 1])) {
				_lightBatchesOffsets.push_back(i);
			}
		}
		_lightBatchesOffsets.push_back(_lightOrder.size() - 1);
	}
}

light* world::getLight(int lightID) {
	return &_lights[lightID];
}

void world::deleteLight(int lightID) {
	_lights.erase(lightID);
}
void world::deleteLightAndOrder(int lightID) {
	_lights.erase(lightID);

	_lightOrder.clear();
	for (auto it = _lights.begin(); it != _lights.end(); ++it) {
		_lightOrder.push_back(&it->second);
	}

	_sortLights();

	//Adds the indexes at which the lights aren't the same as the next light
	_lightBatchesOffsets.clear();
	if (!_lightOrder.empty()) {
		for (int i = 0; i < _lightOrder.size() - 1; i++) {
			if (!_lightOrder[i]->isTheSame(*_lightOrder[i + 1])) {
				_lightBatchesOffsets.push_back(i);
			}
		}
		_lightBatchesOffsets.push_back(_lightOrder.size() - 1);
	}
}

void world::_drawLights(gengine::spriteBatch* spriteBatch, gengine::meshBatch* meshBatch, gengine::camera2D* camera) {
	GLint intensityLoc = _lightSystem.getUniformLocation("intensity");
	GLint coneAngleLoc = _lightSystem.getUniformLocation("coneAngle");
	GLint lightDirectionLoc = _lightSystem.getUniformLocation("lightDirection");

	meshBatch->begin();

	int indexesReached = 0;
	for (int i = 0; i < _lightOrder.size(); i++) {
		_lightOrder[i]->draw(spriteBatch, meshBatch, camera, intensityLoc, coneAngleLoc, lightDirectionLoc, _Box2DWorld, _modified);

		if (i == _lightBatchesOffsets[indexesReached]) {
			indexesReached++;

			glUniform1f(intensityLoc, _lightOrder[i]->intensity / 5);
			glUniform1f(coneAngleLoc, _lightOrder[i]->coneAngle);
			glUniform2f(lightDirectionLoc, _lightOrder[i]->lightDirection.x, _lightOrder[i]->lightDirection.y);

			meshBatch->end();

			if (i != _lightBatchesOffsets.back()) {
				meshBatch->begin();
			}
		}
	}
}

int world::addParticleEmitter(particleEmitter* newEmitter) {
	mt19937_64 generator(time(0));
	uniform_int_distribution<int> idRange{ 0, MAX_PARTICLE_EMITTER_ID_NUMBER };

	int ID = idRange(generator);
	bool ok = false;

	while (ok == false) {
		if (_particleEmitters.count(ID) == 0) {
			_particleEmitters.emplace(std::make_pair(ID, newEmitter));
			ok = true;
		}
		else {
			ID = idRange(generator);
		}
	}

	return ID;
}

void world::deleteParticleEmitter(int ID) {
	_particleEmitters.erase(ID);
}

particleEmitter* world::getEmitter(int ID) {
	return _particleEmitters[ID];
}

bool world::_sortBackToFront(light* a, light* b) {
	gengine::color colorA = a->getColor();
	gengine::color colorB = b->getColor();
	if (a->getIntensity() != b->getIntensity()) {
		return (a->getIntensity() < b->getIntensity());
	}
	else {
		if (colorA.r != colorB.r) {
			return (colorA.r < colorB.r);
		}
		else if (colorA.g != colorB.g) {
			return (colorA.g < colorB.g);
		}
		else if (colorA.b != colorB.b) {
			return (colorA.b < colorB.b);
		}
		else if (colorA.a != colorB.a) {
			return (colorA.a < colorB.a);
		}
	}
}

void world::_sortLights() {
	std::stable_sort(_lightOrder.begin(), _lightOrder.end(), _sortBackToFront);
}

void world::debugDraw(gengine::debugRenderer* debugRenderer, gengine::camera2D* camera) {
	for (size_t i = 0; i < _tileGroups.size(); i++) {
		_tileGroups[i]->debugDraw(debugRenderer);
	}

	for (auto it : _entities) {
		it->debugDraw(debugRenderer);
	}
}

int luaHandler::LUA_WORLD_addToAmbientLightColor(lua_State *S) {
	_world->addToAmbientLightColor(glm::vec4(lua_tonumber(S, 1), lua_tonumber(S, 2), lua_tonumber(S, 3),255));

	return 0;
}

int luaHandler::LUA_WORLD_getAmbientLightColor(lua_State *S) {
	glm::vec4 ambientLightColor = _world->getAmbientLightColor();

	lua_newtable(S);
	lua_pushnumber(S, 1);
	lua_pushnumber(S, ambientLightColor.x);
	lua_settable(S, -3);
	lua_pushnumber(S, 2);
	lua_pushnumber(S, ambientLightColor.y);
	lua_settable(S, -3);
	lua_pushnumber(S, 3);
	lua_pushnumber(S, ambientLightColor.z);
	lua_settable(S, -3);

	return 1;
}

int luaHandler::LUA_WORLD_damageFrontTile(lua_State *S) {
	int x = lua_tointeger(S, 1);
	int y = lua_tointeger(S, 2);

	tile* takenTile = _world->getTile(x, y);

	if (takenTile != nullptr) {
		takenTile->damage(lua_tointeger(S, 3), true);
	}

	return 0;
}

int luaHandler::LUA_WORLD_damageBackTile(lua_State *S) {
	int x = lua_tointeger(S, 1);
	int y = lua_tointeger(S, 2);

	tile* takenTile = _world->getTile(x, y);

	if (takenTile != nullptr) {
		takenTile->damage(lua_tointeger(S, 3), false);
	}

	return 0;
}

int luaHandler::LUA_WORLD_getTile(lua_State *S) {
	int x = lua_tointeger(S, 1);
	int y = lua_tointeger(S, 2);

	tile* takenTile = _world->getTile(x, y);

	if (takenTile != nullptr) {
		tile** addedLightUserdata = (tile**)lua_newuserdata(S, sizeof(tile*));
		*addedLightUserdata = takenTile;
		luaL_getmetatable(S, "tile_m");
		lua_setmetatable(S, -2);
	}
	else {
		lua_pushnil(S);
		luaL_getmetatable(S, "tile_m");
		lua_setmetatable(S, -2);

	}

	return 1;
}

int luaHandler::LUA_WORLD_raycast(lua_State *S) {
	float x = globals::pixToMet(lua_tonumber(S, 1));
	float y = globals::pixToMet(lua_tonumber(S, 2));
	float angle = 360 - lua_tonumber(S, 3);
	angle *= (2 * M_PI) / 360;
	float length = globals::pixToMet(lua_tonumber(S, 4));

	b2Vec2 endPointVec;
	endPointVec.x = cos(angle) * length;
	endPointVec.y = sin(angle) * length;

	worldRayCastCallback rayCastCallback;
	rayCastCallback.pointR = b2Vec2(x - endPointVec.x, y - endPointVec.y);
	_world->getB2World()->RayCast(&rayCastCallback, b2Vec2(x, y), b2Vec2(x - endPointVec.x, y - endPointVec.y));

	lua_newtable(S);
	lua_pushnumber(S, 1);
	lua_pushnumber(S, globals::metToPix(rayCastCallback.pointR.x));
	lua_settable(S, -3);
	lua_pushnumber(S, 2);
	lua_pushnumber(S, globals::metToPix(rayCastCallback.pointR.y));
	lua_settable(S, -3);

	return 1;
}

int luaHandler::LUA_WORLD_createProjectile(lua_State *S) {
	std::string nameID = lua_tostring(S, 1);

	float x = globals::pixToMet(lua_tonumber(S, 2));
	float y = globals::pixToMet(lua_tonumber(S, 3));

	projectile* newProjectile = new projectile;
	newProjectile->create(nameID, glm::vec2(x, y), glm::vec2(lua_tonumber(S, 4), lua_tonumber(S, 5)), nullptr);

	return 0;
}

int luaHandler::LUA_WORLD_createEffect(lua_State *S) {
	std::string nameID = lua_tostring(S, 1);

	float x = globals::pixToMet(lua_tonumber(S, 2));
	float y = globals::pixToMet(lua_tonumber(S, 3));
	float angle;

	if (lua_isnoneornil(S, 4)) {
		angle = 0;
	}
	else {
		angle = lua_tonumber(S, 4);
	}

	effect* newEffect = new effect(nameID, glm::vec2(x,y), angle);

	return 0;
}

int luaHandler::LUA_WORLD_getCameraPosition(lua_State *S) {
	lua_newtable(S);
	lua_pushnumber(S, 1);
	lua_pushnumber(S, globals::metToPix(_world->getActivateCamera()->getPosition().x));
	lua_settable(S, -3);
	lua_pushnumber(S, 2);
	lua_pushnumber(S, globals::metToPix(_world->getActivateCamera()->getPosition().y));
	lua_settable(S, -3);

	return 1;
}

int luaHandler::LUA_WORLD_getCameraDimensions(lua_State *S) {
	lua_newtable(S);
	lua_pushnumber(S, 1);
	lua_pushnumber(S, _world->getActivateCamera()->getDimensions().x);
	lua_settable(S, -3);
	lua_pushnumber(S, 2);
	lua_pushnumber(S, _world->getActivateCamera()->getDimensions().y);
	lua_settable(S, -3);

	return 1;
}


void luaHandler::_registerWorldFunction() {
	luaL_Reg worldlib_m[] = {
		{ "getTile", LUA_WORLD_getTile },
		{ "raycast", LUA_WORLD_raycast },
		{ "addToAmbientLightColor", LUA_WORLD_addToAmbientLightColor },
		{ "getAmbientLightColor", LUA_WORLD_getAmbientLightColor },
		{ "damageFrontTile", LUA_WORLD_damageFrontTile },
		{ "damageBackTile", LUA_WORLD_damageBackTile },
		{ "createProjectile", LUA_WORLD_createProjectile },
		{ "createEffect", LUA_WORLD_createEffect },
		{ "getCameraPosition", LUA_WORLD_getCameraPosition },
		{ "getCameraDimensions", LUA_WORLD_getCameraDimensions },
		{ NULL, NULL }
	};

	lua_newtable(_S);
	luaL_setfuncs(_S, worldlib_m, 0);
	lua_setglobal(_S, "world");

	lua_getglobal(_S, "world"); // world
	lua_pushnumber(_S, globals::TILE_SIZE);
	lua_setfield(_S, -2, "tileSize");
}