#pragma once
#include <glm/glm.hpp>
#include <Box2D/Box2D.h>
#include <Gengine/spriteSheet.h>
#include <Gengine/spriteBatch.h>
#include <Gengine/vertex.h>

#include "entity.h"
#include "inventory.h"
#include "userData.h"
#include "heldItem.h"
#include "skeleton.h"

enum groundState {
	ground,
	falling,
	quickFalling,
	rising,
	jumping,
	landing,
	quickLanding
};

class world;

class agent: public entity{
public:
	friend class luaHandler;

	virtual void draw(gengine::spriteBatch* spriteBatch);
	void destroy(world* world);

	virtual void debugDraw(gengine::debugRenderer* debugRenderer) { return; }

	virtual void addDamageLabel(glm::vec2 position, int damage, gengine::color labelColor = gengine::color(255, 0, 0, 255)) { return; }

	virtual void update();
	virtual void updateGroundState();
	virtual void updateAnimations();
	virtual void updatePlatformFiltering();

	virtual void damage(int damage, int invulnerabilityTime, gengine::color labelColor = gengine::color(255,0,0,255));
	virtual void knockBack(float angle, float force);

	void setSpeedMultiplier(float multiplier);

	virtual void playAnimation(std::string defaultFlipAnimation, std::string flippedAnimation, int time, int trackIndex, bool loop) { _skeleton.setAnimation(defaultFlipAnimation, flippedAnimation, time, loop,trackIndex, true); }

	virtual void startContact(userData* collidedWith, userData* thisFixture);
	virtual void endContact(userData* collidedWith, userData* thisFixture);

	virtual void equipItem(int itemIndex);

	inventory* getInventory();
	skeleton* getSkeleton() { return &_skeleton; }

protected:
	skeleton _skeleton;

	float _xAcceleration;
	float _maxXSpeed;
	float _minJumpSpeed;
	float _maxJumpSpeed;
	float _jumpForce;
	float _speedMultiplier = 1;

	float _currentFallDamage;

	void _moveRight();
	void _moveLeft();
	void _slow();
	void _jump();
	void _releaseJump();

	inventory* _inventory = nullptr;

	heldItem* _rightHeldItem = nullptr;
	heldItem* _leftHeldItem = nullptr;
	int _rightEquippedIndex = 0;
	int _leftEquippedIndex = 1;

	int _sinceLastRightUse;
	int _sinceLastLeftUse;

	bool _aimingRight;
	bool _aimingLeft;

	groundState _groundState;

	bool _walkingRight;
	bool _facingRight;

	bool _isWalking;
	bool _wasWalking;

	world* _world;

	std::map<std::string, void*> LUA_cppvars;
	std::vector<std::string> _activeBehaviours;

	int _health = 100;
	int _invulnerabilityTimer = 0;
};