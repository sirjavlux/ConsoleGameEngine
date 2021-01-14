#ifndef _LOG_H
#define _LOG_H

#include <map>
#include <list>
#include <iostream>
#include <vector>
#include <mutex>
#include <exception>
#include <Windows.h>
#include <thread>

#include "Utils.h"
#include "DrawPixel.h"

/*///////////////////////////
* PIXEL
*////////////////////////////

class Pixel
{
private:
	COLORREF COLOR;
	int xOffset;
	int l;
	int layer;
public:
	Pixel(COLORREF color, int lenght, int offset);
	Pixel(int lenght, int offset);
	Pixel();
	void setLenght(int lenght);
	int getLenght();
	void setLayer(int layer);
	int getLayer();
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
	std::vector<std::vector<DrawPixel*>*> * image;
	ImageColorMap* cMap;
public:
	Image(ImageColorMap * colorMap);
	Image();
	void addLine(std::string line);
	std::vector<std::vector<DrawPixel*>*> *& getVector();
	int calcWidth();
	int calcHeight();
};

/*///////////////////////////
* PHYSICS
*////////////////////////////

class Vector2D {
private:
	double x, y;
public:
	Vector2D(double x, double y, double multiplier);
	Vector2D(double x, double y);
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
	Vector2D* getVelocity();
};

class Colider2D {
private:
public:
};

/*///////////////////////////
* GAME OBJECT
*////////////////////////////

class GameObject
{
private:
	int x, y, l, h, w;
	double degrees, maxVelocity;
	std::string name;
	Image * image;
public:
	GameObject(int xLoc, int yLoc, int layer, std::string n, Image * image, int scale);
	GameObject(int xLoc, int yLoc, int layer, std::string n, int scale);
	GameObject(std::string n);
	GameObject();
	~GameObject();
	void teleport(int xLoc, int yLoc);
	std::string getName();
	double getRotation();
	void setRotation(double degrees);
	int getX();
	int getY();
	int getLayer();
	int getHeight();
	int getWidth();
	void updateImage(Image * image);
	Image * getImage();
	void addForce(Vector2D vec);
	Vector2D getVelocity();
	void setMaxVelocity(double amount);
	double getMaxVelocity();
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
	GameObject * cameraFollowObject;
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
	int getCameraFollowOffsetX();
	int getCameraFollowOffsetY();
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
* FRAME FUNCTIONS
*////////////////////////////

const char emptyPixel = ' ';

void drawFrame(SEngine * engine);
void setBottomTextBox(std::string str);
std::string getBottomTextBox();
void registerObjectToFrame(SEngine* engine, GameObject* obj);
void findObjectsInFrame(SEngine* engine);

/*///////////////////////////
* Key Enums
*////////////////////////////

enum class KeyState {
	down,
	up,
	idle
};

enum class Key {
	KEY_A = 0x41,
	KEY_D = 0x44,
	KEY_S = 0x53,
	KEY_W = 0x57,
	KEY_SPACE = 0x20
};

const Key keyArr[5] = { Key::KEY_A, Key::KEY_D, Key::KEY_S, Key::KEY_W, Key::KEY_SPACE };

#endif