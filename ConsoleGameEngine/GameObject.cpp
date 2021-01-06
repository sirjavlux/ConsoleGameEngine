#include <iostream>
#include <map>
#include <deque>
#include <mutex>

#include "SEngine.h"
#include "Utils.h"

using namespace std;

/*//////////////////////////////////////////////////////////////
* GAMEOBJECTS NEEDS TO BE ALLOCATED IN HEAP MEMORY, USING NEW
*///////////////////////////////////////////////////////////////

std::map<std::string, GameObject*> * objectMap = new std::map<std::string, GameObject*>();

mutex getObjectMutex;
GameObject * getGameObject(string name) {
	lock_guard<mutex> lock(getObjectMutex);
	std::map<std::string, GameObject*>::iterator iter = objectMap->find(name);
	if (iter != objectMap->end()) return objectMap->at(name);
	else return nullptr;
}

GameObject * getUnsecureGameObject(string name) {
	return objectMap->find(name) == objectMap->end() ? &GameObject() : objectMap->at(name);
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
}

//GameObject constructor
GameObject::GameObject(int xLoc, int yLoc, int layer, string n, Image * newImage, int scale) {
	x = xLoc * scale;
	y = yLoc * scale;
	l = layer;
	name = n;
	degrees = 0;
	maxVelocity = 0;
	updateImage(newImage);
}
GameObject::GameObject(int xLoc, int yLoc, int layer, string n, int scale) {
	x = xLoc * scale;
	y = yLoc * scale;
	l = layer;
	h = 0;
	w = 0;
	degrees = 0;
	maxVelocity = 0;
	name = n;
}
GameObject::GameObject(string n) {
	x = 0;
	y = 0;
	l = 0;
	h = 0;
	w = 0;
	degrees = 0;
	maxVelocity = 0;
	name = n;
}
GameObject::GameObject() {
	x = 0;
	y = 0;
	l = 0;
	h = 0;
	w = 0;
	degrees = 0;
	maxVelocity = 0;
	name = "";
}
//manage rotation
double GameObject::getRotation() {
	return degrees;
}
void GameObject::setRotation(double degrees) {
	GameObject::degrees = degrees;
}
//on removal
GameObject::~GameObject() {

}
//teleport
mutex locationMutex;
void GameObject::teleport(int xLoc, int yLoc) {
	lock_guard<mutex> lock(locationMutex);
	x = xLoc;
	y = yLoc;
}
//get name
string GameObject::getName() {
	return name;
}
//get location
int GameObject::getX() {
	lock_guard<mutex> lock(locationMutex);
	return x;
}
int GameObject::getY() {
	lock_guard<mutex> lock(locationMutex);
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
void GameObject::updateImage(Image * newImage) {
	image = newImage;
	h = newImage->calcHeight();
	w = newImage->calcWidth();
}
//get image
Image * GameObject::getImage() {
	return image;
}
//add force
Vector2D* safelyGetForce(GameObject* obj);
void safelySetAddForceVector(GameObject* obj, Vector2D* vec);
mutex addForceMutex;
void GameObject::addForce(Vector2D vec) {
	//thread lock
	lock_guard<mutex> lock(addForceMutex);

	//get old vector
	Vector2D * oldVec = safelyGetForce(this);

	//if old vector were found add old velocity to the new one
	if (oldVec != nullptr) {
		vec.setX(oldVec->getX() + vec.getX());
		vec.setY(oldVec->getY() + vec.getY());
	}

	//check if exceeds max velocity and reduce if needed
	double currVel = vec.getLenght();
	if (currVel > maxVelocity) {
		vec.normalize();
		vec.multiply(maxVelocity);
	}

	//set new vector
	safelySetAddForceVector(this, new Vector2D(vec.getX(), vec.getY()));
	delete oldVec;
}
//get Velocity 
Vector2D * GameObject::getVelocity() {
	Vector2D* oldVec = safelyGetForce(this);
	if (oldVec == nullptr) return &Vector2D();
	return oldVec;
}
void GameObject::setMaxVelocity(double amount) {
	maxVelocity = amount;
}
double GameObject::getMaxVelocity() {
	return maxVelocity;
}