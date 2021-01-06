#include <iostream>

#include "SEngine.h"

using namespace std;

void keyAUpdateEvent(KeyState state, SEngine* engine) {
	if (state == KeyState::down) {
		if (engine->hasCameraObjectAttatched()) {
			GameObject* obj = engine->getCameraFollowObject();
			Vector2D vel(-1, 0);
			obj->addForce(vel);
		}

	}
}

void keyWUpdateEvent(KeyState state, SEngine* engine) {
	if (state == KeyState::down) {
		if (engine->hasCameraObjectAttatched()) {
			GameObject* obj = engine->getCameraFollowObject();
			Vector2D vel(0, 1);
			obj->addForce(vel);
		}
	}
}

void keySUpdateEvent(KeyState state, SEngine* engine) {
	if (state == KeyState::down) {
		if (engine->hasCameraObjectAttatched()) {
			GameObject* obj = engine->getCameraFollowObject();
			Vector2D vel(0, -1);
			obj->addForce(vel);
		}
	}
}

void keyDUpdateEvent(KeyState state, SEngine* engine) {
	if (state == KeyState::down) {
		if (engine->hasCameraObjectAttatched()) {
			GameObject* obj = engine->getCameraFollowObject();
			Vector2D vel(1, 0);
			obj->addForce(vel);
		}
	}
}

void keySPACEUpdateEvent(KeyState state, SEngine* engine) {

}