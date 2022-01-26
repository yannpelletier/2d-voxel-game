#pragma once
#include <map>
#include <string>
#include "GLTexture.h"
#include "imageLoader.h"
#include <map>
#include <iostream>

namespace gengine{
	class textureCache
	{
	public:
		textureCache();
		~textureCache();

		GLTexture* getTexturePtr(std::string texturePath);
		GLTexture getTexture(std::string texturePath);

	private:
		std::map<std::string, GLTexture> _textureMap;
	};

}