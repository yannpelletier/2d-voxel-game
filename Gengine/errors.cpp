#include "errors.h"

#include <iostream>


using namespace std;

namespace gengine{
	void fatalError(string errorString){
		cout << errorString << endl;

		cout << "Enter any key to quit" << endl;
		int temp;
		cin >> temp;

		SDL_Quit();
		exit(1);
	}
}