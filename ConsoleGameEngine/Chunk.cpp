#include <iostream>

#include "Chunk.h"

extern const int colorSize = 100 * 100 * 3;

Chunk::Chunk(std::pair<int, int> location) {
	loc = new std::pair<int, int>(location.first, location.second);
	objects = new std::list<GameObject*>();
	pixels = new std::map<std::pair<int, int>, DrawPixel*>();
	colors = nullptr;
	colorsEnabled = false;
}

Chunk::~Chunk() {
	delete loc;
	delete objects;
	delete pixels;
	if (colors != nullptr) delete[] colors;
}

void Chunk::activateColorMap() {
	if (colorsEnabled == false) {
		colors = new byte[100 * 100 * 3]();
		colorsEnabled = true;
		updateColors();
	}
}

void Chunk::dissableColorMap() {
	if (colorsEnabled == true) {
		if (colors != nullptr) delete[] colors;
		colorsEnabled = false;
	}
}

void Chunk::updateColors() {
	if (colorsEnabled == true) {
		std::map<std::pair<int, int>, DrawPixel*> safePixels = *pixels;
		std::map<std::pair<int, int>, DrawPixel*>::iterator iter = safePixels.begin();
		while (iter != safePixels.end()) {
			COLORREF* colorPtr = iter->second->getColor();
			int loc = iter->first.first * 300 + iter->first.second * 3;
			iter++;

			if (loc + 3 >= colorSize || loc <= 0) continue;
			else if (colorPtr == nullptr) continue;

			COLORREF color = *colorPtr;
			const byte red = GetRValue(color);
			const byte green = GetGValue(color);
			const byte blue = GetBValue(color);
			colors[loc] = blue; // blue
			colors[loc + 1] = green; // green
			colors[loc + 2] = red; // red
		}
	}
}

void Chunk::addGameObject(GameObject* obj, SEngine* engine) {
	objects->push_back(obj);
	updateGameObjectPixels(obj, engine);
}

void Chunk::removeGameObject(GameObject* obj) {
	objects->remove(obj);
}

std::list<GameObject*> * Chunk::getGameObjects() {
	return objects;
}

std::pair<int, int> * Chunk::getLocation() {
	return loc;
}

byte* Chunk::getPixelColors() {
	return colors;
}

DrawPixel* Chunk::getPixel(int x, int y) {
	return (*pixels)[std::pair<int, int>(y, x)];
}

void Chunk::setPixel(int x, int y, DrawPixel * pixel) {
	(*pixels)[std::pair<int, int>(y, x)] = pixel;
}

void Chunk::setColor(int x, int y, COLORREF* colorPntr) {
	int loc = y * 100 * 3 + x * 3;
	if (loc + 3 >= colorSize || loc <= 0) return;
	COLORREF color = *colorPntr;
	const byte red = GetRValue(color);
	const byte green = GetGValue(color);
	const byte blue = GetBValue(color);
	colors[loc] = blue; // blue
	colors[loc + 1] = green; // green
	colors[loc + 2] = red; // red
}

void Chunk::updateGameObjectPixels(GameObject* obj, SEngine * engine) {
	Image* image = obj->getImage();
	std::vector< std::vector<DrawPixel*> *> * pixelVec = image->getVector();
	int y = loc->first * 100;
	int x = loc->second * 100;
	const double scale = (double)engine->getPixelScale();
	// object data
	double rotation = obj->getRotation();
	const double objLocX = (double)obj->getX();
	const double objLocY = (double)obj->getY();
	const double objWidth = (double)obj->getWidth() * scale;
	const double objHeight = (double)obj->getHeight() * scale;
	int objWidthOriginal = obj->getWidth();
	int objHeightOriginal = obj->getHeight();
	int layer = obj->getLayer();

	/*//////////////////////////
	* if rotation
	*///////////////////////////

	if (rotation > 0.0) {

	}
	
	/*//////////////////////////
	* no rotation
	*///////////////////////////

	else {
		// calculate image offset from chunk
		const double offsetY = objLocY - y;
		const double offsetX = objLocX - x;

		// calculate in chunk image parts
		const int minImageY = offsetY > 0 ? 0 : offsetY * -1;
		const int minImageX = offsetX > 0 ? 0 : offsetX * -1;
		const int imageYReach = objLocY + objHeight - y;
		const int imageXReach = objLocX + objWidth - x;
		int imageYIncrement = - scale / 2; // used for getting vurrent image y loc
		int imageXIncrement = - scale / 2; // used for getting vurrent image x loc

		// calculate chunk start / end points
		const int startY = offsetY < 0 ? 0 : offsetY;
		const int startX = offsetX < 0 ? 0 : offsetX;
		const int endY = imageYReach > 100 ? 100 : imageYReach;
		const int endX = imageXReach > 100 ? 100 : imageXReach;
		
		double imageY = (int)round(minImageY / scale);
		double imageX = (int)round(minImageX / scale);
		// loop trough image
		for (int i = startY; i < endY + 5; i++) {
			int pixelImageLocY = imageY + (int)round((imageYIncrement < 0 ? 0 : imageYIncrement) / scale);
			int finalPixelImageLocY = objHeightOriginal - 1 - pixelImageLocY;
			if (pixelImageLocY >= objHeightOriginal) continue;
			else {
				for (int i2 = startX; i2 < endX; i2++) {
					int pixelImageLocX = imageX + (int)round((imageXIncrement < 0 ? 0 : imageXIncrement) / scale);
					if (pixelImageLocX >= objWidthOriginal) continue;
					else {
						DrawPixel* pixel = pixelVec->at(finalPixelImageLocY)->at(pixelImageLocX);
						imageXIncrement++;

						// set pixel to chunk
						if (pixel != nullptr) {
							DrawPixel* oldPixel = getPixel(i2, i);
							if (oldPixel != nullptr) {
								if (oldPixel->getLayer() > layer) continue;
							}
							pixel->setLayer(layer);
							setPixel(i2, i, pixel);
						}
					}
				}
				imageXIncrement = - scale / 2;
				imageYIncrement++;
			}
		}
	}
	updateColors();
}