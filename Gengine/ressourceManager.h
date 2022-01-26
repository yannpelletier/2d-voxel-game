#pragma once
#include <map>
#include <string>
#include "textureCache.h"
#include "GLTexture.h"

namespace gengine{
	class ressourceManager
	{
	public:
		static GLTexture getTexture(std::string texturePath);
		static GLTexture* getTexturePtr(std::string texturePath);

	private:
		static textureCache _textureCache;
	};

}