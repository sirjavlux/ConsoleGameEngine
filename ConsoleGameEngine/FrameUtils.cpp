#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <sstream>
#include <cmath>
#include <deque>
#include <Windows.h>
#include <thread>
#include <mutex>
#include <chrono>
#include<cmath>
#include <algorithm>

#include "SEngine.h"
#include "Utils.h"

using namespace std;

//cached gameobjects
list<GameObject*> gameObjects;
list<GameObject*> closeToFrame;

map<int, list<GameObject*>> inFrameUpdated;
map<int, int> inFrameUpdatedLines;

string textBox = "";

//fix wierd out of frame bug <-----------------------
//check if gameobject overlaps frame
bool overlapsFrame(SEngine* engine, GameObject* obj) {

	int width = (int) (getScreenWidth() * 1.2);
	int height = (int) (getScreenHeight() * 1.2);

	//frame coordinates
	int cameraX = engine->getCameraX() - width / 2;
	int cameraY = engine->getCameraY() - height / 2;
	int frameX1 = cameraX; int frameX2 = cameraX + width;
	int frameY1 = cameraY; int frameY2 = cameraY + height;

	//object coordinates
	int x1 = obj->getX(); int x2 = x1 + obj->getWidth();
	int y1 = obj->getY(); int y2 = y1 + obj->getHeight();

	if (x1 >= frameX2 || x2 <= frameX1) return false;
	else if (y2 <= frameY1 || y1 >= frameY2) return false;
	else return true;
}

//check if gameobject overlaps frame with offset
bool overlapsFrame(SEngine* engine, GameObject * obj, int offset) {

	int width = (int)(getScreenWidth() * 1.2);
	int height = (int)(getScreenHeight() * 1.2);

	//frame coordinates
	int cameraX = engine->getCameraX() - width / 2;
	int cameraY = engine->getCameraY() - height / 2;
	int frameX1 = cameraX; int frameX2 = cameraX + width;
	int frameY1 = cameraY; int frameY2 = cameraY + height;

	//object coordinates
	int scale = engine->getPixelScale();
	int x1 = obj->getX(); int x2 = x1 + obj->getWidth();
	int y1 = obj->getY(); int y2 = y1 + obj->getHeight();

	if (x1 >= frameX2 + offset * scale || x2 <= frameX1 - offset * scale) return false;
	else if (y2 <= frameY1 - offset * scale || y1 >= frameY2 + offset * scale) return false;
	else return true;
}

//update scene objects
void safeClear();
void updateFrameObjects(SEngine * engine) {
	
	safeClear();

	if (getActiveScene() != nullptr) {
		Scene* activeScene = getActiveScene();

		vector<string> objects = *activeScene->getObjectNames();

		vector<string>::iterator iter = objects.begin();

		while (iter != objects.end()) {
			GameObject* obj = getGameObject(*iter);
			registerObjectToFrame(engine, obj);
			iter++;
		}
	}
}

