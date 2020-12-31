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

void startCheckingKeyInput(SEngine * engine);
void startConsoleDraw(SEngine* engine);
void startPhysics(SEngine* engine);

void startCoreLoop(SEngine* engine) {
	//run core loop
	while (engine->isGameRunning()) {

		//start time
		auto start = std::chrono::steady_clock::now();

		//update events
		updateEvents(engine);
		//update camera
		engine->updateCamera();

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

//Get a console handle
HWND myconsole = GetConsoleWindow();

/*///////////////////////////////
* MANAGE TICKSPEEDS
*////////////////////////////////
int SEngine::getTickSpeed() {
	return tickSpeed;
}
int SEngine::getDrawSpeed() {
	return drawSpeed;
}
void SEngine::setTickSpeed(int amount) {
	tickSpeed = amount;
}
void SEngine::setDrawSpeed(int amount) {
	drawSpeed = amount;
}
/*///////////////////////////////
* CONSTRUCTOR, DESTRUCTOR, GAMELOOP
*////////////////////////////////
SEngine::SEngine(int maxTickSpeed, int maxDrawSpeed, int pixelScale) {
	//set up variables
	SEngine::pixelScale = pixelScale;
	running = true;
	tickSpeed = maxTickSpeed, drawSpeed = maxDrawSpeed;
	cameraX = 0, cameraY = 0, cameraFollowOffsetX = 0, cameraFollowOffsetY = 0;
	cameraFollowObject = "";
	mydc = GetDC(myconsole);
	//start game loop
	startGame();
}
SEngine::SEngine() {
	//set up variables
	SEngine::pixelScale = 10;
	running = true;
	tickSpeed = 20, drawSpeed = 120;
	cameraX = 0, cameraY = 0, cameraFollowOffsetX = 0, cameraFollowOffsetY = 0;
	cameraFollowObject = "";
	mydc = GetDC(myconsole);
	//start game loop
	startGame();
}
SEngine::~SEngine() {

}
void SEngine::startGame() {
	//iniciate keyInput thread
	keyInput = new thread(startCheckingKeyInput, this);
	//iniciate draw thread
	draw = new thread(startConsoleDraw, this);
	//iniciate physics thread
	physics = new thread(startPhysics, this);
	//iniciate find objects in frame
	frames = new thread(findObjectsInFrame, this);

	//remove scrollbar
	ShowScrollBar(myconsole, SB_BOTH, FALSE);

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
	updateCameraMovementFrame();
}
void SEngine::setCameraFollow(GameObject obj) {
	cameraFollowObject = obj.getName();
}
void SEngine::cancelCameraFollow() {
	cameraFollowObject = "";
}
bool SEngine::hasCameraObjectAttatched() {
	return cameraFollowObject.length() > 0;
}
GameObject* SEngine::getCameraFollowObject() {
	return getGameObject(cameraFollowObject);
}
void SEngine::setCameraFollowOffsetX(int amount) {
	cameraFollowOffsetX = amount;
}
void SEngine::setCameraFollowOffsetY(int amount) {
	cameraFollowOffsetY = amount;
}
void SEngine::updateCamera() {
	if (hasCameraObjectAttatched()) {
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


int main()
{
	//add scene
	Scene* scene = new Scene("start_frame", 0, 0);
	//set start scene and active
	setActiveStartScene(scene);

	//create color map
	ImageColorMap cMap;
	cMap.setColor('B', RGB(0, 0, 255));
	cMap.setColor('G', RGB(0, 255, 0));
	cMap.setColor('R', RGB(255, 0, 0));

	//test object
	GameObject* obj = new GameObject(2, 4, 1, "test");
	Image image(&cMap);
	image.addLine("GGGGGGG");
	image.addLine("  GGG  ");
	image.addLine(" GGGGG ");
	image.addLine("GGGGGGG");
	obj->updateImage(image);
	registerGameObject(obj, scene);

	//test object 2
	GameObject* obj3 = new GameObject(0, 6, 2, "line");
	Image image3(&cMap);
	image3.addLine("RRRRRRRRRRRRRRRRRRBBBBBBBBBBBBBBBBBBBB");
	obj3->updateImage(image3); 
	registerGameObject(obj3, scene);

	//performance test
	for (int i = 0; i < 1; i++) {
		stringstream stm;
		stm << "performance" << i;
		string name = stm.str();
		GameObject * obj2 = new GameObject(100, 8, 3, name);
		Image image2(&cMap);
		image2.addLine("RRRR  RRR RRRRRRRRRRRRRRRRRRRR"); 
		image2.addLine("RRRR  RRR RRRRRRRRRRRRRRRRRRRR");
		image2.addLine("RRRR  RRR RRRRRRRRRRRRRRRRRRRR");
		image2.addLine("RRRR  RRR RRRRRRRRRRRRRRRRRRRR");
		image2.addLine("RRRR  RRR RRRRRRRRRRRRRRRRRRRR");
		image2.addLine("RRRR  RRR RRRRRRRRRRRRRRRRRRRR");
		image2.addLine("RRRR  RRR RRRRRRRRRRRRRRRRRRRR");
		image2.addLine("RRRR  RRR RRRRRRRRRRRRRRRRRRRR");
		image2.addLine("RRRR  RRR RRRRRRRRRRRRRRRRRRRR");
		obj2->updateImage(image2);
		registerGameObject(obj2, scene);
	}

	//start engine
	SEngine* engine = new SEngine();
}