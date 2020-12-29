#include <iostream>

#include "DefinedKeys.h"
#include "SEngine.h"

using namespace std;

//KeyPressed event
void pressedKeyEvent() {
	int* lastKey = getLastKeyPressed();
	int key = *lastKey;

	//temp variables
	int x = getCameraX();
	int y = getCameraY();

	//do stuff depending on key type
	switch (key) {
	case KEY_A: 
		//test move camera
		x -= 2;
		teleportCamera(x, y);
		break;
	case KEY_W: 
		//test move camera
		y++;
		teleportCamera(x, y);
		break;
	case KEY_S: 
		//test move camera
		y--;
		teleportCamera(x, y);
		break;
	case KEY_D: 
		//test move camera
		x += 2;
		teleportCamera(x, y);
		break;
	case KEY_SPACE: 
		break;
	}

	//cout << key << endl;

	*lastKey = 0; //reset key
}

//update events
void updateEvents() {
	if (*getLastKeyPressed() > 0) pressedKeyEvent(); //fire if key was pressed
}