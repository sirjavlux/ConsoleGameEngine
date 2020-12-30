#include <iostream>
#include <map>
#include <deque>

#include "SEngine.h"
#include "Utils.h"

using namespace std;

/*//////////////////////////////////////////////////////////////
* GAMEOBJECTS NEEDS TO BE ALLOCATED IN HEAP MEMORY, USING NEW
*///////////////////////////////////////////////////////////////

std::map<std::string, GameObject*> * objectMap = new std::map<std::string, GameObject*>();

GameObject * getGameObject(string name) {
	return objectMap->at(name);
}

void removeGameObject(GameObject * obj) {
	objectMap->erase(obj->getName());
}

void registerGameObject(GameObject* obj, Scene* scene) {
	string n = obj->getName();
	int x = obj->getX();
	int y = obj->getY();
	int h = obj->getHeight();
	int w = obj->getWidth();

	objectMap->insert(pair<string, GameObject *> (n, obj));
	scene->addGameObject(*obj);
	registerObjectToFrame(n, x, y, h, w);
	
}

//GameObject constructor
GameObject::GameObject(int xLoc, int yLoc, int layer, string n, Image newImage) {
	x = xLoc;
	y = yLoc;
	l = layer;
	name = n;
	updateImage(newImage);
}
GameObject::GameObject(int xLoc, int yLoc, int layer, string n) {
	x = xLoc;
	y = yLoc;
	l = layer;
	h = 0;
	w = 0;
	name = n;
}
GameObject::GameObject(string n) {
	x = 0;
	y = 0;
	l = 0;
	h = 0;
	w = 0;
	name = n;
}
GameObject::GameObject() {
	x = 0;
	y = 0;
	l = 0;
	h = 0;
	w = 0;
	name = "";
}
//on removal
GameObject::~GameObject() {

}
//teleport
void GameObject::teleport(int xLoc, int yLoc) {
	x = xLoc;
	y = yLoc;
}
//get name
string GameObject::getName() {
	return name;
}
//get location
int GameObject::getX() {
	return x;
}
int GameObject::getY() {
	return y;
}
//get layer
int GameObject::getLayer() {
	return l;
}
//height/width
int GameObject::getHeight() {
	return h;
}
int GameObject::getWidth() {
	return w;
}
//update image
void GameObject::updateImage(Image newImage) {
	image = newImage;
	h = newImage.calcHeight();
	w = newImage.calcWidth();
}
//get image
Image * GameObject::getImage() {
	return &image;
}