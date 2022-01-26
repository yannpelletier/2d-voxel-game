#pragma once
#include <Gengine/spriteBatch.h>
#include <Gengine/spriteSheet.h>
#include "spine/spine.h"
#include <map>

class bone;
class sAnimation;

class eventRegister {
public:
	void setTrue(std::string eventName);
	bool getEventValue(std::string eventName);

	void resetEvents();
private:
	std::map<std::string, bool> _eventMap;
};

class skeleton
{
public:
	skeleton();
	~skeleton();

	void init(spSkeletonData* skeletonData, int drawLayer, glm::vec2 offset);
	void newBone(std::string name, gengine::spriteSheet image);
	void newBone(std::string name);

	void setAnimation(std::string name, bool loop, int trackIndex, bool overwrite);
	void setAnimation(std::string defaultFlipAnimation, std::string flippedAnimation, bool loop, int trackIndex, bool overwrite);
	void setAnimation(std::string name, int time, bool loop, int trackIndex, bool overwrite);
	void setAnimation(std::string defaultFlipAnimation, std::string flippedAnimation, int time, bool loop, int trackIndex, bool overwrite);
	void setDirection(bool flipX);

	void setAnimationSpeedMultiplier(float multiplier, int trackIndex);

	bool getEvent(std::string name);

	void setMix(std::string from, std::string to, float time);

	std::pair<std::string,std::string> getAnimationNames(int trackIndex);
	spSkeleton* getSkeleton();
	glm::vec3 getBonePosInfo(std::string boneName);

	void draw(gengine::spriteBatch* spriteBatch);

	void update(glm::vec2 position, int deltaTime);

	void updateBegin(glm::vec2 position, int deltaTime); //manipulate bones between the updates
	void updateEnd();

	void applyMainAnimation();

	void forceAdditiveRotate(std::string boneName, float angleDeg, float angleDifferenceWhenFlipped);
	void forceAbsoluteRotate(std::string boneName, float angleDeg);

	int getDrawLayer() { return _drawLayer; }

	static std::map<spAnimationState*, eventRegister*> eventMaps;

private:
	void _updateAnimations();

	bool _validateTrackIndex(int trackIndex);

	spSkeleton* _skeleton;
	std::map<std::string, bone> _bones;
	std::map<std::string, bool> _events;
	
	spAnimationState* _mainAnimation;
	std::vector<std::pair<std::string, std::string>> _mixedAnimationsDirectionPairs;

	glm::vec2 _offset;

	int _drawLayer;

	bool _lastFlipX = 0;
};

class bone {

public:
	void init(spSlot* boneSlot, gengine::spriteSheet& spriteSheet);

	void draw(gengine::spriteBatch* spriteBatch, int layer);

	spSlot* _boneSlot;
	gengine::spriteSheet _image;	
};