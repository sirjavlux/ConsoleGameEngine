#include <iostream>
#include <Windows.h>
#include <chrono>
#include <mutex>
#include <map> 
#include <math.h>
#include <cmath>

#include "SEngine.h"

using namespace std;

const int physicsSpeed = 50;

//physics loop
void moveObjects(SEngine* engine);
void startPhysics(SEngine* engine) {
	while (engine->isGameRunning()) {
		//start time
		auto start = std::chrono::steady_clock::now();

		//do tasks
		moveObjects(engine);

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

/*////////////////////////////
* Add force to objects
*/////////////////////////////

// modify values safely
mutex addForceObjectsMutex, oldAddForceObjectsMutex;
map<GameObject*, Vector2D*> * addForceObjects = new map<GameObject*, Vector2D*>();
map<GameObject*, Vector2D*> * oldAddForceObjects = new map<GameObject*, Vector2D*>();

Vector2D* safelyGetOldForce(GameObject* obj) {
	lock_guard<mutex> lock(oldAddForceObjectsMutex);
	map<GameObject*, Vector2D*>::iterator iter = oldAddForceObjects->find(obj);
	if (iter != oldAddForceObjects->end()) return oldAddForceObjects->at(obj);
	else return nullptr;
}

Vector2D* safelyGetForce(GameObject* obj) {
	lock_guard<mutex> lock(addForceObjectsMutex);
	map<GameObject*, Vector2D*>::iterator iter = addForceObjects->find(obj);
	if (iter != addForceObjects->end()) {
		Vector2D* vec = addForceObjects->at(obj);
		int xMoveAmount = (int)round(vec->getX());
		int yMoveAmount = (int)round(vec->getY());
		if (xMoveAmount > 10000 || yMoveAmount > 10000 || xMoveAmount < -10000 || yMoveAmount < -10000) {
			return safelyGetOldForce(obj);
		}
		else {
			return addForceObjects->at(obj);
		}
	}
	else return nullptr;
}

void safelySetAddForceVector(GameObject* obj, Vector2D* vec) {
	lock_guard<mutex> lock(addForceObjectsMutex);
	(*addForceObjects)[obj] = vec;
}

void safelySetOldAddForceVector(GameObject* obj, Vector2D* vec) {
	lock_guard<mutex> lock(oldAddForceObjectsMutex);
	(*oldAddForceObjects)[obj] = vec;
}

void safelyRemoveAddForceVector(GameObject* obj) {
	lock_guard<mutex> lock(addForceObjectsMutex);
	addForceObjects->erase(obj);
}

map<GameObject*, Vector2D*> * safelyGetAddForceObjects() {
	lock_guard<mutex> lock(addForceObjectsMutex);
	return addForceObjects;
}

// loop trough objects, modify vectors and move objects
void moveObjects(SEngine* engine) {
	map<GameObject*, Vector2D*> * objectMap = safelyGetAddForceObjects();
	map<GameObject*, Vector2D*>::iterator iter = objectMap->begin();

	while (iter != objectMap->end()) {

		GameObject* obj = iter->first;
		Vector2D* vec = iter->second;

		int xMoveAmount = (int)round(vec->getX());
		int yMoveAmount = (int)round(vec->getY());

		if (xMoveAmount > 10000 || yMoveAmount > 10000 || xMoveAmount < -10000 || yMoveAmount < -10000) {
			continue;
		}
		else {
			safelySetOldAddForceVector(obj, vec);
		}

		// move object
		obj->teleport(obj->getX() + xMoveAmount, obj->getY() + yMoveAmount);

		iter++;
	}
}