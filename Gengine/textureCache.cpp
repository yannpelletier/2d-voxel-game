#include "textureCache.h"

using namespace std;

namespace gengine{

	textureCache::textureCache()
	{
	}


	textureCache::~textureCache()
	{
	}

	GLTexture* textureCache::getTexturePtr(std::string texturePath) {
		//Look up the texture and see if it's in the map with an iterator
		auto mit = _textureMap.find(texturePath);

		//Checks if it's in the map
		if (mit == _textureMap.end()) {
			//Load the texture
			GLTexture newTexture = imageLoader::loadPNG(texturePath);

			_textureMap.insert(make_pair(texturePath, newTexture));

			return &newTexture;
		}
		return &_textureMap[texturePath];
	}

	GLTexture textureCache::getTexture(std::string texturePath){
		//Look up the texture and see if it's in the map with an iterator
		auto mit = _textureMap.find(texturePath);

		//Checks if it's in the map
		if (mit == _textureMap.end()){
			//Load the texture
			GLTexture newTexture = imageLoader::loadPNG(texturePath);

			_textureMap.insert(make_pair(texturePath, newTexture));

			return newTexture;
		}
		return _textureMap[texturePath];
	}

}