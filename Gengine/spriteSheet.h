#pragma once
#include "GLTexture.h"
#include "animation.h"
#include <map>
#include <iostream>
#include <glm/glm.hpp>

namespace gengine {
	class spriteSheet
	{
	public:
		void init(const GLTexture& Texture) {
			_texture = Texture;
		}
		void init(const GLTexture& Texture, const glm::ivec2& tileDims) {
			_texture = Texture;
			_dims = tileDims;
		}

		void addAnimation(std::string animationName, int beginIndex, int endIndex, int speedBetweenFrames) {
			if (_dims.x != -1 && _dims.y != -1) {
				if (endIndex >= beginIndex) {
					animation newAnim;
					newAnim.init(beginIndex, endIndex, speedBetweenFrames);
					_animations.insert(make_pair(animationName, newAnim));
				}
			}
		}

		glm::fvec4 getAnimUV() {
			glm::fvec4 returnedUVs;

			if (_animations.empty()) {
				
				if (_direction > 0) {
					returnedUVs = glm::fvec4(0.0f, 0.0f, 1.0f, 1.0f);
				}
				else {
					returnedUVs = glm::fvec4(1.0f, 0.0f, -1.0f, 1.0f);
				}

				if (_flipY) {
					returnedUVs.w = -1;
				}

			}
			else {
				returnedUVs = _getUVs(_animations[_currentAnimation].getIndex());

				if (_direction < 0) {
					returnedUVs.x += 1.0f / (float)_dims.x;
					returnedUVs.z *= _direction;
				}
				if (_flipY == 1) {
					returnedUVs.y += 1.0f / (float)_dims.y;
					returnedUVs.w *= -1;
				}
			}

			return returnedUVs;
		}

		void update() {
			if (!_animations.empty()) {
				if (_loop == -1) {
					_animations[_currentAnimation].update();
				}
				else {
					if (_loop > 0) {
						if (_animations[_currentAnimation].update()) {
							_loop -= 1;

						}
					}
				}
			}
		}

		void changeAnimation(std::string animationName) {

			if (animationName != _currentAnimation) {
				auto it = _animations.count(animationName);
				if (it != 0) {
					_currentAnimation = animationName;
					_animations[_currentAnimation].reset();
				}
			}
		}

		void setLoops(int loops) {
			_loop = loops;
		}

		void changeDirection(bool direction) {
			_direction = direction;
		}

		void setFlipY(bool flipY) {
			_flipY = flipY;
		}

		void setFrame(int frameIndex) {
			_animations[_currentAnimation].setCurrentIndex(frameIndex);
		}

		glm::fvec4 getUvsOf(int frameIndex) {
			glm::fvec4 returnedUVs;

			returnedUVs = _getUVs(frameIndex);


			return returnedUVs;
		}

		GLTexture* getTexture() { return &_texture;  }

	private:
		std::map<std::string, animation> _animations;

		glm::fvec4 _getUVs(int index) {
			int xTile = index % _dims.x;
			int yTile = index / _dims.x;

			glm::fvec4 returnedUv;
			returnedUv.x = (float)(xTile) / (float)(_dims.x);
			returnedUv.y = float(yTile) / (float)(_dims.y);
			returnedUv.z = 1.0f / (float)(_dims.x);
			returnedUv.w = 1.0f / (float)(_dims.y);

			return returnedUv;
		}

		int _loop = -1;
		std::string _currentAnimation = "default";
		int _direction = 1;
		bool _flipY = 0;
		GLTexture _texture;
		glm::ivec2 _dims = glm::ivec2(-1,-1);
	};

}