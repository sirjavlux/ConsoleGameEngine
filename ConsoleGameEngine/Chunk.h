#ifndef _CHUNK_H
#define _CHUNK_H

#include <map>
#include <Windows.h>
#include <list>

#include "DrawPixel.h"
#include "SEngine.h"

/*///////////////////////////
* Chunk
*////////////////////////////

class Chunk {
private:
	std::list<GameObject*>* objects;
	std::map<std::pair<int, int>, DrawPixel*>* pixels;
	byte* colors;
	std::pair<int, int>* loc;
	bool colorsEnabled;
	void updateColors();
public:
	Chunk(std::pair<int, int> location);
	~Chunk();
	DrawPixel* getPixel(int x, int y);
	byte* getPixelColors();
	std::pair<int, int>* getLocation();
	void addGameObject(GameObject* obj, SEngine* engine);
	void removeGameObject(GameObject* obj);
	std::list<GameObject*>* getGameObjects();
	void updateGameObjectPixels(GameObject* obj, SEngine* engine);
	void setPixel(int x, int y, DrawPixel* pixel);
	void setColor(int x, int y, COLORREF* color);
	void activateColorMap();
	void dissableColorMap();
};

#endif