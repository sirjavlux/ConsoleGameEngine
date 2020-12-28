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

#include "Frame.h"
#include "Engine.h"
#include "GameObject.h"
#include "Image.h"

using namespace std;

//cached gameobjects
deque<string> notInFrame;
deque<string> inFrame;
deque<string> closeToFrame;
string textBox = "";

//check if gameobject overlaps frame
bool overlapsFrame(GameObject obj) {
	//frame coordinates
	int cameraX = getCameraX() - frameW / 2;
	int cameraY = getCameraY() - frameH / 2;
	int frameX1 = cameraX; int frameX2 = cameraX + frameW;
	int frameY1 = cameraY; int frameY2 = cameraY + frameH;

	//object coordinates
	int x1 = obj.getX(); int x2 = obj.getX() + obj.getWidth();
	int y1 = obj.getY(); int y2 = obj.getY() + obj.getHeight();

	if (x2 < frameX1 || x1 > frameX2) return false;
	else if (y2 < frameY1 || y1 > frameY2) return false;
	else return true;
}

//check if gameobject overlaps frame with offset
bool overlapsFrame(GameObject obj, int offset) {
	//frame coordinates
	int cameraX = getCameraX() - frameW / 2;
	int cameraY = getCameraY() - frameH / 2;
	int frameX1 = cameraX; int frameX2 = cameraX + frameW;
	int frameY1 = cameraY; int frameY2 = cameraY + frameH;

	//object coordinates
	int x1 = obj.getX(); int x2 = obj.getX() + obj.getWidth();
	int y1 = obj.getY(); int y2 = obj.getY() + obj.getHeight();

	if (x2 < frameX1 - offset || x1 > frameX2 + offset) return false;
	else if (y2 < frameY1 - offset || y1 > frameY2 + offset) return false;
	else return true;
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
		deque<string> mapObjects = notInFrame;
		deque<string> newNotInFrame, newInFrame;
		deque<string>::iterator iter = mapObjects.begin();
		while (iter != mapObjects.end()) {
			string objName = *iter;
			GameObject obj = getGameObject(objName);
			if (overlapsFrame(obj)) {
				inFrame.push_back(objName);
			}
			else if (overlapsFrame(obj, updateOffset)) {
				closeToFrame.push_back(objName);
			}
			else {
				newNotInFrame.push_back(objName);
			}
			iter++;
		}
		notInFrame = newNotInFrame;
		//update frame objects
		deque<string> mapObjects2 = inFrame;
		deque<string>::iterator iter2 = mapObjects2.begin();
		while (iter2 != mapObjects2.end()) {
			string objName = *iter2;
			GameObject obj = getGameObject(objName);
			if (!overlapsFrame(obj)) {
				closeToFrame.push_back(objName);
			}
			else {
				newInFrame.push_back(objName);
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
void registerObjectToFrame(GameObject obj) {
	if (overlapsFrame(obj)) {
		inFrame.push_back(obj.getName());
	}
	else if (overlapsFrame(obj, updateOffset)) {
		closeToFrame.push_back(obj.getName());
	}
	else {
		notInFrame.push_back(obj.getName());
	}
}

bool cameraMove = false;
//update close objects when moving camera
void updateCameraMovementFrame() {
	cameraMove = true;
}

void updateCameraMovementFrameFinal() {
	if (cameraMove == true) {
		deque<string> newCloseToFrame;
		deque<string> mapObjects = closeToFrame;
		deque<string>::iterator iter = mapObjects.begin();
		while (iter != mapObjects.end()) {
			string objName = *iter;
			GameObject obj = getGameObject(objName);
			//to frame
			if (overlapsFrame(obj)) {
				inFrame.push_back(objName);
			}
			//away from close to frame
			else if (!overlapsFrame(obj, updateOffset)) {
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
		updateCloseToFrame();
		updateCameraMovementFrameFinal();

		//get end time
		auto end = std::chrono::steady_clock::now();

		//get time differance
		double elapsedTime = double(std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count());

		//stringstream stm;
		//stm << endl << "Time to load " << elapsedTime;
		//setBottomTextBox(stm.str());

		double elapsedTimeDiff = 1 - ((elapsedTime > 1000.0 ? 1000.0 : elapsedTime) / 1000.0);
		int finalSleepTime = (1000 / objectSearchDelay);
		finalSleepTime = int (((double)finalSleepTime) * elapsedTimeDiff);
		Sleep(finalSleepTime);
	}
}

deque<string> * getInFrame() {
	return &inFrame;
}

void setBottomTextBox(string str) {
	textBox = str;
}

string getBottomTextBox() {
	return textBox;
}