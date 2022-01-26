#include "skeleton.h"
#include "spine/spine.h"
#include "spine/extension.h"
#include <GLM/gtx/rotate_vector.hpp>

std::map<spAnimationState*, eventRegister*> skeleton::eventMaps;


skeleton::skeleton()
{
}


skeleton::~skeleton()
{
	skeleton::eventMaps.erase(_mainAnimation);
}

void eventListener(spAnimationState* state, int trackIndex, spEventType type, spEvent* event, int loopCount) {
	if (type == SP_ANIMATION_EVENT) {
		skeleton::eventMaps[state]->setTrue(event->data->name);
	}
	else if (type == SP_ANIMATION_COMPLETE) {
		skeleton::eventMaps[state]->setTrue("SP_ANIMATION_COMPLETE");
	}
	else if (type == SP_ANIMATION_END) {
		skeleton::eventMaps[state]->setTrue("SP_ANIMATION_END");
	}
	else if (type == SP_ANIMATION_START) {
		skeleton::eventMaps[state]->setTrue("SP_ANIMATION_START");
	}
}

void eventRegister::setTrue(std::string eventName) {
	auto it = _eventMap.find(eventName);
	if (it != _eventMap.end()) {
		it->second = true;
	}
	else {
		_eventMap.emplace(std::pair<std::string, bool>(eventName, true));
	}
}
bool eventRegister::getEventValue(std::string eventName){
	auto it = _eventMap.find(eventName);
	if (it != _eventMap.end()) {
		return it->second;
	}
	else {
		return false;
	}
}

void eventRegister::resetEvents() {
	for (auto& it : _eventMap) {
		it.second = false;
	}
}

void skeleton::init(spSkeletonData* skeletonData, int drawLayer, glm::vec2 offset) {
	_skeleton = spSkeleton_create(skeletonData);
	spAnimationStateData* stateData = spAnimationStateData_create(skeletonData);
	_mainAnimation = spAnimationState_create(stateData);

	skeleton::eventMaps.emplace(std::pair <spAnimationState*, eventRegister* >(_mainAnimation, new eventRegister));

	_drawLayer = drawLayer;

	_mainAnimation->listener = &eventListener;

	_offset = offset;
}

void skeleton::setDirection(bool flipX) {
	_skeleton->flipX = flipX;
}

void skeleton::setAnimationSpeedMultiplier(float multiplier, int trackIndex) {
	if (_validateTrackIndex(trackIndex)) {
		_mainAnimation->tracks[trackIndex]->timeScale = multiplier;
	}
}

void skeleton::setAnimation(std::string name, bool loop, int trackIndex, bool overwrite) {
	while (trackIndex > (int)_mixedAnimationsDirectionPairs.size() - 1) {
		std::pair<std::string, std::string> newPair;
		newPair.first == "";
		newPair.second == "";
		_mixedAnimationsDirectionPairs.push_back(newPair);
	}

	if (_mixedAnimationsDirectionPairs[trackIndex].first == name && _mixedAnimationsDirectionPairs[trackIndex].second == name && !overwrite) {
		return;
	}

	spAnimation* toSet = spSkeletonData_findAnimation(_skeleton->data, name.c_str());
	if (toSet != NULL) {
		spAnimationState_setAnimation(_mainAnimation, trackIndex, toSet, loop);

		_mixedAnimationsDirectionPairs[trackIndex].first = name;
		_mixedAnimationsDirectionPairs[trackIndex].second = name;
	}
	else {
		std::cout << name << "is not a valid name for an animation." << endl;
	}
}

void skeleton::setAnimation(std::string defaultFlipAnimation, std::string flippedAnimation, bool loop, int trackIndex, bool overwrite) {
	while (trackIndex > (int)_mixedAnimationsDirectionPairs.size() - 1) {
		std::pair<std::string, std::string> newPair;
		newPair.first == "";
		newPair.second == "";
		_mixedAnimationsDirectionPairs.push_back(newPair);
	}

	if (_mixedAnimationsDirectionPairs[trackIndex].first == defaultFlipAnimation && _mixedAnimationsDirectionPairs[trackIndex].second == flippedAnimation && !overwrite) {
		return;
	}

	bool currentFlipX = _skeleton->flipX;

	if (currentFlipX == 0) {
		spAnimation* toSet = spSkeletonData_findAnimation(_skeleton->data, defaultFlipAnimation.c_str());

		if (toSet != 0) {
			spAnimationState_setAnimation(_mainAnimation, trackIndex, toSet, loop);

			_mixedAnimationsDirectionPairs[trackIndex].first = defaultFlipAnimation;
			_mixedAnimationsDirectionPairs[trackIndex].second = flippedAnimation;
		}
		else {
			std::cout << defaultFlipAnimation << "is not a valid name for an animation." << endl;
		}
	}
	else {
		spAnimation* toSet = spSkeletonData_findAnimation(_skeleton->data, flippedAnimation.c_str());

		if (toSet != 0) {
			spAnimationState_setAnimation(_mainAnimation, trackIndex, toSet, loop);

			_mixedAnimationsDirectionPairs[trackIndex].first = defaultFlipAnimation;
			_mixedAnimationsDirectionPairs[trackIndex].second = flippedAnimation;
		}
		else {
			std::cout << flippedAnimation << "is not a valid name for an animation." << endl;
		}
	}


}

