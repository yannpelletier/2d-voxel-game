#pragma once
#include <Gengine/ressourceManager.h>
#include <Gengine/debugRenderer.h>
#include <Gengine/camera2D.h>
#include <Gengine/particleEngine2D.h>

#include "particleEmitter.h"
#include "definition.h"
#include "material.h"
#include "contactListener.h"
#include "tileBatch.h"
#include "tile.h"
#include "globals.h"
#include "entity.h"
#include "event.h"
#include "bodyTemplate.h"
#include "lightSystem.h"
#include "worldGenerator.h"
#include "worldTemplate.h"
#include "effect.h"
#include "damageLabel.h"
#include "childTile.h"
#include "regularTile.h"

#include <vector>
#include <algorithm>

class worldRayCastCallback : public b2RayCastCallback {
public:
	b2Vec2 pointR;
	float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction) {
		if (fixture->GetUserData() != nullptr) {
			if (static_cast<userData*>(fixture->GetUserData())->type == gameObjectType::TILE || static_cast<userData*>(fixture->GetUserData())->type == gameObjectType::TILESLOPE) {
				pointR = point;
				return fraction;
			}
			else {
				return -1;
			}
		}
		return 1;
	}
};

class world
{
public:
	friend class regularTile;
	friend class fatherTile;
	friend class childTile;

	world(float gravity, gengine::camera2D* camera);
	~world();
	
	//Generates a portion of the world from a certain biome model(worldGenerator)
	void generate(worldGenerator* worldGenerator, int fromX, int toX, int seed);
	//Sets the acceleration towards the bottom of the world for every objects in it
	void setGravity(float gravity);

	//Returns the active camera
	gengine::camera2D* getActivateCamera() { return _camera; }
	//Returns the box2d world of the world
	b2World* getB2World() { return _Box2DWorld; }

	//Updates everything contained within the world
	void update(gengine::camera2D* camera);
	//Process the physics for the box2d world
	void step(float dt, int velocityIterations, int positionIterations);
	//Draws everything, including lights
	void draw(gengine::spriteBatch* spriteBatch, gengine::meshBatch* meshBatch, gengine::GLSLProgram* textureProgram, gengine::camera2D* camera);
	//Draws the hitboxes of different components in the world
	void debugDraw(gengine::debugRenderer* debugRenderer, gengine::camera2D* camera);

private:
	//Draws everything without light
	void _drawWorld(gengine::spriteBatch* spriteBatch, gengine::camera2D* camera);
	//Draws only the front and back tiles 
	void _drawTiles(gengine::spriteBatch* spriteBatch, gengine::camera2D* camera);
	//Draw the different layers of background along with the parallax
	void _drawBackground(gengine::spriteBatch* spriteBatch, gengine::camera2D* camera);
	//Draws lights
	void _drawLights(gengine::spriteBatch* spriteBatch, gengine::meshBatch* meshBatch, gengine::camera2D* camera);

public:
	//Adds a light without reordering the light vector, returns the ID of the new added light
	int addLight(glm::vec2* position, light lightToCopy); 
	//Adds a light and reorders the light vector, returns the ID of the new added light
	int addLightAndOrder(glm::vec2* position, light lightToCopy);
	//Orders the light vector to give a constant to the order in which the lights are drawn depending on their color
	void orderLights();
	//Returns the light associated with the lightID given, returns -1 if the light doesn't exist
	light* getLight(int lightID);
	//Deletes the light associated with the lightID given without ordering the light vector
	void deleteLight(int lightID);
	//Deletes the light associated with the lightID given and orders the light vector
	void deleteLightAndOrder(int lightID);
	//Adds a label that allows to show how much damage was done to an entity with an attack
	void addDamageLabel(glm::vec2 position, int value, gengine::color color);
	//Adds a label that allows to show how much damage was done to an entity with an attack, or what buff or debuff it gave the entity
	void addDamageLabel(glm::vec2 position, std::string value, gengine::color color);
	//Puts an entity in the queue of added entities so that it can be added after the box2d loop
	void addEntity(entity* newEntity);
	//Puts an entity in the queue of deleted entities so that it can be deleted after the box2d loop
	void deleteEntity(entity* newEntity);
	//Puts a body in the queue of added bodies so that it can be added after the box2d loop
	void createBody(b2BodyDef bodyDef, b2FixtureDef fixture, b2Body** bodyPointer, std::vector<b2Fixture*>* fixturesPointers, b2Vec2 beginImpulse = b2Vec2(0, 0));
	//Puts a body in the queue of added bodies so that it can be added after the box2d loop
	void createBody(b2BodyDef bodyDef, std::vector<b2FixtureDef> fixtures, b2Body** bodyPointer, std::vector<b2Fixture*>* fixturesPointers, b2Vec2 beginImpulse = b2Vec2(0, 0));
	//Puts a body in the queue of deleted bodies so that it can be deleted after the box2d loop
	void deleteBody(b2Body* toDelete);
	//Adds a dropped pickable inventory item to the world 
	void addDrop(inventoryItem* toAdd);
	//Removes a dropped pickable inventory item from the world 
	void deleteDrop(inventoryItem* toDelete);
	//Adds an event defined in lua in the world
	void addEvent(std::string nameID, int duration);
	//Adds an event defined in lua in the world
	void addEvent(std::string nameID);
	//Removes an event defined in lua in the world
	void deleteEvent(std::string nameID);
	//Update every events active in the world
	void updateEvents();
	//Adds something that constantly emits particles and returns the ID of the added particle emitter
	int addParticleEmitter(particleEmitter* newEmitter);
	//Removes a particle emitter
	void deleteParticleEmitter(int ID);
	//Returns the particle emitter associated with the ID 
	particleEmitter* getEmitter(int ID);
private:
	//Checks in the queue of added entities if there are entities to add after the box2d loop
	void _checkForAddedEntities();
	//Checks in the queue of deleted entities if there are entities to delete after the box2d loop
	void _checkForDeletedEntities();
	//Checks in the queue of added bodies if there are bodies to add after the box2d loop
	void _checkForAddedBodies();
	//Checks in the queue of deleted bodies if there are bodies to delete after the box2d loop
	void _checkForDeletedBodies();
	//Reorders the _lightOrder vector
	void _sortLights();
	//Function used with std::sort to determine the order in which lights are drawn
	static bool _sortBackToFront(light* a, light* b);
	//Modified modulo operator for certain operations (a % b)
	int modulo(int a, unsigned int b);

public:
	bool addTileIngame(definition* definition, bool front, int gridX, int gridY);
	void deleteTileIngame(bool front, int gridX, int gridY);
	void addTileModIngame(materialMod* definition,bool front,int gridX, int gridY);

	
	worldTemplate getStructure(std::string filePath);
	void expandWorldToTile(unsigned int x, unsigned int y);
	void addTilesFromTemplate(worldTemplate& worldTemplate, int gridX, int gridY);

