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

//set to image map
void setPixel(int xPixLoc, int yPixLoc, int cameraX, int cameraY, int width, int height, int scale, int lenght, COLORREF color, int size, int maxX, const double rotation, const double rotationOriginX, const double rotationOriginY, const int yLoc, const int xLoc) {
	
	const byte red = GetRValue(color);
	const byte green = GetGValue(color);
	const byte blue = GetBValue(color);

	/*/////////////////////////////////
	* Without object being rotated
	*//////////////////////////////////

	if (rotation == 0) {
		const int row = 3 * width;
		const int startPrintX = xPixLoc;
		int printLoc = (yPixLoc - cameraY) * row + (xPixLoc - cameraX) * 3; // calculate start print location
		//calculate y axis render locations 
		const int startRow = printLoc / row;
		int rowsLeft = height - 1 - startRow;
		int minY = scale > rowsLeft ? scale - rowsLeft : 0;
		int maxY = startRow < 0 ? scale - startRow : scale;
		//calculate print lenght considering edge of screen
		int pixelLenght = scale * lenght;
		const int screenEnd = maxX - startPrintX;
		if (pixelLenght > screenEnd) pixelLenght = screenEnd;
		//calculate start index and loc based on screen location
		const int startOffset = cameraX - startPrintX;
		int startLoc = 0;
		int startIndex = 0;
		if (startOffset > 0) {
			startLoc = startOffset;
			startIndex = startOffset * 3;
		}
		//start rendering
		int index = startIndex;
		for (int i3 = minY; i3 < maxY; i3++) {
			for (int i4 = startLoc; i4 < pixelLenght; i4++) {
				int loc = printLoc + index; // calculate render loc

				data[loc] = blue; // blue
				data[loc + 1] = green; // green
				data[loc + 2] = red; // red

				index += 3; // increase to next pixel index
			}
			printLoc += row; // increase to next row
			index = startIndex;
		}
	}

	/*/////////////////////////////////
	* When rotated
	*//////////////////////////////////

	else {
		//variables
		const int row = 3 * width;
		//calculate location
		const double radians = (180 - rotation) * M_PI / 180;
		const double cs = cos(radians);
		const double ss = sin(radians);
		const double rotationY = (xPixLoc - rotationOriginX) * cs + (yPixLoc - rotationOriginY) * ss;
		const double rotationX = (xPixLoc - rotationOriginX) * ss - (yPixLoc - rotationOriginY) * cs;
		//start rendering
		for (int i3 = 0; i3 < scale; i3++) {
			for (int i4 = 0; i4 < scale * lenght; i4++) {
				int y = (int)rotationOriginY + (int)round(i4 * cs + i3 * ss + rotationY);
				int x = (int)rotationOriginX + (int)round(i4 * ss - i3 * cs + rotationX);

				const int currRow = y - cameraY;
				const int rowLoc = currRow * row;
				const int colLoc = (x - cameraX) * 3;

				if (colLoc + 3 > row || colLoc < 0) continue;
				if (currRow < 0) continue;

				int loc = rowLoc + colLoc; // calculate print loc

				if (loc + 3 > size) continue;
				data[loc] = blue; // blue
				data[loc + 1] = green; // green
				data[loc + 2] = red; // red

				//fill empty pixels
				if (colLoc + 6 > row || colLoc < 0) continue;

				if (loc + 6 > size) continue;
				data[loc + 3] = blue; // blue
				data[loc + 3 + 1] = green; // green
				data[loc + 3 + 2] = red; // red
			}
		}
	}
}

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
void setPixelAtLocation(const int frameLoc, const int colorLoc, byte* byteImage) {
	std::lock_guard<std::mutex> lock(pixelMutex);
	if (byteImage[colorLoc] == 0 && byteImage[colorLoc + 1] == 0 && byteImage[colorLoc + 2] == 0) return;
	data[frameLoc] = byteImage[colorLoc]; // blue
	data[frameLoc + 1] = byteImage[colorLoc + 1]; // green
	data[frameLoc + 2] = byteImage[colorLoc + 2]; // red
}

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
			const int objXMin = obj->getX(); int objXMax = objXMin + objWidth;
			const int objYMin = obj->getY(); int objYMax = objYMin + objHeight;
			const int row = objWidth * 3;
			const int imageSize = objWidth * objHeight * 3;

			/*////////////////////
			* IF ROTATION
			*/////////////////////

			if (rotation > 0) {

			}

			/*////////////////////
			* IF NO ROTATION
			*/////////////////////

			else {
				// frame/camera data y axis
				const int offsetY = objYMin - cameraY;
				const int yFrameOffset = offsetY < 0 ? 0 : offsetY;
				const int yImageMin = offsetY < 0 ? offsetY * -1 : 0;
				const int yImageMax = objHeight - (objYMax - (cameraY + height) < 0 ? 0 : objYMax - (cameraY + height));

				// frame/camera data x axis
				const int offsetX = objXMin - cameraX;
				const int xFrameOffset = (offsetX < 0 ? 0 : offsetX) * 3;
				const int xImageMin = offsetX < 0 ? offsetX * -1 : 0;
				const int xImageMax = objWidth - (objXMax - (cameraX + width) < 0 ? 0 : objXMax - (cameraX + width));

				// loop data
				int currentRow = yImageMin * row;
				const int startXIncrement = xImageMin * 3;
				int xIncrement = startXIncrement;

				int currentFrameRow = yFrameOffset * frameRow;
				int frameXIncrement = xFrameOffset;

				// loop trough image
				for (int y = yImageMin; y < yImageMax; y++) {
					for (int x = xImageMin; x < xImageMax; x++) {

						int colorLoc = imageSize - (currentRow + xIncrement);
						int frameLoc = currentFrameRow + frameXIncrement;

						xIncrement += 3;
						frameXIncrement += 3;

						setPixelAtLocation(frameLoc, colorLoc, byteImage); // set final pixel
					}
					frameXIncrement = xFrameOffset;
					xIncrement = startXIncrement;
					currentRow += row;
					currentFrameRow += frameRow;
				}
			}
		}
	}
}

//draw frame on console
std::map<int, std::list<GameObject*>*> safelyGetGameObjects();
void drawFrame(SEngine * engine) {

	const int width = getScreenWidth();
	const int height = getScreenHeight();
	const int scale = engine->getPixelScale();

	//setup pixel drawing
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
		std::list<GameObject*> objects = *(iter->second);
		const int frameSize = (int)objects.size();

		//calculate threads needed
		const int threadCap = 1;
		int threadsToUse = frameSize / 40;
		if (threadsToUse > threadCap) threadsToUse = threadCap;
		else if (threadsToUse < 1) threadsToUse = 1;
		if (threadsToUse == 1) {
			calculateImages(objects, width, height, 0, (int)frameSize, cameraX, cameraY, engine);
		}
		else {
			std::vector<std::thread> threads;
			//do tasks
			double increment = 1.0 / threadsToUse;
			double begin = 0;
			for (int i = 0; i < threadsToUse; i++) {
				threads.push_back(std::thread(calculateImages, objects, width, height, (int)(frameSize * begin), (int)(frameSize * (begin + increment)), cameraX, cameraY, engine));
				begin += increment;
			}
			//finish tasks 
			for (int i = 0; i < threadsToUse; i++) {
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
		drawFrame(engine);

		//get end time
		auto end = std::chrono::steady_clock::now();

		//get time differance
		double elapsedTime = double(std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count());

		std::stringstream stm;
		stm << "FPS " << (int)(1000 / elapsedTime) << " : Load Time " << elapsedTime << std::endl;
		setBottomTextBox(stm.str());
	}
}