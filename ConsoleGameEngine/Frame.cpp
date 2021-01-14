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

#include "Chunk.h"
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
				int y = rotationOriginY + (int) round(i4 * cs + i3 * ss + rotationY);
				int x = rotationOriginX + (int) round(i4 * ss - i3 * cs + rotationX);

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

//calculate pixels based on object images
std::list<Chunk*> safelyGetChunksInFrame();
void calculateImages(std::list<Chunk*> * frame, int width, int height, int from, int to, int cameraX, int cameraY, SEngine* engine) {
	std::list<Chunk*>::iterator iter = frame->begin();
	std::advance(iter, from);
	int toMove = to - from;
	int count = 0;
	// frame data
	const int row = 3 * width;
	const int size = width * height * 3;
	const int scale = engine->getPixelScale();
	// start looping trough chunks
	while (iter != frame->end()) {
		if (count >= toMove) break;
		else {
			// get chunk data
			Chunk* chunk = *iter;
			int y = chunk->getLocation()->first * 100;
			int x = chunk->getLocation()->second * 100;
			byte* colors = chunk->getPixelColors();

			// calculate y axis
			const int offsetY = y - cameraY;

			// calculate x axis
			const int offsetX = x - cameraX;
			const int scaledOffsetX = offsetX * 3;

			// frame data
			const int startRow = offsetY * row;
			int currentRow = startRow;
			int currentColorRow = 0;
			int xIncrement = 0;

			for (int i = 0; i < 100; i++) {
				for (int i2 = 0; i2 < 100; i2++) {
					int placeX = scaledOffsetX + xIncrement;
					int colorLoc = currentColorRow + xIncrement;
					xIncrement += 3;

					if (placeX >= row || placeX <= 0) continue;
					int loc = currentRow + placeX;
					if (loc >= size || loc < 0) continue;
					
					if (colors == nullptr) break;
					data[loc] = colors[colorLoc]; // blue
					data[loc + 1] = colors[colorLoc + 1]; // green
					data[loc + 2] = colors[colorLoc + 2]; // red
				}
				xIncrement = 0;
				currentRow += row;
				currentColorRow += 300;
			}
		}
		count++;
		iter++;
	}
}

//draw frame on console
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

	std::list<Chunk*> frame = safelyGetChunksInFrame();
	
	const double chunkSize = frame.size();

	//calculate threads needed
	const int threadCap = 8;
	int threadsToUse = 2;
	if (threadsToUse > threadCap) threadsToUse = threadCap;
	else if (threadsToUse < 1) threadsToUse = 1;
	if (threadsToUse == 1) {
		calculateImages(&frame, width, height, 0, chunkSize, cameraX, cameraY, engine);
	}
	else {
		std::vector<std::thread> threads;
		//do tasks
		double increment = 1.0 / threadsToUse;
		double begin = 0;
		for (int i = 0; i < threadsToUse; i++) {
			threads.push_back(std::thread(calculateImages, &frame, width, height, (int)(chunkSize * begin), (int)(chunkSize * (begin + increment)), cameraX, cameraY, engine));
			begin += increment;
		}
		//finish tasks 
		for (int i = 0; i < threadsToUse; i++) {
			threads.at(i).join();
		}
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