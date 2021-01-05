#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <mutex>

#include "SEngine.h"
#include "Utils.h"

using namespace std;

map<Key, KeyState> keyStates;

//secure key state functions
mutex keyStatesMutex;
void safeSetKeyState(Key key, KeyState state) {
	lock_guard<mutex> lock(keyStatesMutex);
	keyStates[key] = state;
}

KeyState safelyGetKeyState(Key key) {
	lock_guard<mutex> lock(keyStatesMutex);
	return keyStates[key];
}

map<Key, KeyState> safelyGetKeyStateMap() {
	lock_guard<mutex> lock(keyStatesMutex);
	return keyStates;
}

//check if key is pressed
bool keyPressed(Key key) {
	return (GetAsyncKeyState(getEnumInteger(key)) & 0x8000) != 0;
}

//start key checking loop
void fireKeyEvent(Key key, KeyState state, SEngine* engine);
void startCheckingKeyInput(SEngine* engine) {
	//set all default key values
	for (int i = 0; i < sizeof(keyArr) / sizeof(keyArr[0]); i++) {
		keyStates[keyArr[i]] = KeyState::idle;
	}
	while (engine->isGameRunning()) {
		//start time
		auto start = std::chrono::steady_clock::now();

		//check keys and fire events
		for (int i = 0; i < sizeof(keyArr) / sizeof(keyArr[0]); i++) {
			Key key = keyArr[i];
			bool pressed = keyPressed(key);
			KeyState oldState = safelyGetKeyState(key);
			//set to key pressed state
			if (pressed) {
				safeSetKeyState(key, KeyState::down);
				//trigger event
				fireKeyEvent(key, KeyState::down, engine);
			}
			//set to relesased or idle
			else if (oldState == KeyState::down) {
				safeSetKeyState(key, KeyState::up);
				//trigger event
				fireKeyEvent(key, KeyState::up, engine);
			}
			else {
				safeSetKeyState(key, KeyState::idle);
			}
		}

		//get end time
		auto end = std::chrono::steady_clock::now();

		//get time differance
		double elapsedTime = double(std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count());

		double elapsedTimeDiff = 1 - ((elapsedTime > 1000.0 ? 1000.0 : elapsedTime) / 1000.0);
		int finalSleepTime = (1000 / engine->getTickSpeed());
		finalSleepTime = int(((double)finalSleepTime) * elapsedTimeDiff);
		Sleep(finalSleepTime);
	}
}

//fire key event
void keyAUpdateEvent(KeyState state, SEngine* engine);
void keyWUpdateEvent(KeyState state, SEngine* engine);
void keySUpdateEvent(KeyState state, SEngine* engine);
void keyDUpdateEvent(KeyState state, SEngine* engine);
void keySPACEUpdateEvent(KeyState state, SEngine* engine);

void fireKeyEvent(Key key, KeyState state, SEngine * engine) {
	switch (key) {
	case Key::KEY_A:
		keyAUpdateEvent(state, engine);
		break;
	case Key::KEY_W:
		keyWUpdateEvent(state, engine);
		break;
	case Key::KEY_S:
		keySUpdateEvent(state, engine);
		break;
	case Key::KEY_D:
		keyDUpdateEvent(state, engine);
		break;
	case Key::KEY_SPACE:
		keySPACEUpdateEvent(state, engine);
		break;
	}
}