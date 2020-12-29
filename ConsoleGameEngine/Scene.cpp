#include <iostream>
#include <map>
#include <deque>
#include <exception>

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

	void setActiveScene(Scene * scene) {
		activeScene = scene;
		updateFrameObjects();
	}

	void setActiveScene(string name) {
		activeScene = getScene(name);
		updateFrameObjects();
	}

	Scene * getActiveScene() {
		return activeScene;
	}

	void setActiveStartScene(Scene* scene) {
		startScene = scene;
	}

	void setActiveStartScene(string name) {
		startScene = getScene(name);
		updateFrameObjects();
	}

	Scene* getActiveStartScene() {
		return startScene;
	}

	/*//////////////////////////////
	*SCENE CLASS
	*///////////////////////////////

	//on removal
	Scene::~Scene() {
		delete this;
	}
	//constructor
	Scene::Scene() { 
		n = "";
		CameraStartX = 0;
		CameraStartY = 0;
		scenes[n] = this;
	}
	Scene::Scene(string name) {
		n = name;
		CameraStartX = 0;
		CameraStartY = 0;
		scenes[n] = this;
	}
	Scene::Scene(string name, int startX, int startY) {
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
	list<string> Scene::getObjectNames() {
		return objects;
	}
	void Scene::addGameObject(GameObject obj) {
		objects.push_back(obj.getName());
	}
	void Scene::removeGameObject(string name) {
		objects.remove(name);
	}
	void Scene::setName(string name) {
		n = name;
	}
	string Scene::getName() {
		return n;
	}