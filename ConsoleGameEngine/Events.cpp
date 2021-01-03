#include <iostream>

#include "DefinedKeys.h"
#include "SEngine.h"

using namespace std;

//KeyPressed event
void pressedKeyEvent(SEngine * engine) {
	int* lastKey = getLastKeyPressed();
	int key = *lastKey;

	//temp variables
	int x = engine->getCameraX();
	int y = engine->getCameraY();

	//do stuff depending on key type
	switch (key) {
	case KEY_A: 
		//test move camera
		x -= 8;
		engine->teleportCamera(x, y);
		break;
	case KEY_W: 
		//test move camera
		y += 8;
		engine->teleportCamera(x, y);
		break;
	case KEY_S: 
		//test move camera
		y -= 8;
		engine->teleportCamera(x, y);
		break;
	case KEY_D: 
		//test move camera
		x += 8;
		engine->teleportCamera(x, y);
		break;
	case KEY_SPACE: 
		break;
	}

	//cout << key << endl;

	*lastKey = 0; //reset key
}