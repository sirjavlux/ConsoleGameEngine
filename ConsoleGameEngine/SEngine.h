#ifndef _LOG_H
#define _LOG_H

#include <map>
#include <list>
#include <iostream>
#include <vector>
#include <deque>
#include <exception>
#include <Windows.h>
#include <thread>

#include "Utils.h"

/*///////////////////////////
* PIXEL
*////////////////////////////

class Pixel
{
private:
	COLORREF COLOR;
	int xOffset;
	int l;
public:
	Pixel(COLORREF color, int lenght, int offset);
	Pixel(int lenght, int offset);
	Pixel();
	void setLenght(int lenght);
	int getLenght();
	void setOffset(int offset);
	int getOffset();
	void setColor(COLORREF color);
	COLORREF getColor();
};

/*///////////////////////////
* IMAGE
*////////////////////////////

class Image
{
private:
	std::vector<std::vector<Pixel*>*> * image;
	ImageColorMap* cMap;
public:
	Image(ImageColorMap * colorMap);
	Image();
	void addLine(std::string line);
	std::vector<std::vector<Pixel*>*> *& getVector();
	int calcWidth();
	int calcHeight();
};

/*///////////////////////////
* GAME OBJECT
*////////////////////////////

class GameObject
{
private:
	int x, y, l, h, w;
	std::string name;
	Image image;
public:
	GameObject(int xLoc, int yLoc, int layer, std::string n, Image image);
	GameObject(int xLoc, int yLoc, int layer, std::string n);
	GameObject(std::string n);
	GameObject();
	~GameObject();
	void teleport(int xLoc, int yLoc);
	std::string getName();
	int getX();
	int getY();
	int getLayer();
	int getHeight();
	int getWidth();
	void updateImage(Image image);
	Image * getImage();
};

void removeGameObject(GameObject * obj);
GameObject * getGameObject(std::string name);
GameObject * getUnsecureGameObject(std::string name);

/*///////////////////////////
* MAIN ENGINE FUNCTIONS
*////////////////////////////

class SEngine {
private:
	std::thread* keyInput;
	std::thread* frames;
	std::thread* draw;
	std::thread* physics;
	bool running;
	int tickSpeed;
	int cameraX, cameraY, cameraFollowOffsetX, cameraFollowOffsetY;
	int pixelScale;
	std::string cameraFollowObject;
	std::string name;
	HDC mydc;
	bool started;
public:
	void startGame();
	void updateCamera();
	void setName(std::string name);
	SEngine();
	~SEngine();
	std::string getName();
	int getTickSpeed();
	void setTickSpeed(int amount);
	int getPixelScale();
	void setPixelScale(int scale);
	int getCameraX();
	int getCameraY();
	void teleportCamera(int x, int y);
	void setCameraFollow(GameObject * obj);
	void cancelCameraFollow();
	bool hasCameraObjectAttatched();
	GameObject* getCameraFollowObject();
	void setCameraFollowOffsetX(int amount);
	void setCameraFollowOffsetY(int amount);
	bool isGameRunning();
	HDC getDC();
	HWND getWindow();
	void shutdown();
};

/*///////////////////////////
* SCENE CLASS AND FUNCTIONS
*////////////////////////////

class Scene {
private:
	std::vector<std::string> * objects;
	int CameraStartX, CameraStartY;
	std::string n;
public:
	~Scene();
	Scene();
	Scene(std::string name);
	Scene(std::string name, int startX, int startY);
	std::pair<int, int> getStartLocation();
	void setStartX(int x);
	void setStartY(int y);
	std::vector<std::string> *& getObjectNames();
	void addGameObject(GameObject obj);
	void removeGameObject(std::string name);
	void setName(std::string name);
	std::string getName();
};

Scene* getScene(std::string name);
void removeScene(std::string name);
void setActiveScene(SEngine* engine, Scene* scene);
void setActiveScene(SEngine* engine, std::string name);
Scene* getActiveScene();
void setActiveStartScene(Scene* scene);
void setActiveStartScene(SEngine* engine, std::string name);
Scene* getActiveStartScene();

void registerGameObject(GameObject* obj, Scene* scene); // GameObject function

/*///////////////////////////
* KEY PRESS FUNCTIONS
*////////////////////////////

int* getLastKeyPressed();

/*///////////////////////////
* FRAME FUNCTIONS
*////////////////////////////

const std::string borderStr = "\033[47m \033[0m";
const char emptyPixel = ' ';

void drawFrame(SEngine * engine);
void setBottomTextBox(std::string str);
std::string getBottomTextBox();
std::map<int, std::list<GameObject*>> getInFrame();
void registerObjectToFrame(SEngine* engine, GameObject* obj);
void updateObjectToFrame(GameObject obj);
void updateCameraMovementFrame();
void findObjectsInFrame(SEngine* engine);
void updateFrameObjects(SEngine * engine);

/*///////////////////////////
* EVENT
*////////////////////////////

void updateEvents(SEngine* engine);

/*///////////////////////////
* PHYSICS
*////////////////////////////

class Vector2D {
private:
	int x, y;
public:
	Vector2D();
	double getX();
	double getY();
	double getLenght();
	void setX(double newX);
	void setY(double newY);
	void multiply(double amount);
	void normalize();
};

class ObjectPhysics2D {
private:
	Vector2D vector2d;
public:
	ObjectPhysics2D();
	void addForce(Vector2D velocity);
	Vector2D * getVelocity();
};

class Colider2D {
private:
public:
};

/*///////////////////////////
* EXCEPTIONS
*////////////////////////////

#endif