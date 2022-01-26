#include "ressourceManager.h"

namespace gengine{
	textureCache ressourceManager::_textureCache;

	GLTexture ressourceManager::getTexture(std::string texturePath){
		return _textureCache.getTexture(texturePath);
	}
	GLTexture* ressourceManager::getTexturePtr(std::string texturePath) {
		return _textureCache.getTexturePtr(texturePath);
	}

}