#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <sstream>
#include <cmath>
#include <deque>
#include <Windows.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <cmath>
#include <algorithm>

#include "Chunk.h"
#include "SEngine.h"
#include "Utils.h"

//cached gameobjects
std::map<std::pair<int, int>, Chunk*> chunks;
std::list<Chunk*> chunksInFrame;
std::list<Chunk*> chunksCloseToFrame;

std::string textBox = "";

//update scene objects
void safeClear();
void updateFrameObjects(SEngine* engine) {

	safeClear();

	if (getActiveScene() != nullptr) {
		Scene* activeScene = getActiveScene();

		std::vector<std::string> objects = *activeScene->getObjectNames();

		std::vector<std::string>::iterator iter = objects.begin();

		while (iter != objects.end()) {
			GameObject* obj = getGameObject(*iter);
			registerObjectToFrame(engine, obj);
			iter++;
		}
	}
}

// register new object
void registerObjectToFrame(SEngine* engine, GameObject* obj) {
	// calculate chunks
	double scale = (double)engine->getPixelScale();
	double w = (double)(obj->getWidth()) * scale;
	double h = (double)(obj->getHeight()) * scale;
	double x = (double)obj->getX();
	double y = (double)obj->getY();

	int fromX = (int)round((x / 100.0));
	int toX = fromX + (int)ceil((w / 100.0));

	int fromY = (int)round((y / 100.0));
	int toY = fromY + (int)ceil((h / 100.0));

	for (int i = fromY - 1; i < toY + 1; i++) {
		for (int i2 = fromX - 1; i2 < toX + 1; i2++) {
			std::pair<int, int> yx(i, i2);
			if (chunks[yx] != nullptr) {
				chunks[yx]->addGameObject(obj, engine);
			}
			else {
				Chunk* chunk = new Chunk(yx);
				chunk->addGameObject(obj, engine);
				chunks[yx] = chunk;
			}
		}
	}
}

// modify / get chunks in frame safely
std::mutex chunksInFrameMutex, chunksCloseToFrameMutex, chunksMutex;

// Close to frame chunks
std::list<Chunk*> safelyGetChunksCloseToFrame() {
	std::lock_guard<std::mutex> lock(chunksCloseToFrameMutex);
	return chunksCloseToFrame;
}

void safelyAddChunkCloseToFrame(Chunk* chunk) {
	std::lock_guard<std::mutex> lock(chunksCloseToFrameMutex);
	if (std::find(chunksCloseToFrame.begin(), chunksCloseToFrame.end(), chunk) == chunksCloseToFrame.end()) chunksCloseToFrame.push_back(chunk);
}

void safelyRemoveChunkCloseToFrame(Chunk* chunk) {
	std::lock_guard<std::mutex> lock(chunksCloseToFrameMutex);
	chunksCloseToFrame.remove(chunk);
}

// Frame chunks
std::list<Chunk*> safelyGetChunksInFrame() {
	std::lock_guard<std::mutex> lock(chunksInFrameMutex);
	return chunksInFrame;
}

void safelyAddChunkToFrame(Chunk* chunk) {
	std::lock_guard<std::mutex> lock(chunksInFrameMutex);
	if (std::find(chunksInFrame.begin(), chunksInFrame.end(), chunk) == chunksInFrame.end()) chunksInFrame.push_back(chunk);
}

void safelyRemoveChunkFromFrame(Chunk* chunk) {
	std::lock_guard<std::mutex> lock(chunksInFrameMutex);
	chunksInFrame.remove(chunk);
}

// Regular chunk map
std::map<std::pair<int, int>, Chunk*> safelyGetChunks() {
	std::lock_guard<std::mutex> lock(chunksMutex);
	return chunks;
}

Chunk* safelyGetChunk(std::pair<int, int> yx) {
	std::lock_guard<std::mutex> lock(chunksMutex);
	return chunks[yx];
}

//safely clear old objects
void safeClear() {
	std::lock_guard<std::mutex> lock(chunksInFrameMutex);
	chunksInFrame.clear();

	std::lock_guard<std::mutex> lock2(chunksMutex);
	chunks.clear();
}

