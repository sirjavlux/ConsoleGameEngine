#define _WIN32_WINNT 0x0500
#define _USE_MATH_DEFINES
#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <cmath>
#include <deque>
#include <list>
#include <Windows.h>
#include <thread>
#include <wingdi.h>
#include <cstdint>
#include <mutex>
#include <math.h>

#include "SEngine.h"
#include "Utils.h"
#include "DefinedImageColors.h"

BITMAPINFOHEADER bmih;
BITMAPINFO bmpi;
int oldSize = getScreenWidth() * getScreenHeight() * 3;
byte* data = new byte[oldSize]{};

//update frame size data
void updateFrameDrawSize(int width, int height) {
	int size = width * height * 3;
	bmih.biWidth = width;
	bmih.biHeight = height;
	bmih.biSizeImage = size;
	delete[] data;
	data = new byte[size]{};
	oldSize = size;
}

//reset old data
void resetPixelData(int size) {
	memset(data, 0, size * sizeof(*data));
}

//draw pixels
void drawPixelsToScreen(SEngine * engine, int width, int height) {
	bmpi.bmiHeader = bmih;
	SetDIBitsToDevice(engine->getDC(), 0, 0, width, height, 0, 0, 0, height, data, &bmpi, DIB_RGB_COLORS);
}

//set pixel at pixel location
std::mutex pixelMutex;
byte* getColorValues(byte*& byteImage, const int colorLoc) {
	//std::lock_guard<std::mutex> lock(pixelMutex);
	byte color[3] = {
	byteImage[colorLoc],
	byteImage[colorLoc + 1],
	byteImage[colorLoc + 2] 
	};

	return color;
}

void setRotatedPixelAtLocation(
	const int colorLoc, byte*& byteImage, const int scale, const double rotation,
	const double xPixLoc, const double yPixLoc, const double rotationOriginX, const double rotationOriginY,
	const int row, const int cameraX, const int cameraY, const int frameSize, const double cs, const double ss) {

	byte* color = getColorValues(byteImage, colorLoc);
	byte b = color[0];
	byte g = color[1];
	byte r = color[2];

	if (b == 0 && g == 0 && r == 0) return;

	//calculate location
	const double rotationY = (xPixLoc - rotationOriginX) * cs + (yPixLoc - rotationOriginY) * ss;
	const double rotationX = (xPixLoc - rotationOriginX) * ss - (yPixLoc - rotationOriginY) * cs;
	int yLoc = ((int)rotationOriginY - cameraY) * row;
	int xLoc = ((int)rotationOriginX - cameraX) * 3;

	// place pixel
	for (int i3 = 0; i3 < scale; i3++) {
		for (int i4 = 0; i4 < scale; i4++) {
			const int rowLoc = yLoc + ((int)round(i4 * cs + i3 * ss + rotationY)) * row;
			const int colLoc = xLoc + ((int)round(i4 * ss - i3 * cs + rotationX)) * 3;

			if (colLoc + 3 > row || colLoc < 0) continue;
			if (rowLoc < 0) continue;

			int frameLoc = rowLoc + colLoc;
			
			if (frameLoc < 0 || frameLoc + 3 > frameSize) continue;
			data[frameLoc] = b; // blue
			data[frameLoc + 1] = g; // green
			data[frameLoc + 2] = r; // red

			if (frameLoc + 6 > frameSize) continue;
			data[frameLoc + 3] = b; // blue
			data[frameLoc + 3 + 1] = g; // green
			data[frameLoc + 3 + 2] = r; // red
		}
	}
}

void setPixelAtLocation(const int frameLocScaled, const int colorLoc, byte*& byteImage, const int scale, const int frameRow, const int frameSize) {
	
	byte* color = getColorValues(byteImage, colorLoc);
	byte b = color[0];
	byte g = color[1];
	byte r = color[2];

	if (b == 0 && g == 0 && r == 0) return;

	// place pixel
	int finalXIncrement = 0;
	int finalYIncrement = 0;
	for (int y2 = 0; y2 < scale; y2++) {
		for (int x2 = 0; x2 < scale; x2++) {
			int frameLoc = frameLocScaled + finalYIncrement + finalXIncrement;
			if (frameLoc < 0 || frameLoc + 3 > frameSize) break;
			data[frameLoc] = b; // blue
			data[frameLoc + 1] = g; // green
			data[frameLoc + 2] = r; // red
			finalXIncrement += 3;
		}
		finalXIncrement = 0;
		finalYIncrement += frameRow;
	}
}

