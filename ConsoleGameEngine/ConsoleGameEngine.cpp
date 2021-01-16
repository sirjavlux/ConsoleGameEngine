#define _WIN32_WINNT 0x0500
#include <iostream>
#include <thread>
#include <map>
#include <Windows.h>
#include <string>
#include <chrono>
#include <sstream>
#include <tchar.h>
#include <string>
#include <mutex>

#include "SEngine.h"
#include "Utils.h"

using namespace std;

void startCheckingKeyInput(SEngine * engine);
void startConsoleDraw(SEngine* engine);
void startPhysics(SEngine* engine);
void UpdateTick(SEngine* engine);

//Get a console handle
HWND myconsole = GetConsoleWindow();

void startCoreLoop(SEngine* engine) {
	//run core loop
	while (engine->isGameRunning()) {

		//remove scrollbar
		ShowScrollBar(myconsole, SB_BOTH, FALSE);

		//start time
		auto start = std::chrono::steady_clock::now();

		//update
		UpdateTick(engine);

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

/*///////////////////////////////
* MANAGE TICKSPEEDS
*////////////////////////////////
int SEngine::getTickSpeed() {
	return tickSpeed;
}
void SEngine::setTickSpeed(int amount) {
	tickSpeed = amount;
}
/*///////////////////////////////
* CONSTRUCTOR, DESTRUCTOR, GAMELOOP
*////////////////////////////////
SEngine::SEngine() {
	//set up variables
	SEngine::pixelScale = 10;
	running = true;
	tickSpeed = 20;
	cameraX = 0, cameraY = 0, cameraFollowOffsetX = 0, cameraFollowOffsetY = 0;
	cameraFollowObject = nullptr;
	mydc = GetDC(myconsole);
	SEngine::name = "ConsoleGameEngine";
	started = false;
}
void onDisable();
SEngine::~SEngine() {
	onDisable();
}
string SEngine::getName() {
	return name;
}
void SEngine::startGame() {
	if (started) {
		return;
	}
	else {
		started = true;
	}
	//set title
	SetConsoleTitleA(name.c_str());
	//iniciate keyInput thread
	keyInput = new thread(startCheckingKeyInput, this);
	//iniciate draw thread
	draw = new thread(startConsoleDraw, this);
	//iniciate physics thread
	physics = new thread(startPhysics, this);
	//iniciate find objects in frame
	frames = new thread(findObjectsInFrame, this);

	//activate startscreen
	if (getActiveStartScene() != nullptr) setActiveScene(this, getActiveStartScene());
	else shutdown();

	//start core loop
	startCoreLoop(this);
}
/*///////////////////////////////
* CAMERA
*////////////////////////////////
int SEngine::getCameraX() {
	return cameraX;
}
int SEngine::getCameraY() {
	return cameraY;
}
void SEngine::teleportCamera(int x, int y) {
	cameraX = x;
	cameraY = y;
}
void SEngine::setCameraFollow(GameObject * obj) {
	cameraFollowObject = obj;
}
void SEngine::cancelCameraFollow() {
	cameraFollowObject = nullptr;
}
bool SEngine::hasCameraObjectAttatched() {
	return cameraFollowObject != nullptr;
}
GameObject* SEngine::getCameraFollowObject() {
	return cameraFollowObject;
}
void SEngine::setCameraFollowOffsetX(int amount) {
	cameraFollowOffsetX = amount;
}
void SEngine::setCameraFollowOffsetY(int amount) {
	cameraFollowOffsetY = amount;
}
int SEngine::getCameraFollowOffsetX() {
	return cameraFollowOffsetX;
}
int SEngine::getCameraFollowOffsetY() {
	return cameraFollowOffsetY;
}
void SEngine::updateCamera() {
	if (hasCameraObjectAttatched()) {
		//update camera following object
		GameObject * obj = getCameraFollowObject();
		int x = obj->getX();
		int y = obj->getY();
		cameraX = x + cameraFollowOffsetX;
		cameraY = y + cameraFollowOffsetY;
	}
}
/*///////////////////////////////
* MANAGING
*////////////////////////////////
void SEngine::setName(std::string name) {
	SEngine::name = name;
	SetConsoleTitleA(name.c_str());
}
bool SEngine::isGameRunning() {
	return running;
}
HDC SEngine::getDC() {
	return mydc;
}
HWND SEngine::getWindow() {
	return myconsole;
}

void SEngine::shutdown() {
	running = false;
	keyInput->detach();
	physics->join();
	frames->join();
	draw->join();
	ReleaseDC(myconsole, mydc);
	system("CLS");
	LOG("System closed!");
}
int SEngine::getPixelScale() {
	return pixelScale;
}
void SEngine::setPixelScale(int scale) {
	pixelScale = scale;
}

void onEnable(SEngine* engine);

int main()
{
	//create engine 
	SEngine* engine = new SEngine();

	//run on enable function
	onEnable(engine);

	//start game loop
	engine->startGame();
}