#include <iostream>

#include "SEngine.h"

using namespace std;

void keyAUpdateEvent(KeyState state, SEngine* engine) {
	if (state == KeyState::down) {
		int x = engine->getCameraX();
		int y = engine->getCameraY();
		x -= engine->getPixelScale();
		engine->teleportCamera(x, y);
	}
}

void keyWUpdateEvent(KeyState state, SEngine* engine) {
	if (state == KeyState::down) {
		int x = engine->getCameraX();
		int y = engine->getCameraY();
		y += engine->getPixelScale();
		engine->teleportCamera(x, y);
	}
}

void keySUpdateEvent(KeyState state, SEngine* engine) {
	if (state == KeyState::down) {
		int x = engine->getCameraX();
		int y = engine->getCameraY();
		y -= engine->getPixelScale();
		engine->teleportCamera(x, y);
	}
}

void keyDUpdateEvent(KeyState state, SEngine* engine) {
	if (state == KeyState::down) {
		int x = engine->getCameraX();
		int y = engine->getCameraY();
		x += engine->getPixelScale();
		engine->teleportCamera(x, y);
	}
}

void keySPACEUpdateEvent(KeyState state, SEngine* engine) {

}