void skeleton::setAnimation(std::string name, int time, bool loop,int trackIndex, bool overwrite) {
	while (trackIndex > (int)_mixedAnimationsDirectionPairs.size() - 1) {
		std::pair<std::string, std::string> newPair;
		newPair.first == "";
		newPair.second == "";
		_mixedAnimationsDirectionPairs.push_back(newPair);
	}

	if (_mixedAnimationsDirectionPairs[trackIndex].first == name && _mixedAnimationsDirectionPairs[trackIndex].second == name && !overwrite) {
		return;
	}

	spAnimation* toSet = spSkeletonData_findAnimation(_skeleton->data, name.c_str());
	if (toSet != NULL) {
		float animTime = toSet->duration;

		spAnimationState_setAnimation(_mainAnimation, trackIndex, toSet, loop);

		_mainAnimation->tracks[trackIndex]->timeScale = animTime / (float)(time / 1000);

		_mixedAnimationsDirectionPairs[trackIndex].first = name;
		_mixedAnimationsDirectionPairs[trackIndex].second = name;
	}
	else {
		std::cout << name << "is not a valid name for an animation." << endl;
	}
}

void skeleton::setAnimation(std::string defaultFlipAnimation, std::string flippedAnimation, int time, bool loop, int trackIndex, bool overwrite) {
	while (trackIndex > (int)_mixedAnimationsDirectionPairs.size() - 1) {
		std::pair<std::string, std::string> newPair;
		newPair.first == "";
		newPair.second == "";
		_mixedAnimationsDirectionPairs.push_back(newPair);
	}

	if (_mixedAnimationsDirectionPairs[trackIndex].first == defaultFlipAnimation && _mixedAnimationsDirectionPairs[trackIndex].second == flippedAnimation && !overwrite) {
		return;
	}

	bool currentFlipX = _skeleton->flipX;

	if (currentFlipX == 0) {
		spAnimation* toSet = spSkeletonData_findAnimation(_skeleton->data, defaultFlipAnimation.c_str());

		if (toSet != NULL) {
			spAnimationState_setAnimation(_mainAnimation, trackIndex, toSet, loop);

			float animTime = toSet->duration;

			_mainAnimation->tracks[trackIndex]->timeScale = animTime / ((float)(time) / 1000);

			_mixedAnimationsDirectionPairs[trackIndex].first = defaultFlipAnimation;
			_mixedAnimationsDirectionPairs[trackIndex].second = flippedAnimation;
		}
		else {
			std::cout << defaultFlipAnimation << "is not a valid name for an animation." << endl;
		}
	}
	else {
		spAnimation* toSet = spSkeletonData_findAnimation(_skeleton->data, flippedAnimation.c_str());

		if (toSet != NULL) {
			spAnimationState_setAnimation(_mainAnimation, trackIndex, toSet, loop);

			float animTime = toSet->duration;

			_mainAnimation->tracks[trackIndex]->timeScale = animTime / ((float)(time) / 1000);

			_mixedAnimationsDirectionPairs[trackIndex].first = defaultFlipAnimation;
			_mixedAnimationsDirectionPairs[trackIndex].second = flippedAnimation;
		}
		else {
			std::cout << flippedAnimation << "is not a valid name for an animation." << endl;
		}
	}
}

bool skeleton::getEvent(std::string name) {
	bool first = skeleton::eventMaps[_mainAnimation]->getEventValue(name);
	return first;
}

void skeleton::setMix(std::string from, std::string to, float time) {
	spAnimationStateData_setMixByName(_mainAnimation->data, from.c_str(), to.c_str(), time);
}

std::pair<std::string, std::string> skeleton::getAnimationNames(int trackIndex) {
	if (trackIndex >= _mainAnimation->tracksCount) {
		std::pair<std::string, std::string> returned;
		returned.first = "";
		returned.second = "";
		return returned;
	}
	else {
		return _mixedAnimationsDirectionPairs[trackIndex];
	}
}

