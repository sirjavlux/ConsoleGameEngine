#include "SEngine.h"

Chunk::Chunk(std::pair<int, int> location) {
	loc = new std::pair<int, int>(location.first, location.second);
	objects = new std::map<int, std::list<GameObject*>*>();
}

Chunk::~Chunk() {
	delete loc;
	delete objects;
}

void Chunk::addGameObject(GameObject* obj, SEngine* engine) {
	std::list<GameObject*>* list = (*objects)[obj->getLayer()];
	if (list == nullptr) {
		list = new std::list<GameObject*>();
		list->push_back(obj);
		(*objects)[obj->getLayer()] = list;
	}
	else if (std::find(list->begin(), list->end(), obj) == list->end()) list->push_back(obj);
	obj->addChunkAddress(this);
}

void Chunk::removeGameObject(GameObject* obj) {
	std::list<GameObject*>* list = (*objects)[obj->getLayer()];
	if (list != nullptr) list->remove(obj);
}

std::map<int, std::list<GameObject*>*> Chunk::getGameObjects() {
	return *objects;
}

std::pair<int, int> * Chunk::getLocation() {
	return loc;
}