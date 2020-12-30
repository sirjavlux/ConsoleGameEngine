#include <iostream>
#include <map>
#include <deque>
#include <list>
#include <exception>
#include <algorithm>

#include "SEngine.h"

using namespace std;

	/*//////////////////////////////
	*REGULAR FUNCTIONS
	*///////////////////////////////
	
	map<string, Scene*> scenes;
	Scene * activeScene;
	Scene * startScene;

	Scene * getScene(string name) {
		return scenes.at(name);
	}

	void removeScene(string name) {
		scenes.erase(name);
	}

	void setActiveScene(SEngine * engine, Scene * scene) {
		activeScene = scene;
		updateFrameObjects(engine);
	}

	void setActiveScene(SEngine* engine, string name) {
		activeScene = getScene(name);
		updateFrameObjects(engine);
	}

	Scene * getActiveScene() {
		return activeScene;
	}

	void setActiveStartScene(Scene* scene) {
		startScene = scene;
	}

	void setActiveStartScene(SEngine* engine, string name) {
		startScene = getScene(name);
		updateFrameObjects(engine);
	}

	Scene* getActiveStartScene() {
		return startScene;
	}

	/*//////////////////////////////
	*SCENE CLASS
	*///////////////////////////////

	//on removal
	Scene::~Scene() {
		objects = new vector<string>();
		delete this;
	}
	//constructor
	Scene::Scene() { 
		objects = new vector<string>();
		n = "";
		CameraStartX = 0;
		CameraStartY = 0;
		scenes[n] = this;
	}
	Scene::Scene(string name) {
		objects = new vector<string>();
		n = name;
		CameraStartX = 0;
		CameraStartY = 0;
		scenes[n] = this;
	}
	Scene::Scene(string name, int startX, int startY) {
		objects = new vector<string>();
		n = name;
		CameraStartX = startX;
		CameraStartY = startY;
		scenes[n] = this;
	}

	//getters and setters
	pair<int, int> Scene::getStartLocation() {
		return pair<int, int> (CameraStartX, CameraStartY);
	}
	void Scene::setStartX(int x) {
		CameraStartX = x;
	}
	void Scene::setStartY(int y) {
		CameraStartY = y;
	}
	vector<string> *& Scene::getObjectNames() {
		return objects;
	}
	void Scene::addGameObject(GameObject obj) {
		objects->push_back(obj.getName());
	}
	void Scene::removeGameObject(string name) {
		objects->erase(remove(objects->begin(), objects->end(), name));
	}
	void Scene::setName(string name) {
		n = name;
	}
	string Scene::getName() {
		return n;
	}