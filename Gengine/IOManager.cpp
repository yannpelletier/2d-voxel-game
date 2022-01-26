#include "IOManager.h"


using namespace std;

namespace gengine{

	bool IOManager::readFileToBuffer(string filePath, vector<unsigned char>& buffer){
		ifstream file(filePath, ios::binary);
		if (file.fail()){
			perror(filePath.c_str());
			return false;
		}

		//Seek to the end
		file.seekg(0, ios::end);

		//Get the file size
		int fileSize = file.tellg();
		file.seekg(0, ios::beg);

		buffer.resize(fileSize);

		file.read((char *)&(buffer[0]), fileSize);
		file.close();

		return true;
	}

	bool IOManager::readFileToBuffer(string filePath, std::string& buffer) {
		ifstream file(filePath, ios::binary);
		if (file.fail()) {
			perror(filePath.c_str());
			return false;
		}

		//Seek to the end
		file.seekg(0, ios::end);

		//Get the file size
		int fileSize = file.tellg();
		file.seekg(0, ios::beg);

		buffer.resize(fileSize);

		file.read((char *)&(buffer[0]), fileSize);
		file.close();

		return true;
	}

}