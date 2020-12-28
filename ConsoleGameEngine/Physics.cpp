#include <iostream>
#include <Windows.h>

#include "Engine.h"

using namespace std;

const int physicsSpeed = 20;

//physics loop
void startPhysics() {
	while (isGameRunning()) {

		Sleep(1000 / physicsSpeed);
	}
}