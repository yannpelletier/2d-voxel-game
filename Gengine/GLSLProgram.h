#pragma once

#include <string>
#include <GL/glew.h>
#include "errors.h"
#include "IOManager.h"

#include <vector>
#include <fstream>


using namespace std;

namespace gengine{
	class GLSLProgram
	{
	public:
		GLSLProgram();
		~GLSLProgram();

		void compileShaders(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);

		void compileShadersFromSource(const char* vertexSource, const char* fragmentSource);

		void linkShaders();

		void addAttribute(const std::string& attributeName);

		GLint getUniformLocation(const std::string& uniformName);

		void use();
		void unuse();

		void dispose();
	private:

		int _numAttributes;

		void compileShader(const char* source, const std::string& name, GLuint id);

		GLuint _programID;

		GLuint _vertexShaderID;
		GLuint _fragmentShaderID;
	};

}