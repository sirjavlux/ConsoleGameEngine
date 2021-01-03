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
list<string> notInFrame;
map<int, list<string>> inFrame;
map<int, list<string>> inFrameUpdated;
list<string> closeToFrame;
string textBox = "";

std::mutex copyInFrame, copyInFrameUpdated;
map<int, list<string>> getCopyOfInFrame() {
	lock_guard<mutex> lock(copyInFrame);
	return inFrame;
}

//check if gameobject overlaps frame
bool overlapsFrame(SEngine* engine, int x, int y, int h, int w) {
	int width = getScreenWidth() * 1.5;
	int height = getScreenHeight() * 1.5;
	//frame coordinates
	int cameraX = engine->getCameraX() - width / 2;
	int cameraY = engine->getCameraY() - height / 2;
	int frameX1 = cameraX; int frameX2 = cameraX + width;
	int frameY1 = cameraY; int frameY2 = cameraY + height;

	//object coordinates
	int scale = engine->getPixelScale();
	int x1 = x * scale - cameraX; int x2 = x * scale - cameraX + w * scale;
	int y1 = y * scale - cameraY; int y2 = y * scale - cameraY + h * scale;

	if (x1 > width || x2 < 0) return false;
	else if (y2 < 0 || y1 > height) return false;
	else return true;
}

//check if gameobject overlaps frame with offset
bool overlapsFrame(SEngine* engine, int x, int y, int h, int w, int offset) {
	int width = getScreenWidth() * 1.5;
	int height = getScreenHeight() * 1.5;
	//frame coordinates
	int cameraX = engine->getCameraX() - width / 2;
	int cameraY = engine->getCameraY() - height / 2;
	int frameX1 = cameraX; int frameX2 = cameraX + width;
	int frameY1 = cameraY; int frameY2 = cameraY + height;

	//object coordinates
	int scale = engine->getPixelScale();
	int x1 = x * scale - cameraX; int x2 = x * scale - cameraX + w * scale;
	int y1 = y * scale - cameraY; int y2 = y * scale - cameraY + h * scale;

	if (x1 > width + offset * scale || x2 < 0 - offset * scale) return false;
	else if (y2 < 0 - offset * scale || y1 > height + offset * scale) return false;
	else return true;
}

//update scene objects
void updateFrameObjects(SEngine * engine) {
	notInFrame.clear();
	inFrame.clear();
	closeToFrame.clear();

	if (getActiveScene() != nullptr) {
		Scene* activeScene = getActiveScene();

		vector<string> objects = *activeScene->getObjectNames();

		vector<string>::iterator iter = objects.begin();

		while (iter != objects.end()) {
			GameObject* obj = getGameObject(*iter);
			registerObjectToFrame(engine, *iter, obj->getX(), obj->getY(), obj->getHeight(), obj->getWidth(), obj->getLayer());
			iter++;
		}
	}
}

int lastCloseUpcateCameraLocX = 0;
int lastCloseUpcateCameraLocY = 0;