spSkeleton* skeleton::getSkeleton() {
	return _skeleton; 
}

glm::vec3 skeleton::getBonePosInfo(std::string boneName) {
	if (_bones.count(boneName)) {
		spSkeleton_updateWorldTransform(_skeleton);

		spRegionAttachment* attachment = SUB_CAST(spRegionAttachment, _bones[boneName]._boneSlot->attachment);

		spRegionAttachment_updateOffset(attachment);

		float vertices[8];

		spRegionAttachment_computeWorldVertices(attachment, _bones[boneName]._boneSlot->bone, vertices);

		glm::vec3 returned;
		returned.x = _bones[boneName]._boneSlot->bone->worldX + _skeleton->x;
		returned.y = _bones[boneName]._boneSlot->bone->worldY + _skeleton->y;

		glm::vec2 point1 = glm::vec2(vertices[0], vertices[1]);
		glm::vec2 point2 = glm::vec2(vertices[2], vertices[3]);

		returned.z = atan2((point2.y - point1.y), (point2.x - point1.x));
		if (returned.z < 0) {
			returned.z += 2 * M_PI;
		}

		return returned;

	}
	else {
		return glm::vec3(0, 0, 0);
	}

}

void skeleton::newBone(std::string name, gengine::spriteSheet image) {
	spSlot* newBoneSlot = spSkeleton_findSlot(_skeleton, name.c_str());
	if (newBoneSlot != 0) {
		bone storedBone;
		storedBone.init(newBoneSlot, image);
		_bones.emplace(std::pair<std::string, bone>(name, storedBone));
	}
}

void skeleton::newBone(std::string name) {
	gengine::spriteSheet image;
	spSlot* newBoneSlot = spSkeleton_findSlot(_skeleton, name.c_str());
	if (newBoneSlot != 0) {
		bone storedBone;
		storedBone.init(newBoneSlot, image);
		_bones.emplace(std::pair<std::string, bone>(name, storedBone));
	}
}

void skeleton::draw(gengine::spriteBatch* spriteBatch) {
	for (auto& it : _bones) {
		it.second.draw(spriteBatch, _drawLayer);
	}
}

void skeleton::update(glm::vec2 position,int deltaTime) {
	skeleton::eventMaps[_mainAnimation]->resetEvents();

	_skeleton->x = position.x + _offset.x;
	_skeleton->y = position.y + _offset.y;
	spAnimationState_update(_mainAnimation, (float)(deltaTime) / 1000);

	_updateAnimations();

	bool currentFlipX = _skeleton->flipX;

	if (currentFlipX != _lastFlipX) {
		for (int i = 0; i < _mainAnimation->tracksCount; i++) {
			if (_mixedAnimationsDirectionPairs[i].first != "") {
				spTrackEntry* track = spAnimationState_getCurrent(_mainAnimation, i);
				float time = track->time;
				int loop = track->loop;

				if (currentFlipX == 0) {
					spAnimation* animationToSet = spSkeletonData_findAnimation(_skeleton->data, _mixedAnimationsDirectionPairs[i].first.c_str());
					spAnimationState_setAnimation(_mainAnimation, i, animationToSet, loop);

					track->time = time;
				}
				else {
					spAnimation* animationToSet = spSkeletonData_findAnimation(_skeleton->data, _mixedAnimationsDirectionPairs[i].second.c_str());
					spAnimationState_setAnimation(_mainAnimation, i, animationToSet, loop);

					track->time = time;
				}
			}
		}
	}
	_lastFlipX = currentFlipX;

	applyMainAnimation();
	spSkeleton_update(_skeleton, (float)(deltaTime) / 1000);
	spSkeleton_updateWorldTransform(_skeleton);
}

void skeleton::updateBegin(glm::vec2 position, int deltaTime) {
	skeleton::eventMaps[_mainAnimation]->resetEvents();

	_skeleton->x = position.x + _offset.x;
	_skeleton->y = position.y + _offset.y;
	spAnimationState_update(_mainAnimation, (float)(deltaTime) / 1000);

	spSkeleton_update(_skeleton, (float)(deltaTime) / 1000);
	_updateAnimations();


	bool currentFlipX = _skeleton->flipX;

	if (currentFlipX != _lastFlipX) {
		for (int i = 0; i < _mainAnimation->tracksCount; i++) {
			if (_mixedAnimationsDirectionPairs[i].first != "") {
				spTrackEntry* track = spAnimationState_getCurrent(_mainAnimation, i);
				if (track != 0) {
					float time = track->time;
					float timeScale = track->timeScale;
					bool loop = track->loop;

					if (currentFlipX == 0) {
						spAnimation* animationToSet = spSkeletonData_findAnimation(_skeleton->data, _mixedAnimationsDirectionPairs[i].first.c_str());
						spAnimationState_setAnimation(_mainAnimation, i, animationToSet, loop);
						track = spAnimationState_getCurrent(_mainAnimation, i);

						track->time = time;
						track->timeScale = timeScale;
					}
					else {
						spAnimation* animationToSet = spSkeletonData_findAnimation(_skeleton->data, _mixedAnimationsDirectionPairs[i].second.c_str());
						spAnimationState_setAnimation(_mainAnimation, i, animationToSet, loop);
						track = spAnimationState_getCurrent(_mainAnimation, i);

						track->time = time;
						track->timeScale = timeScale;

					}
				}
			}
		}
	}
	_lastFlipX = currentFlipX;

	applyMainAnimation();
}

