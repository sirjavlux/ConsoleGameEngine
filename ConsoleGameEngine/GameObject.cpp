#include <iostream>
#include <map>
#include <deque>

#include "GameObject.h"
#include "Frame.h"

using namespace std;

//Object management
map<string, GameObject> objects;
list<string> objectsColection;

void registerGameObject(GameObject obj) {
	objects[obj.getName()] = obj;
	objectsColection.push_back(obj.getName());
	registerObjectToFrame(obj);
}

list<string> getObjects() {
	return objectsColection;
}

//get gameobject
GameObject getGameObject(string name) {
	return objects[name];
}

//GameObject constructor
GameObject::GameObject(int xLoc, int yLoc, int layer, string name, Image newImage) {
	x = xLoc;
	y = yLoc;
	l = layer;
	GameObject::name = name;
	updateImage(newImage);
}
GameObject::GameObject(int xLoc, int yLoc, int layer, string name) {
	x = xLoc;
	y = yLoc;
	l = layer;
	GameObject::name = name;
}
GameObject::GameObject(string name) {
	x = 0;
	y = 0;
	l = 0;
	GameObject::name = name;
}
GameObject::GameObject() {
	x = 0;
	y = 0;
	l = 0;
	GameObject::name = "";
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
//erase
void GameObject::erase() {
	objects.erase(name);
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
Image GameObject::getImage() {
	return image;
}