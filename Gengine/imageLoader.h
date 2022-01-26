#pragma once

#include "GLTexture.h"
#include <string>
#include "errors.h"
#include "GL/glew.h"
#include <iostream>
#include "IOManager.h"
#include "picoPNG.h"

namespace gengine{
	class imageLoader
	{
	public:
		static GLTexture loadPNG(std::string filePath);
	};
}