#include <iostream>
#include <thread>
#include <map>
#include <Windows.h>
#include <string>
#include <chrono>
#include <sstream>

#include "SEngine.h"
#include "Utils.h"


using namespace std;

void startCheckingKeyInput();
void startConsoleDraw();

//iniciate keyInput thread
thread keyInput(startCheckingKeyInput);
//iniciate draw thread
thread draw(startConsoleDraw); 
//iniciate physics thread
thread physics(startPhysics);
//iniciate find objects in frame
thread frames(findObjectsInFrame);

//variables
bool running = true;
const int tickSpeed = 20;
const int drawSpeed = 60;
int cameraX = 0;
int cameraY = 0;
string cameraFollowObject = "";
int cameraFollowXOffset = 0;
int cameraFollowYOffset = 0;

//get camera location
int getCameraX() {
	return cameraX;
}
int getCameraY() {
	return cameraY;
}
void teleportCamera(int x, int y) {
	cameraX = x;
	cameraY = y;
	updateCameraMovementFrame();
}
//camera follow
void setCameraOffsetX(int amount) {
	cameraFollowXOffset = amount;
}
void setCameraOffsetY(int amount) {
	cameraFollowYOffset = amount;
}
void setCameraFollow(GameObject obj) {
	cameraFollowObject = obj.getName();
}
void cancelCameraFollow() {
	cameraFollowObject = "";
}
bool hasCameraObjectAttatched() {
	return cameraFollowObject.length() > 0;
}
GameObject getCameraFollowObject() {
	return getGameObject(cameraFollowObject);
}
//update camera follow 
void updateCamera() {
	if (hasCameraObjectAttatched()) {
		GameObject obj = getCameraFollowObject();
		int x = obj.getX();
		int y = obj.getY();
		cameraX = x + cameraFollowXOffset;
		cameraY = y + cameraFollowYOffset;
	}
}

//return game run state
bool isGameRunning() {
	return running;
}

//shutdown
void shutdown() {
	running = false;
	keyInput.detach();
	physics.join();
	frames.join();
	draw.join();
	system("CLS");
	LOG("System closed!");
}

//core game loop
int main()
{
	//add scene
	Scene * scene = new Scene("start_frame", 0, 0);

	//set start scene and active
	setActiveStartScene(scene);

	//test object
	GameObject* obj = new GameObject(2, 4, 1, "test");
	Image image;
	image.addLine("GGGGGGG");
	image.addLine("  GGG  ");
	image.addLine(" GGGGG ");
	image.addLine("GGGGGGG");
	obj->updateImage(image);
	registerGameObject(obj, scene);

	//test object 2
	GameObject* obj3 = new GameObject(0, 6, 2, "line");
	Image image3;
	image3.addLine("BBBBBBBBBBBBBBBBBBRRRRRRRRRRRRRRRRRRRR");
	obj3->updateImage(image3); 
	registerGameObject(obj3, scene);

	//performance test
	for (int i = 0; i < 10000; i++) {
		stringstream stm;
		stm << "performance" << i;
		string name = stm.str();
		GameObject * obj2 = new GameObject(100, 8, 3, name);
		Image image2;
		image2.addLine("RRRR  R");
		obj2->updateImage(image2);
		registerGameObject(obj2, scene);
	}

	//activate start scene
	setActiveScene(getActiveStartScene());

	//run core loop
	while (running) {

		//start time
		auto start = std::chrono::steady_clock::now();

		//update events
		updateEvents();
		//update camera
		updateCamera();

		//get end time
		auto end = std::chrono::steady_clock::now();

		//get time differance
		double elapsedTime = double(std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count());

		double elapsedTimeDiff = 1 - ((elapsedTime > 1000.0 ? 1000.0 : elapsedTime) / 1000.0);
		int finalSleepTime = (1000 / tickSpeed);
		finalSleepTime = int(((double)finalSleepTime) * elapsedTimeDiff);
		Sleep(finalSleepTime);
	}
}

//frame drawing loop
void startConsoleDraw() {
	//run core loop
	while (running) {

		//start time
		auto start = std::chrono::steady_clock::now();

		//create and draw frame
		drawFrame();

		//get end time
		auto end = std::chrono::steady_clock::now();

		
		//get time differance
		double elapsedTime = double(std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count());

		//double elapsedTimeDiff = 1 - ((elapsedTime > 1000.0 ? 1000.0 : elapsedTime) / 1000.0);
		//int finalSleepTime = (1000 / drawSpeed);
		//finalSleepTime = int(((double)finalSleepTime) * elapsedTimeDiff);
		//Sleep(finalSleepTime);
		
		//Sleep(1000);

		stringstream stm;
		stm << "FPS " << (int) (1000 / elapsedTime) << " : Load Time " << elapsedTime << endl;
		setBottomTextBox(stm.str());
	}
}
