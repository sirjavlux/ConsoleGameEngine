#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <cmath>
#include <deque>
#include <Windows.h>
#include <thread>
#include <mutex>
#include <chrono>
#include<cmath>

#include "SEngine.h"
#include "Utils.h"

using namespace std;

//cached gameobjects
vector<string> notInFrame;
map<int, vector<string>> inFrame;
vector<string> closeToFrame;
string textBox = "";

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

//update close to frame if camera has moved enough
void updateCloseToFrame(SEngine * engine) {
	//calculate distance
	double dist = distance(engine->getCameraX(), lastCloseUpcateCameraLocX, engine->getCameraY(), lastCloseUpcateCameraLocY);
	//check if distance is far enugh for update
	if (dist >= getScreenWidth() / 4) {
		//update close objects
		lastCloseUpcateCameraLocX = engine->getCameraX();
		lastCloseUpcateCameraLocY = engine->getCameraY();
		vector<string> mapObjects = notInFrame;
		vector<string> newNotInFrame;
		map<int, vector<string>> newInFrame;
		vector<string>::iterator iter = mapObjects.begin();
		while (iter != mapObjects.end()) {
			string objName = *iter;
			if (!objName.empty()) {
				GameObject * obj = getGameObject(objName);
				if (overlapsFrame(engine, obj->getX(), obj->getY(), obj->getHeight(), obj->getWidth())) {
					inFrame[obj->getLayer()].push_back(objName);
				}
				else if (overlapsFrame(engine, obj->getX(), obj->getY(), obj->getHeight(), obj->getWidth(), sqrt(getScreenWidth() * getScreenHeight()) / 2)) {
					closeToFrame.push_back(objName);
				}
				else {
					newNotInFrame.push_back(objName);
				}
			}
			iter++;
		}
		notInFrame = newNotInFrame;
		//update frame objects
		map<int, vector<string>> mapObjects2 = inFrame;
		map<int, vector<string>>::iterator iter2 = mapObjects2.begin();
		while (iter2 != mapObjects2.end()) {
			vector<string> objects = (*iter2).second;
			for (int i = 0; i < objects.size(); i++) {
				string objName = objects.at(i);
				if (!objName.empty()) {
					GameObject* obj = getGameObject(objName);
					if (!overlapsFrame(engine, obj->getX(), obj->getY(), obj->getHeight(), obj->getWidth())) {
						closeToFrame.push_back(objName);
					}
					else {
						newInFrame[obj->getLayer()].push_back(objName);
					}
				}
			}
			iter2++;
		}
		inFrame = newInFrame;
	}
}

//update moving object
void updateObjectToFrame(GameObject obj) {

}

//register object for possible rendering
void registerObjectToFrame(SEngine * engine, std::string n, int x, int y, int h, int w, int l) {
	if (overlapsFrame(engine, x, y, h, w)) {
		inFrame[l].push_back(n);
	}
	else if (overlapsFrame(engine, x, y, h, w, sqrt(getScreenWidth() * getScreenHeight()) / 2)) {
		closeToFrame.push_back(n);
	}
	else {
		notInFrame.push_back(n);
	}
}

bool cameraMove = false;
//update close objects when moving camera
void updateCameraMovementFrame() {
	cameraMove = true;
}

void updateCameraMovementFrameFinal(SEngine * engine) {
	if (cameraMove == true) {
		vector<string> newCloseToFrame;
		vector<string> mapObjects = closeToFrame;
		vector<string>::iterator iter = mapObjects.begin();
		while (iter != mapObjects.end()) {
			string objName = *iter;
			GameObject * obj = getGameObject(objName);
			//to frame
			if (overlapsFrame(engine, obj->getX(), obj->getY(), obj->getHeight(), obj->getWidth())) {
				inFrame[obj->getLayer()].push_back(objName);
			}
			//away from close to frame
			else if (!overlapsFrame(engine, obj->getX(), obj->getY(), obj->getHeight(), obj->getWidth(), sqrt(getScreenWidth() * getScreenHeight()) / 2)) {
				notInFrame.push_back(objName);
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

//find all objects and decide wether to keep them
const int objectSearchDelay = 10;
void findObjectsInFrame(SEngine* engine) {
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
}

map<int, vector<string>> * getInFrame() {
	return &inFrame;
}

void setBottomTextBox(string str) {
	textBox = str;
}

string getBottomTextBox() {
	return textBox;
}