	regularTile* replaceForRegularTile(int gridX, int gridY);
	fatherTile* replaceForFatherTile(int gridX, int gridY);
	childTile* replaceForChildTile(int gridX, int gridY);
	void replaceForEmptyTile(int gridX, int gridY);

	tile* getTile(int gridX, int gridY);
	tile* getTile(glm::vec2 point); //Gets the tile at a certain point

	glm::vec2 getWorldCoordsDimensions();

	void setChanged();
	bool hasChanged();

	void addToAmbientLightColor(glm::vec4& color);
	glm::vec4 getAmbientLightColor();

private:
	//Adds a simple tile of the specified material to the world at the specified coordinates while the game is playing 
	bool _addRegularTileIngame(material* definition, bool front, int gridX, int gridY);
	//Adds a placable item of the specified item to the world at the specified coordinates while the game is playing 
	bool _addPlacableItemIngame(placableItem* definition, int gridX, int gridY);

	bool _regularTileCanBePlaced(bool front, int gridX, int gridY);
	void _setAutoTileCountAround(bool front, int gridX, int gridY);

	void _addTileGen(definition* definition, bool front, int gridX, int gridY);
	//Adds a simple tile of the specified material to the world at the specified coordinates while the world is generating
	void _addRegularTileGen(material* definition, bool front, int gridX, int gridY);
	//Adds a placable item of the specified item to the world at the specified coordinates while the world is generating
	void _addPlacableItemGen(placableItem* definition, int gridX, int gridY);
	void _addTileModGen(materialMod* definition, bool front, int gridX, int gridY);
	void _deleteTileGen(bool front, int gridX, int gridY);
	void _handlePostGen();
	bool _isSupported(int gridX, int gridY);

	int _getAutoTileCount(bool front, int gridX, int gridY);

	void _checkSupportAround(int gridX, int gridY);

	bool _insideMap(int gridX, int gridY);

	void _deleteBatch(tileHitbox* batchToDelete);

	//Check if a tile meets all the conditions to be meshed with other tiles
	bool _meshable(int gridX, int gridY);
	//Returns true if the tile has nothing as its front material
	bool _frontEmpty(int gridX, int gridY);

	//Get the tiles included in the same square as the one given
	void _getMaxBatchSquare(int gridX, int gridY, glm::ivec2 &topLeft, glm::ivec2 &bottomRight);
	glm::ivec2 _getTilePosition(float x, float y);

	//Generates the physics meshes for the entire world
	void _generateMeshes();
	//Generates the physics meshes only in a certain area
	void _generateMeshes(glm::ivec2 bottomLeft, glm::ivec2 topRight);
	void _generateMeshesOfRegion(int gridX, int gridY);

private:
	glm::ivec2 _dimensions;
	bool _modified;
	float _gravity;

	gengine::camera2D* _camera;
	b2World* _Box2DWorld;
	lightSystem _lightSystem;
	contactListener* _contactListener;
	
	glm::vec4 _ambientLightColor = glm::vec4(255, 255, 255, 255);

	std::vector<entity*> _entities;
	std::vector<std::vector<tile*>> _tiles;
	std::vector<tileHitbox*> _tileGroups;
	std::vector<inventoryItem*> _drops;
	std::map<int, particleEmitter*> _particleEmitters;
	std::map<int, light> _lights;
	std::vector<light*> _lightOrder;
	std::vector<int> _lightBatchesOffsets;
	std::vector<event*> _events;
	damageLabel* _damageLabels;

	glm::ivec2 _lastBottomLeftCameraCorner;
	glm::ivec2 _lastTopRightCameraCorner;
	glm::ivec2 _bottomLeftCameraCorner;
	glm::ivec2 _topRightCameraCorner;

	std::vector<bodyTemplate> _toCreate;
	std::vector<b2Body*> _bodiesToDestroy;
	std::vector<entity*> _entitiesToAdd;
	std::vector<entity*> _entitiesToDestroy;

	const int NUM_DAMAGE_LABELS = 20;
	const int MAX_LIGHT_ID_NUMBER = 100000;
	const int MAX_PARTICLE_EMITTER_ID_NUMBER = 100000;
};