//distance 
double distance(int x2, int x1, int y2, int y1) {
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

//safe access to variables
std::mutex gameObjectsMutex, closeToFrameMutex, updatedFrameMutex, inFrameUpdatedLinesMutex;

void safePushCloseToFrame(GameObject* obj) {
	std::lock_guard<std::mutex> lock(closeToFrameMutex);
	if (find(closeToFrame.begin(), closeToFrame.end(), obj) != closeToFrame.end()) return;
	closeToFrame.push_back(obj);
}

void safeRemoveCloseToFrame(GameObject* obj) {
	std::lock_guard<std::mutex> lock(closeToFrameMutex);
	closeToFrame.remove(obj);
}

void safePushToUpdatedFrame(GameObject* obj) {
	std::lock_guard<std::mutex> lock(updatedFrameMutex);
	int layer = obj->getLayer();
	if (std::find(inFrameUpdated[layer].begin(), inFrameUpdated[layer].end(), obj) != inFrameUpdated[layer].end()) return;
	inFrameUpdated[layer].push_back(obj);
}

void safeRemoveUpdatedFrame(GameObject* obj) {
	std::lock_guard<std::mutex> lock(updatedFrameMutex);
	inFrameUpdated[obj->getLayer()].remove(obj);
}

void safePushToGameObjects(GameObject* obj) {
	std::lock_guard<std::mutex> lock(gameObjectsMutex);
	gameObjects.push_back(obj);
}

list<GameObject*> saflyGetCloseToFrame() {
	std::lock_guard<std::mutex> lock(closeToFrameMutex);
	return closeToFrame;
}

list<GameObject*> saflyGetGameObjects() {
	std::lock_guard<std::mutex> lock(gameObjectsMutex);
	return gameObjects;
}

map<int, list<GameObject*>> getInFrameUpdated() {
	std::lock_guard<std::mutex> lock(updatedFrameMutex);
	return inFrameUpdated;
}

map<int, int> & getInFrameUpdatedLines() {
	std::lock_guard<std::mutex> lock(inFrameUpdatedLinesMutex);
	return inFrameUpdatedLines;
}

void setInFrameUpdatedLines(int layer, int lines) {
	std::lock_guard<std::mutex> lock(inFrameUpdatedLinesMutex);
	inFrameUpdatedLines[layer] = lines;
}

void safeClear() {
	std::lock_guard<std::mutex> lock(gameObjectsMutex);
	gameObjects.clear();

	std::lock_guard<std::mutex> lock2(closeToFrameMutex);
	closeToFrame.clear();

	std::lock_guard<std::mutex> lock3(updatedFrameMutex);
	inFrameUpdated.clear();
}

/*/////////////////////////////////////
* update objects close to frame
*//////////////////////////////////////

//check for objects close to frame and update
void checkAndAddObjectsCloseToFrame(SEngine * engine, int from, int to) {
	list<GameObject*> objects = saflyGetGameObjects();
	list<GameObject*>::iterator iter = objects.begin();
	while (iter != objects.end()) {
		if (overlapsFrame(engine, *iter, (int) (sqrt(getScreenWidth() * getScreenHeight()) / 2))) {
			safePushCloseToFrame(*iter);
		}
		iter++;
	}
}

int lastCloseUpcateCameraLocX = 0;
int lastCloseUpcateCameraLocY = 0;

//check removal of close to frame
void checkAndRemoveObjectsCloseToFrame(SEngine* engine) {
	list<GameObject*> objects = saflyGetCloseToFrame();
	list<GameObject*>::iterator iter = objects.begin();
	while (iter != objects.end()) {
		if (!overlapsFrame(engine, *iter, (int)(sqrt(getScreenWidth() * getScreenHeight()) / 2))) {
			safeRemoveCloseToFrame(*iter);
		}
		iter++;
	}
}

//update and check objects close to frame
void updateCloseToFrame(SEngine* engine) {
	//calculate distance
	double dist = distance(engine->getCameraX(), lastCloseUpcateCameraLocX, engine->getCameraY(), lastCloseUpcateCameraLocY);
	//check if distance is far enugh for update
	if (dist >= sqrt(getScreenWidth() * getScreenHeight()) / 4) {

		//check if objects should be removed
		std::thread objRemovalThread(checkAndRemoveObjectsCloseToFrame, engine);

		lastCloseUpcateCameraLocX = engine->getCameraX();
		lastCloseUpcateCameraLocY = engine->getCameraY();

		//calculate threads needed
		const int threadCap = 2;
		const int startSize = gameObjects.size();
		int threadsToUse = startSize / 10000;
		if (threadsToUse > threadCap) threadsToUse = threadCap;
		else if (threadsToUse < 1) threadsToUse = 1;
		if (threadsToUse == 1) {
			checkAndAddObjectsCloseToFrame(engine, 0, (int)startSize);
		}
		else {
			std::vector<std::thread> threads;
			//do tasks
			double increment = 1.0 / threadsToUse;
			double begin = 0;
			for (int i = 0; i < threadsToUse; i++) {
				threads.push_back(std::thread(checkAndAddObjectsCloseToFrame, engine, (int)startSize * begin, (int)startSize * (begin + increment)));
				begin += increment;
			}
			//finish tasks 
			for (int i = 0; i < threadsToUse; i++) {
				threads.at(i).join();
			}
		}

		//join object removal thread
		objRemovalThread.join();
	}
}

//register object for possible rendering
void registerObjectToFrame(SEngine * engine, GameObject* obj) {
	if (overlapsFrame(engine, obj)) {
		safePushToUpdatedFrame(obj);
	}
	else if (overlapsFrame(engine, obj, (int)(sqrt(getScreenWidth() * getScreenHeight()) / 2))) {
		safePushCloseToFrame(obj);
	}

	safePushToGameObjects(obj);
}

//start updating frame objects
const int objectSearchDelay = 10;
const int frameObjectUpdatesPerSec = 2;

void updateUpdatedInFrame(SEngine* engine) {
	while (engine->isGameRunning()) {

		//get start time
		auto start = std::chrono::steady_clock::now();

		//in frame map
		map<int, list<GameObject*>> inFrameUpdated = getInFrameUpdated();

		//add
		list<GameObject*> closeToFrame = saflyGetCloseToFrame();
		list<GameObject*>::iterator iter = closeToFrame.begin();
		while (iter != closeToFrame.end()) {
			if (overlapsFrame(engine, *iter)) {
				int layer = (*iter)->getLayer();
				safePushToUpdatedFrame((*iter));
			}
			iter++;
		}

		//remove
		map<int, list<GameObject*>>::iterator iter2 = inFrameUpdated.begin();
		while (iter2 != inFrameUpdated.end()) {
			int lines = 0; // to calculate thread use during frame rendering
			list<GameObject*> objects = iter2->second;
			int layer = iter2->first;
			list<GameObject*>::iterator it = objects.begin();
			while (it != objects.end()) {
				if (!overlapsFrame(engine, *it)) safeRemoveUpdatedFrame(*it);
				else lines += (*it)->getImage()->getVector()->size(); // get lines of pixels in frame
				it++;
			}
			setInFrameUpdatedLines(layer, lines); // set lines
			iter2++;
		}

		//get end time
		auto end = std::chrono::steady_clock::now();

		//get time differance
		double elapsedTime = double(std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count());

		double elapsedTimeDiff = 1 - ((elapsedTime > 1000.0 ? 1000.0 : elapsedTime) / 1000.0);
		int finalSleepTime = (1000 / frameObjectUpdatesPerSec);
		finalSleepTime = int(((double)finalSleepTime) * elapsedTimeDiff);
		Sleep(finalSleepTime);
	}
}

//find all objects and decide wether to keep them
void findObjectsInFrame(SEngine* engine) {
	thread update(updateUpdatedInFrame, engine);
	while (engine->isGameRunning()) {
		//get start time
		auto start = std::chrono::steady_clock::now();

		//update camera caching and such
		if (getActiveScene() != nullptr) {
			updateCloseToFrame(engine);
		}

		//get end time
		auto end = std::chrono::steady_clock::now();

		//get time differance
		double elapsedTime = double(std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count());

		double elapsedTimeDiff = 1 - ((elapsedTime > 1000.0 ? 1000.0 : elapsedTime) / 1000.0);
		int finalSleepTime = (1000 / objectSearchDelay);
		finalSleepTime = int (((double)finalSleepTime) * elapsedTimeDiff);
		Sleep(finalSleepTime);
	}
	update.join();
}

map<int, list<GameObject*>> getInFrame() {
	std::lock_guard<std::mutex> lock(updatedFrameMutex);
	return inFrameUpdated;
}

void setBottomTextBox(string str) {
	textBox = str;
}

string getBottomTextBox() {
	return textBox;
}