/*/////////////////////////////
FIX Y AXIS TOP SCREEN GLITCH
*//////////////////////////////

//calculate pixels based on object images
std::list<Chunk*> safelyGetChunksInFrame();
void calculateImages(std::list<GameObject*>
	frame,
	const int width, const int height,
	const int from, const int to,
	int cameraX, int cameraY,
	SEngine* engine) {

	std::list<GameObject*>::iterator iter = frame.begin();
	std::advance(iter, from);
	int toMove = to - from;
	int count = 0;
	// frame data
	const int row = 3 * width;
	const int size = width * height * 3;
	const int scale = engine->getPixelScale();
	// start looping trough objects
	while (iter != frame.end()) {
		count++;
		if (count > toMove) break;
		else {
			// frame data 
			const int frameRow = width * 3;

			// object data
			GameObject* obj = *iter;
			iter++;
			if (obj == nullptr) continue;
			//if (obj == engine->getCameraFollowObject()) engine->updateCamera();
			byte* byteImage = obj->getImage()->getByteImage();
			if (byteImage == nullptr) continue;
			const double rotation = obj->getRotation();
			const int objWidth = obj->getWidth() * scale;
			const int objHeight = obj->getHeight() * scale;
			const int objWidthOriginal = obj->getWidth();
			const int objHeightOriginal = obj->getHeight();
			const int objXMin = obj->getX(); int objXMax = objXMin + objWidth;
			const int objYMin = obj->getY(); int objYMax = objYMin + objHeight;
			const int row = objWidth / 10 * 3;
			const int imageSize = objHeightOriginal * objWidthOriginal * 3;

			/*////////////////////
			* IF ROTATION
			*/////////////////////

			if (rotation != 0) {
				const double rotationOriginX = (double)objXMin + (double)objWidth / 2.0;
				const double rotationOriginY = (double)objYMin + (double)objHeight / 2.0;
				const double radians = (180 - rotation) * M_PI / 180;
				const double cs = cos(radians);
				const double ss = sin(radians);

				// loop trough pixels
				for (int y = 0; y < objHeightOriginal; y++) {
					for (int x = 0; x < objWidthOriginal; x++) {
						int colorLoc = (objHeightOriginal - y - 1) * row + x * 3;
						int xPixLoc = objXMin + x * scale;
						int yPixLoc = objYMin + y * scale;

						// place pixels
						setRotatedPixelAtLocation(colorLoc, byteImage, scale, rotation, xPixLoc, yPixLoc, rotationOriginX, rotationOriginY, frameRow, cameraX, cameraY, size, cs, ss);
					}
				}
			}

			/*////////////////////
			* IF NO ROTATION
			*/////////////////////

			else {
				// frame/camera data y axis
				const int offsetY = objYMin - cameraY;
				const int yImageMin = (offsetY < 0 ? offsetY * -1 : 0) / scale;
				int yImageMax = (objHeight - (objYMax - (cameraY + height) < 0 ? 0 : objYMax - (cameraY + height))) / scale;
				yImageMax += yImageMax + 1 > objHeightOriginal ? 0 : 1;

				// frame/camera data x axis
				const int offsetX = objXMin - cameraX;
				int xImageMin = (offsetX < 0 ? offsetX * -1 : 0) / scale;
				const int xImageMax = (objWidth - (objXMax - (cameraX + width) < 0 ? 0 : objXMax - (cameraX + width))) / scale;

				// x axis data
				const int startPixelLenght = ((offsetX < 0 ? offsetX * -1 : 0) - xImageMin * scale) * 3;
				const int xFrameOffset = (offsetX < 0 ? 0 : offsetX) * 3 - startPixelLenght;
				int frameXIncrement = xFrameOffset;

				// y axis data
				const int startPixelHeight = (offsetY < 0 ? offsetY * -1 : 0) - yImageMin * scale;
				const int yFrameOffset = (offsetY < 0 ? 0 : offsetY) - startPixelHeight;
				int currentFrameRow = yFrameOffset * frameRow;

				// loop trough pixels
				for (int y = yImageMin; y < yImageMax; y++) {
					for (int x = xImageMin; x < xImageMax; x++) {
						int colorLoc = (objHeightOriginal - y - 1) * row + x * 3;
						int frameLocScaled = currentFrameRow + frameXIncrement;

						// place pixels
						setPixelAtLocation(frameLocScaled, colorLoc, byteImage, scale, frameRow, size);
						frameXIncrement += 3 * scale;
					}
					frameXIncrement = xFrameOffset;
					currentFrameRow += frameRow * scale;
				}
			}
		}
	}
}