//distance 
double distance(int x2, int x1, int y2, int y1) {
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

//safe methods to manage object frame access
std::mutex closeToFrameMutex, newNotInFrameMutex, notInFrameMutex, inFrameMutex, removeNotInFrameMutex, inFrameUpdatedMutex, removeInFrameMutex;
void pushCloseToFrame(string name) {
	lock_guard<mutex> lock(closeToFrameMutex);
	closeToFrame.push_back(name);
}
void pushNotInFrame(string name) {
	lock_guard<mutex> lock(notInFrameMutex);
	notInFrame.push_back(name);
}
void pushNewNotInFrame(string name, vector<string>* newNotInFrame) {
	lock_guard<mutex> lock(newNotInFrameMutex);
	newNotInFrame->push_back(name);
}
void pushInFrame(string name, int layer) {
	lock_guard<mutex> lock(inFrameMutex);
	inFrame[layer].push_back(name);
}
void pushInFrameUpdated(string name, int layer, bool addRemove) {
	lock_guard<mutex> lock(copyInFrameUpdated);
	lock_guard<mutex> lock2(inFrameUpdatedMutex);
	if (addRemove) {
		inFrameUpdated[layer].push_back(name);
	}
	else {
		//inFrameUpdated.at(layer).erase(remove(inFrameUpdated.at(layer).begin(), inFrameUpdated.at(layer).end(), name), inFrameUpdated.at(layer).end());
		inFrameUpdated.at(layer).remove(name);
	}
}
void removeElementNotInFrame(string name) {
	lock_guard<mutex> lock(removeNotInFrameMutex);
	//notInFrame.erase(remove(notInFrame.begin(), notInFrame.end(), name), notInFrame.end());
	notInFrame.remove(name);
}
void removeElementInFrame(string name, int layer) {
	lock_guard<mutex> lock(removeInFrameMutex);
	//inFrame.at(layer).erase(remove(inFrame.at(layer).begin(), inFrame.at(layer).end(), name), inFrame.at(layer).end());
	inFrame.at(layer).remove(name);
}

//check and update not in frame objects
void checkNotInFrame(int from, int to, list<string> * newNotInFrame, SEngine* engine) {
	list<string> objects = notInFrame;
	list<string>::iterator it = objects.begin();
	while (it != objects.end()) {
		string & objName = *it;
		if (!objName.empty()) {
			GameObject* obj = getGameObject(objName);
			if (overlapsFrame(engine, obj->getX(), obj->getY(), obj->getHeight(), obj->getWidth(), sqrt(getScreenWidth() * getScreenHeight()) / 4)) {
				pushCloseToFrame(objName);
				removeElementNotInFrame(objName);
			}
		}
		it++;
	}
}

//check and update objects in frame
void checkInFrame(SEngine* engine) {
	//update frame objects
	map<int, list<string>> mapObjects2 = getCopyOfInFrame();
	map<int, list<string>>::iterator iter2 = mapObjects2.begin();
	while (iter2 != mapObjects2.end()) {
		list<string> objects = (*iter2).second;
		list<string>::iterator it = objects.begin();
		int layer = (*iter2).first;
		while (it != objects.end()) {
			string objName = *it;
			if (!objName.empty()) {
				GameObject* obj = getGameObject(objName);
				if (!overlapsFrame(engine, obj->getX(), obj->getY(), obj->getHeight(), obj->getWidth())) {
					pushCloseToFrame(objName);
					removeElementInFrame(objName, layer);
				}
			}
			it++;
		}
		iter2++;
	}
}

//update close to frame if camera has moved enough
void updateCloseToFrame(SEngine * engine) {
	//calculate distance
	double dist = distance(engine->getCameraX(), lastCloseUpcateCameraLocX, engine->getCameraY(), lastCloseUpcateCameraLocY);
	//check if distance is far enugh for update
	if (dist >= sqrt(getScreenWidth() * getScreenHeight()) / 5) {
		//update close objects
		lastCloseUpcateCameraLocX = engine->getCameraX();
		lastCloseUpcateCameraLocY = engine->getCameraY();
		list<string> newNotInFrame;

		//calculate threads needed
		const int threadCap = 2;
		const int startSize = notInFrame.size();
		int threadsToUse = startSize / 100;
		if (threadsToUse > threadCap) threadsToUse = threadCap;
		else if (threadsToUse < 1) threadsToUse = 1;
		if (threadsToUse == 1) {
			checkNotInFrame(0, (int)startSize, &newNotInFrame, engine);
		}
		else {
			std::vector<std::thread> threads;
			//do tasks
			double increment = 1.0 / threadsToUse;
			double begin = 0;
			for (int i = 0; i < threadsToUse; i++) {
				threads.push_back(std::thread(checkNotInFrame, (int)startSize * begin, (int)startSize * (begin + increment), &newNotInFrame, engine));
				begin += increment;
			}
			//finish tasks 
			for (int i = 0; i < threadsToUse; i++) {
				threads.at(i).join();
			}
		}

		//check in frame
		checkInFrame(engine);
	}
}

//update moving object
void updateObjectToFrame(GameObject obj) {
	
}

//register object for possible rendering
void registerObjectToFrame(SEngine * engine, std::string n, int x, int y, int h, int w, int l) {
	if (overlapsFrame(engine, x, y, h, w)) {
		pushInFrame(n, l);
		pushInFrameUpdated(n, l, true);
	}
	else if (overlapsFrame(engine, x, y, h, w, sqrt(getScreenWidth() * getScreenHeight()) / 4)) {
		pushCloseToFrame(n);
	}
	else {
		pushNotInFrame(n);
	}
}

bool cameraMove = false;
//update close objects when moving camera
void updateCameraMovementFrame() {
	cameraMove = true;
}

void updateCameraMovementFrameFinal(SEngine * engine) {
	if (cameraMove == true) {
		list<string> newCloseToFrame;
		list<string> mapObjects = closeToFrame;
		list<string>::iterator iter = mapObjects.begin();
		while (iter != mapObjects.end()) {
			string objName = *iter;
			GameObject * obj = getGameObject(objName);
			//to frame
			if (overlapsFrame(engine, obj->getX(), obj->getY(), obj->getHeight(), obj->getWidth())) {
				pushInFrame(objName, obj->getLayer());
			}
			//away from close to frame
			else if (!overlapsFrame(engine, obj->getX(), obj->getY(), obj->getHeight(), obj->getWidth(), sqrt(getScreenWidth() * getScreenHeight()) / 4)) {
				pushNotInFrame(objName);
			}
			else {
				newCloseToFrame.push_back(objName);
			}
			iter++;
		}
		closeToFrame = newCloseToFrame;
		cameraMove = false;
	}
}

//start updating frame objects
const int objectSearchDelay = 10;
const int frameObjectUpdatesPerSec = 2;
void updateUpdatedInFrame(SEngine* engine) {
	while (engine->isGameRunning()) {
		//get start time
		auto start = std::chrono::steady_clock::now();

		//update in frame updated variable

		//remove
		map<int, list<string>> mapObjects2 = getInFrame();
		map<int, list<string>>::iterator iter2 = mapObjects2.begin();
		while (iter2 != mapObjects2.end()) {
			list<string>* objects = &(*iter2).second;
			int layer = (*iter2).first;
			list<string>::iterator it = objects->begin();
			while (it != objects->end()) {
				string objName = *it;
				if (!objName.empty()) {
					GameObject* obj = getGameObject(objName);
					if (!overlapsFrame(engine, obj->getX(), obj->getY(), obj->getHeight(), obj->getWidth())) {
						pushInFrameUpdated(objName, layer, false);
					}
				}
				it++;
			}
			iter2++;
		}

		//add missing in frame objects <------------- fix memory out of range error
		map<int, list<string>> mapObjects3 = getCopyOfInFrame();
		map<int, list<string>>::iterator iter = mapObjects3.begin();
		while (iter != mapObjects3.end()) {
			list<string> objects = (*iter).second;
			int layer = (*iter).first;
			list<string>::iterator it = objects.begin();
			while (it != objects.end()) {
				string objName = *it;
				if (!objName.empty()) {
					if (find(objects.begin(), objects.end(), objName) == objects.end()) {
						pushInFrameUpdated(objName, layer, true);
					}
				}
				it++;
			}
			iter++;
		}

		//add
		list<string> mapObjects = closeToFrame;
		list<string>::iterator it = mapObjects.begin();
		while (it != mapObjects.end()) {
			string objName = *it;
			GameObject* obj = getGameObject(objName);
			int layer = obj->getLayer();
			if (overlapsFrame(engine, obj->getX(), obj->getY(), obj->getHeight(), obj->getWidth())) {
				pushInFrameUpdated(objName, layer, true);
			}
			it++;
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
			updateCameraMovementFrameFinal(engine);
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

map<int, list<string>> getInFrame() {
	lock_guard<mutex> lock(copyInFrameUpdated);
	return inFrameUpdated;
}

void setBottomTextBox(string str) {
	textBox = str;
}

string getBottomTextBox() {
	return textBox;
}