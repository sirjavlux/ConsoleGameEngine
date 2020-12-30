#include <iostream>
#include <Windows.h>
#include <chrono>

#include "SEngine.h"

using namespace std;

const int physicsSpeed = 20;

//physics loop
void startPhysics(SEngine* engine) {
	while (engine->isGameRunning()) {
		//start time
		auto start = std::chrono::steady_clock::now();

		//do tasks

		//get end time
		auto end = std::chrono::steady_clock::now();

		//get time differance
		double elapsedTime = double(std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count());

		double elapsedTimeDiff = 1 - ((elapsedTime > 1000.0 ? 1000.0 : elapsedTime) / 1000.0);
		int finalSleepTime = (1000 / physicsSpeed);
		finalSleepTime = int(((double)finalSleepTime) * elapsedTimeDiff);
		Sleep(finalSleepTime);
	}
}