//draw frame on console
std::list<GameObject*> safelyGetGameObjectsFromLayer(int layer);
std::map<int, std::list<GameObject*>*> safelyGetGameObjects();
void drawFrame(SEngine * engine) {

	const int width = getScreenWidth();
	const int height = getScreenHeight();
	const int scale = engine->getPixelScale();

	// setup pixel drawing
	const int size = width * height * 3;
	if (size != oldSize) {
		updateFrameDrawSize(width, height);
	}
	else {
		resetPixelData(size);
	}

	const int cameraX = engine->getCameraX() - width / 2;
	const int cameraY = engine->getCameraY() - height / 2;

	std::map<int, std::list<GameObject*>*> frame = safelyGetGameObjects();
	std::map<int, std::list<GameObject*>*>::iterator iter = frame.begin();
	while (iter != frame.end()) {
		if (iter->second == nullptr) {
			iter++;
			continue;
		}
		std::list<GameObject*> objects = safelyGetGameObjectsFromLayer(iter->first);

		const int frameSize = (int)objects.size();

		// calculate thread chunks to process
		std::deque<int> indexes;
		std::list<GameObject*>::iterator objIter = objects.begin();
		int index = 0;
		int pixels = 0;
		while (objIter != objects.end()) {
			// add pixel size from object to pixel amount
			GameObject* obj = *objIter;
			pixels += obj->getHeight() * obj->getWidth();

			// add index if pixel requirements are met
			if (pixels >= 64000) {
				indexes.push_back(index);
				pixels = 0;
			}

			// increment
			index++;
			objIter++;
		}

		// add last index to indexes if not existing
		if (indexes.size() > 0) if (indexes.back() < objects.size()) indexes.push_back((int)objects.size());

		// calculate threads needed
		if (indexes.size() <= 1) {
			calculateImages(objects, width, height, 0, (int)frameSize, cameraX, cameraY, engine);
		}
		else {
			std::vector<std::thread> threads;
			std::deque<int>::iterator indexIter = indexes.begin();

			//do tasks
			int lastIndex = 0;
			while (indexIter != indexes.end()) {
				int nextIndex = *indexIter;
				threads.push_back(std::thread(calculateImages, objects, width, height, lastIndex, nextIndex, cameraX, cameraY, engine));
				lastIndex = nextIndex;
				indexIter++;
			}
			//finish tasks 
			for (int i = 0; i < threads.size(); i++) {
				threads.at(i).join();
			}
		}

		iter++;
	}

	//draw frame
	drawPixelsToScreen(engine, width, height);

	//performance benchmarking below
	//print underneth text
	std::cout << " " << getBottomTextBox();

	//fix console writing/remove flicker and reset
	ClearScreen();
}

//frame drawing loop
std::mutex * frameMutex = new std::mutex();
void updateAndDrawFrame(SEngine* engine) {
	std::lock_guard<std::mutex> lock(*frameMutex);
	//create and draw frame
	engine->updateCamera();
	drawFrame(engine);
}
std::mutex * getDrawFrameMutex() {
	return frameMutex;
}
void startConsoleDraw(SEngine* engine) {
	//setup bmih
	bmih.biBitCount = 24;
	bmih.biClrImportant = 0;
	bmih.biClrUsed = 0;
	bmih.biCompression = BI_RGB;
	bmih.biPlanes = 1;
	bmih.biSize = 40;
	updateFrameDrawSize(getScreenWidth(), getScreenHeight());
	//run core loop
	while (engine->isGameRunning() == true) {

		//start time
		auto start = std::chrono::steady_clock::now();

		//create and draw frame
		updateAndDrawFrame(engine);

		//get end time
		auto end = std::chrono::steady_clock::now();

		//get time differance
		double elapsedTime = double(std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count());

		std::stringstream stm;
		stm << "FPS " << (int)(1000 / elapsedTime) << " : Load Time " << elapsedTime << std::endl;
		setBottomTextBox(stm.str());
	}
}