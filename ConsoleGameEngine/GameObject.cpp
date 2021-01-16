#include "SEngine.h"

/*//////////////////////////////////////////////////////////////
* GAMEOBJECTS NEEDS TO BE ALLOCATED IN HEAP MEMORY, USING NEW
*///////////////////////////////////////////////////////////////

std::map<std::string, GameObject*> * objectMap = new std::map<std::string, GameObject*>();

std::mutex getObjectMutex;
GameObject * getGameObject(std::string name) {
	std::lock_guard<std::mutex> lock(getObjectMutex);
	std::map<std::string, GameObject*>::iterator iter = objectMap->find(name);
	if (iter != objectMap->end()) return objectMap->at(name);
	else return nullptr;
}

GameObject * getUnsecureGameObject(std::string name) {
	std::lock_guard<std::mutex> lock(getObjectMutex);
	return objectMap->find(name) == objectMap->end() ? &GameObject() : objectMap->at(name);
}

void removeGameObject(GameObject * obj) {
	std::lock_guard<std::mutex> lock(getObjectMutex);
	objectMap->erase(obj->getName());
}

void registerGameObject(GameObject* obj, Scene* scene) {
	std::lock_guard<std::mutex> lock(getObjectMutex);
	std::string n = obj->getName();
	int x = obj->getX();
	int y = obj->getY();
	int h = obj->getHeight();
	int w = obj->getWidth();

	objectMap->insert(std::pair<std::string, GameObject *> (n, obj));
	scene->addGameObject(*obj);
}

//GameObject constructor
GameObject::GameObject(int xLoc, int yLoc, int layer, std::string n, Image * newImage, int scale) {
	chunks = new std::list<Chunk*>();
	x = xLoc * scale;
	y = yLoc * scale;
	l = layer;
	name = n;
	degrees = 0;
	maxVelocity = 0;
	updateImage(newImage);
}
GameObject::GameObject(int xLoc, int yLoc, int layer, std::string n, int scale) {
	chunks = new std::list<Chunk*>();
	x = xLoc * scale;
	y = yLoc * scale;
	l = layer;
	h = 0;
	w = 0;
	degrees = 0;
	maxVelocity = 0;
	name = n;
}
GameObject::GameObject(std::string n) {
	chunks = new std::list<Chunk*>();
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
	chunks = new std::list<Chunk*>();
	x = 0;
	y = 0;
	l = 0;
	h = 0;
	w = 0;
	degrees = 0;
	maxVelocity = 0;
	name = "";
}
//update object chunks
void GameObject::updateObjectChunks(SEngine * engine) {
	double scale = (double)engine->getPixelScale();

	int fromX = (int)round((x / 100.0)) - 1;
	int toX = fromX + (int)ceil((w * scale / 100.0)) + 1;

	int fromY = (int)round((y / 100.0)) - 1;
	int toY = fromY + (int)ceil((h * scale / 100.0)) + 1;

	std::list<Chunk*> tempChunks = *chunks;
	std::list<Chunk*>::iterator iter = tempChunks.begin();

	//remove out of region chunks
	while (iter != tempChunks.end()) {
		//chunk data
		Chunk* chunk = *iter;
		std::pair<int, int>* loc = chunk->getLocation();
		int xC = loc->second;
		int yC = loc->first;

		//remove objects
		if (xC < fromX || xC > toX || yC < fromY || yC > toY) {
			chunk->removeGameObject(this);
		}
		iter++;
	}

	//add chunks
	registerObjectToFrame(engine, this);
}
//add chunk addresses if not existing
void GameObject::addChunkAddress(Chunk* chunk) {
	if (std::find(chunks->begin(), chunks->end(), chunk) == chunks->end()) chunks->push_back(chunk);
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
std::mutex locationMutex;
void GameObject::teleport(int xLoc, int yLoc) {
	std::lock_guard<std::mutex> lock(locationMutex);
	x = xLoc;
	y = yLoc;
}
//get name
std::string GameObject::getName() {
	return name;
}
//get location
int GameObject::getX() {
	std::lock_guard<std::mutex> lock(locationMutex);
	return x;
}
int GameObject::getY() {
	std::lock_guard<std::mutex> lock(locationMutex);
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
std::mutex addForceMutex;
void GameObject::addForce(Vector2D vec) {
	//thread lock
	std::lock_guard<std::mutex> lock(addForceMutex);

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
	safelySetAddForceVector(this, &vec);
}
//get Velocity 
Vector2D GameObject::getVelocity() {
	Vector2D* oldVec = safelyGetForce(this);
	if (oldVec != nullptr) return *oldVec;
	else return Vector2D();
}
void GameObject::setMaxVelocity(double amount) {
	maxVelocity = amount;
}
double GameObject::getMaxVelocity() {
	return maxVelocity;
}