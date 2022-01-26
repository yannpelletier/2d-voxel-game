#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

namespace gengine{
	class IOManager
	{
	public:
		static bool readFileToBuffer(string filepath, vector<unsigned char>& buffer);
		static bool readFileToBuffer(string filepath, string& buffer);
	};

}