void skeleton::updateEnd() {
	spSkeleton_updateWorldTransform(_skeleton);
}

void skeleton::applyMainAnimation() {
	spAnimationState_apply(_mainAnimation, _skeleton);
}

void skeleton::forceAdditiveRotate(std::string boneName, float angleDeg, float angleDifferenceWhenFlipped) {
	spSkeleton_updateWorldTransform(_skeleton);

	float angleToAdd = angleDeg;

	spBone* bone = _bones[boneName]._boneSlot->bone;

	if (bone->parent != NULL) {
		std::string parentName = bone->parent->data->name;
		float parentAngle = getBonePosInfo(parentName).z * 360 / (2 * M_PI);

		angleToAdd -= parentAngle;
	}

	if (_skeleton->flipX == 0) {
		bone->rotation += angleToAdd;
	}
	else {
		angleToAdd = -angleToAdd;
		bone->rotation += angleToAdd;

		bone->rotation += angleDifferenceWhenFlipped;
	}

}

void skeleton::forceAbsoluteRotate(std::string boneName, float angleDeg){
	spSkeleton_updateWorldTransform(_skeleton);

	spBone* bone = _bones[boneName]._boneSlot->bone;

	bone->rotation = angleDeg;

	if (bone->parent != NULL) {
		std::string parentName = bone->parent->data->name;
		float parentAngle = getBonePosInfo(parentName).z * 360 / (2 * M_PI);

		bone->rotation -= parentAngle;
	}

	if (_skeleton->flipX == 0) {
		bone->rotation = bone->rotation;
	}
	else {
		bone->rotation = -bone->rotation;
	}
}

void skeleton::_updateAnimations() {
	for (int i = 0; i < _mixedAnimationsDirectionPairs.size(); i++) {
		spTrackEntry* track = spAnimationState_getCurrent(_mainAnimation, i);
		if (track != 0) {
			if (_mixedAnimationsDirectionPairs[i].first != "" &&_mixedAnimationsDirectionPairs[i].second != "") {
				if (track->time >= track->animation->duration &&  !track->loop) {
					_mixedAnimationsDirectionPairs[i].first = "";
					_mixedAnimationsDirectionPairs[i].second = "";
				}
			}
		}
	}
}

bool skeleton::_validateTrackIndex(int trackIndex) {
	return trackIndex >= 0 && trackIndex < _mixedAnimationsDirectionPairs.size();
}

void bone::init(spSlot* boneSlot, gengine::spriteSheet& spriteSheet) {
	_boneSlot = boneSlot;
	_image = spriteSheet;
}

void bone::draw(gengine::spriteBatch* spriteBatch, int layer) {
	spRegionAttachment* attachment = SUB_CAST(spRegionAttachment, _boneSlot->attachment);
	spRegionAttachment_updateOffset(attachment);
	float vertices[8];

	spRegionAttachment_computeWorldVertices(attachment, _boneSlot->bone, vertices);

	std::vector<glm::vec2> points;
	points.push_back(glm::vec2(vertices[2], vertices[3]));
	points.push_back(glm::vec2(vertices[0], vertices[1]));
	points.push_back(glm::vec2(vertices[4], vertices[5]));
	points.push_back(glm::vec2(vertices[6], vertices[7]));

	spriteBatch->drawPoints(points, _image.getAnimUV(), _image.getTexture()->id, layer, gengine::color(255, 255, 255, 255));
}

char* _spUtil_readFile(const char* path, int* length) {
	return _readFile(path, length);
}

void _spAtlasPage_createTexture(spAtlasPage* self, const char* path) {
	gengine::GLTexture* texture = gengine::ressourceManager::getTexturePtr(path);
	self->rendererObject = (void*)texture->id;
	self->width = texture->width;
	self->height = texture->height;
}

void _spAtlasPage_disposeTexture(spAtlasPage* self) {

}