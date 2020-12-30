#ifndef _LOG_H
#define _LOG_H

#include <map>
#include <list>
#include <iostream>
#include <vector>
#include <deque>
#include <exception>

/*///////////////////////////
* IMAGE
*////////////////////////////

class Image
{
private:
	std::vector<std::vector<char>> image;
public:
	Image();
	void addLine(std::string line);
	std::vector<std::vector<char>> * getVector();
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

/*///////////////////////////
* SCENE CLASS AND FUNCTIONS
*////////////////////////////

class Scene {
private:
	std::list<std::string> objects;
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
	std::list<std::string> getObjectNames();
	void addGameObject(GameObject obj);
	void removeGameObject(std::string name);
	void setName(std::string name);
	std::string getName();
};

Scene* getScene(std::string name);
void removeScene(std::string name);
void setActiveScene(Scene* scene);
void setActiveScene(std::string name);
Scene* getActiveScene();
void setActiveStartScene(Scene* scene);
void setActiveStartScene(std::string name);
Scene* getActiveStartScene();

void registerGameObject(GameObject* obj, Scene* scene); // GameObject function

/*///////////////////////////
* MAIN ENGINE FUNCTIONS
*////////////////////////////

void shutdown();
const int frameH = 48;
const int frameW = 96;
int getCameraX();
int getCameraY();
void teleportCamera(int x, int y);
void setCameraFollow(GameObject obj);
void cancelCameraFollow();
bool hasCameraObjectAttatched();
GameObject getCameraFollowObject();
void setCameraOffsetX(int amount);
void setCameraOffsetY(int amount);
bool isGameRunning();

/*///////////////////////////
* KEY PRESS FUNCTIONS
*////////////////////////////

int* getLastKeyPressed();

/*///////////////////////////
* FRAME FUNCTIONS
*////////////////////////////

const std::string borderStr = "\033[47m \033[0m";
const char emptyPixel = ' ';

void drawFrame();
void setBottomTextBox(std::string str);
std::string getBottomTextBox();
std::vector<std::string> getInFrame();
void registerObjectToFrame(std::string n, int x, int y, int h, int w);
void updateObjectToFrame(GameObject obj);
void updateCameraMovementFrame();
void findObjectsInFrame();
void convertToColored(char c, char letter, char textC, std::stringstream& stm);
void updateFrameObjects();

/*///////////////////////////
* EVENT
*////////////////////////////

void updateEvents();

/*///////////////////////////
* PHYSICS
*////////////////////////////

void startPhysics();

/*///////////////////////////
* EXCEPTIONS
*////////////////////////////

class InvalidGameObjectIdentifier : public std::exception {
public:
	const char* what() const throw ();
};

#endif