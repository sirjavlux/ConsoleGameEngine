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

#include "SEngine.h"
#include "Utils.h"

using namespace std;

//cached gameobjects
vector<string> notInFrame;
vector<string> inFrame;
vector<string> closeToFrame;
string textBox = "";

//check if gameobject overlaps frame
bool overlapsFrame(int x, int y, int h, int w) {
	//frame coordinates
	int cameraX = getCameraX() - frameW / 2;
	int cameraY = getCameraY() - frameH / 2;
	int frameX1 = cameraX; int frameX2 = cameraX + frameW;
	int frameY1 = cameraY; int frameY2 = cameraY + frameH;

	//object coordinates
	int x1 = x; int x2 = x + w;
	int y1 = y; int y2 = y + h;

	if (x2 < frameX1 || x1 > frameX2) return false;
	else if (y2 < frameY1 || y1 > frameY2) return false;
	else return true;
}

//check if gameobject overlaps frame with offset
bool overlapsFrame(int x, int y, int h, int w, int offset) {
	//frame coordinates
	int cameraX = getCameraX() - frameW / 2;
	int cameraY = getCameraY() - frameH / 2;
	int frameX1 = cameraX; int frameX2 = cameraX + frameW;
	int frameY1 = cameraY; int frameY2 = cameraY + frameH;

	//object coordinates
	int x1 = x; int x2 = x + w;
	int y1 = y; int y2 = y + h;

	if (x2 < frameX1 - offset || x1 > frameX2 + offset) return false;
	else if (y2 < frameY1 - offset || y1 > frameY2 + offset) return false;
	else return true;
}

//update scene objects
void updateFrameObjects() {
	notInFrame.clear();
	inFrame.clear();
	closeToFrame.clear();

	Scene* activeScene = getActiveScene();

	list<string> objects = activeScene->getObjectNames();
	list<string>::iterator iter = objects.begin();

	while (iter != objects.end()) {
		GameObject * obj = getGameObject(*iter);
		registerObjectToFrame(*iter, obj->getX(), obj->getY(), obj->getHeight(), obj->getWidth());
		iter++;
	}
}

int lastCloseUpcateCameraLocX = 0;
int lastCloseUpcateCameraLocY = 0;
int updateOffset = frameH / 2;

//distance 
double distance(int x2, int x1, int y2, int y1) {
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

//update close to frame if camera has moved enough
void updateCloseToFrame() {
	//calculate distance
	double dist = distance(getCameraX(), lastCloseUpcateCameraLocX, getCameraY(), lastCloseUpcateCameraLocY);
	//check if distance is far enugh for update
	if (dist >= updateOffset) {
		//update close objects
		lastCloseUpcateCameraLocX = getCameraX();
		lastCloseUpcateCameraLocY = getCameraY();
		vector<string> mapObjects = notInFrame;
		vector<string> newNotInFrame, newInFrame;
		vector<string>::iterator iter = mapObjects.begin();
		while (iter != mapObjects.end()) {
			string objName = *iter;
			if (!objName.empty()) {
				GameObject * obj = getGameObject(objName);
				if (overlapsFrame(obj->getX(), obj->getY(), obj->getHeight(), obj->getWidth())) {
					inFrame.push_back(objName);
				}
				else if (overlapsFrame(obj->getX(), obj->getY(), obj->getHeight(), obj->getWidth(), updateOffset)) {
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
		vector<string> mapObjects2 = inFrame;
		vector<string>::iterator iter2 = mapObjects2.begin();
		while (iter2 != mapObjects2.end()) {
			string objName = *iter2;
			if (!objName.empty()) {
				GameObject * obj = getGameObject(objName);
				if (!overlapsFrame(obj->getX(), obj->getY(), obj->getHeight(), obj->getWidth())) {
					closeToFrame.push_back(objName);
				}
				else {
					newInFrame.push_back(objName);
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
void registerObjectToFrame(std::string n, int x, int y, int h, int w) {
	if (overlapsFrame(x, y, h, w)) {
		inFrame.push_back(n);
	}
	else if (overlapsFrame(x, y, h, w, updateOffset)) {
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

void updateCameraMovementFrameFinal() {
	if (cameraMove == true) {
		vector<string> newCloseToFrame;
		vector<string> mapObjects = closeToFrame;
		vector<string>::iterator iter = mapObjects.begin();
		while (iter != mapObjects.end()) {
			string objName = *iter;
			GameObject * obj = getGameObject(objName);
			//to frame
			if (overlapsFrame(obj->getX(), obj->getY(), obj->getHeight(), obj->getWidth())) {
				inFrame.push_back(objName);
			}
			//away from close to frame
			else if (!overlapsFrame(obj->getX(), obj->getY(), obj->getHeight(), obj->getWidth(), updateOffset)) {
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
void findObjectsInFrame() {
	while (isGameRunning()) {
		//get start time
		auto start = std::chrono::steady_clock::now();

		//update camera caching and such
		if (getActiveScene() != nullptr) {
			updateCloseToFrame();
			updateCameraMovementFrameFinal();
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

vector<string> getInFrame() {
	return inFrame;
}

void setBottomTextBox(string str) {
	textBox = str;
}

string getBottomTextBox() {
	return textBox;
}