//update in frame chunks
void updateInFrameChunks(SEngine* engine) {
	//camera data
	int width = (int)(getScreenWidth() * 1.2);
	int height = (int)(getScreenHeight() * 1.2);
	int cameraX = engine->getCameraX() - width / 2;
	int cameraY = engine->getCameraY() - height / 2;
	int cameraXMax = cameraX + width;
	int cameraYMax = cameraY + height;
	
	//chunk load cordinates
	int fromX = (int)round(((double)cameraX) / 100.0); int toX = (int)ceil(((double)cameraXMax) / 100.0);
	int fromY = (int)round(((double)cameraY) / 100.0); int toY = (int)ceil(((double)cameraYMax) / 100.0);

	//remove chunks outside frame
	std::list<Chunk*> tempChunks = safelyGetChunksInFrame();
	std::list<Chunk*>::iterator iter = tempChunks.begin();

	while (iter != tempChunks.end()) {
		//check if chunk is in frame
		Chunk* chunk = *iter;
		std::pair<int, int> yx = *(chunk->getLocation());
		int y1 = yx.first * 100; int y2 = y1 + 100;
		int x1 = yx.second * 100; int x2 = x1 + 100;
		//remove if out of frame
		if (x1 >= cameraXMax || x2 <= cameraX) safelyRemoveChunkFromFrame(chunk);
		else if (y2 <= cameraY || y1 >= cameraYMax) safelyRemoveChunkFromFrame(chunk);
		iter++;
	}

	//add new chunks to frame
	for (int i = fromY; i < toY; i++) {
		for (int i2 = fromX; i2 < toX; i2++) {
			std::pair<int, int> yx(i, i2);
			Chunk* chunk = safelyGetChunk(yx);
			if (chunk != nullptr) {
				safelyAddChunkToFrame(chunk);
			}
		}
	}
}

//update close chunk colormaps
void updateCloseToFrameColorMaps(SEngine* engine) {
	//camera data
	int width = (int)(getScreenWidth() * 2);
	int height = (int)(getScreenHeight() * 2);
	int cameraX = engine->getCameraX() - width / 2;
	int cameraY = engine->getCameraY() - height / 2;
	int cameraXMax = cameraX + width;
	int cameraYMax = cameraY + height;

	//chunk load cordinates
	int fromX = (int)round(((double)cameraX) / 100.0); int toX = (int)ceil(((double)cameraXMax) / 100.0);
	int fromY = (int)round(((double)cameraY) / 100.0); int toY = (int)ceil(((double)cameraYMax) / 100.0);

	//remove chunks outside frame
	std::list<Chunk*> tempCloseChunks = safelyGetChunksCloseToFrame();
	std::list<Chunk*>::iterator iter = tempCloseChunks.begin();

	while (iter != tempCloseChunks.end()) {
		//check if chunk is in frame
		Chunk* chunk = *iter;
		std::pair<int, int> yx = *(chunk->getLocation());
		int y1 = yx.first * 100; int y2 = y1 + 100;
		int x1 = yx.second * 100; int x2 = x1 + 100;
		//remove if out of frame
		if (x1 >= cameraXMax || x2 <= cameraX) {
			safelyRemoveChunkCloseToFrame(chunk);
			chunk->dissableColorMap();
		}
		else if (y2 <= cameraY || y1 >= cameraYMax) {
			safelyRemoveChunkCloseToFrame(chunk);
			chunk->dissableColorMap();
		}
		iter++;
	}

	//add new chunks to frame
	for (int i = fromY; i < toY; i++) {
		for (int i2 = fromX; i2 < toX; i2++) {
			std::pair<int, int> yx(i, i2);
			Chunk* chunk = safelyGetChunk(yx);
			if (chunk != nullptr) {
				safelyAddChunkCloseToFrame(chunk);
				chunk->activateColorMap();
			}
		}
	}
}

//find all objects and decide wether to keep them
int objectSearchDelay = 20;
void findObjectsInFrame(SEngine* engine) {
	while (engine->isGameRunning()) {
		//get start time
		auto start = std::chrono::steady_clock::now();

		//update chunks in frame
		updateInFrameChunks(engine);
		updateCloseToFrameColorMaps(engine);

		//get end time
		auto end = std::chrono::steady_clock::now();

		//get time differance
		double elapsedTime = double(std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count());

		double elapsedTimeDiff = 1 - ((elapsedTime > 1000.0 ? 1000.0 : elapsedTime) / 1000.0);
		int finalSleepTime = (1000 / objectSearchDelay);
		finalSleepTime = int (((double)finalSleepTime) * elapsedTimeDiff);
		Sleep(finalSleepTime);
	}
}

void setBottomTextBox(std::string str) {
	textBox = str;
}

std::string getBottomTextBox() {
	return textBox;
}