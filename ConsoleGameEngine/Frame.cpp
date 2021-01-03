#define _WIN32_WINNT 0x0500
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

#include "SEngine.h"
#include "Utils.h"
#include "DefinedImageColors.h"

BITMAPINFOHEADER bmih;
BITMAPINFO bmpi;
int oldSize = getScreenWidth() * getScreenHeight() * 3;
byte* data = new byte[oldSize]{};

//set to image map
void setPixel(int xPixLoc, int yPixLoc, int x, int y, int width, int height, int scale, int lenght, COLORREF color) {
	const int red = GetRValue(color);
	const int green = GetGValue(color);
	const int blue = GetBValue(color);
	const int startPrintX = (xPixLoc - x) * 3;
	const int row = 3 * width;
	int printLoc = (yPixLoc - y) * 3 * width + (xPixLoc - x) * 3; // calculate start print location
	const int startRow = printLoc / row;
	const int pixelLenght = scale * lenght;
	const int pixelColorLenght = pixelLenght * 3;
	int index = 0;
	const int outOfFrameCheck = row - startPrintX;
	const int size = 3 * width * height;
	for (int i3 = 0; i3 < scale; i3++) {
		if (startRow + i3 >= height - 1 || startRow + i3 <= 0) continue;
		for (int i4 = 0; i4 < pixelLenght; i4++) {
			if (index + 2 > outOfFrameCheck) {
				break;
			}
			int loc = printLoc + index;
			if (loc + 3 > size || loc < 0) {
				return;
			}
			data[loc] = blue; // blue
			data[loc + 1] = green; // green
			data[loc + 2] = red; // red

			index += 3; // increase to next pixel index
		}
		printLoc += row; // increase to next row
		index = 0;
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
void calculateImages(int layer, std::map<int, std::list<std::string>> * frame, int from, int to, int scale, int width, int height, int x, int y) {

	int count = 0;
	int toMove = to - from;
	std::list<std::string>::iterator it = frame->at(layer).begin();
	std::advance(it, from);
	while (it != frame->at(layer).end()) {
		//variables
		GameObject* obj;
		try {
			obj = getGameObject(*it);
			it++;
		}
		catch (std::exception) { continue; it++; }

		int xLoc = obj->getX();
		int yLoc = obj->getY();
		int layer = obj->getLayer();
		Image* image = obj->getImage();
		std::vector< std::vector<Pixel* >* >* imageVector = image->getVector();

		//loop trough y axis
		if (imageVector->size() > 0) {
			int rows = imageVector->size();
			for (int i = 0; i < rows; i++) {
				int yPixLoc = (yLoc + i) * scale;
				if (!(yPixLoc < y + height && yPixLoc >= y)) continue;
				int row = rows - (i + 1);
				if (imageVector->at(row)->size() > 0) {
					for (int i2 = 0; i2 < imageVector->at(row)->size(); i2++) {
						//render pixels
						Pixel* pixel = imageVector->at(row)->at(i2);
						COLORREF color = pixel->getColor();
						//allocate pixels
						setPixel((xLoc + pixel->getOffset()) * scale, yPixLoc, x, y, width, height, scale, pixel->getLenght(), color);
					}
				}
			}
		}

		//check if should end
		if (count + 1 >= toMove) {
			break;
		}
		count++;
	}
}

//draw frame on console
void drawFrame(SEngine * engine) {

	int width = getScreenWidth();
	int height = getScreenHeight();
	int scale = engine->getPixelScale();

	//setup pixel drawing
	int size = width * height * 3;
	if (size != oldSize) {
		updateFrameDrawSize(width, height);
	}
	else {
		resetPixelData(size);
	}

	int x = engine->getCameraX() - width / 2;
	int y = engine->getCameraY() - height / 2;

	std::map<int, std::list<std::string>> frame = getInFrame();
	std::map<int, std::list<std::string>>::iterator iter = frame.begin();
	
	while (iter != frame.end()) {
		const int layer = (*iter).first;
		const int startSize = frame.at(layer).size();

		//calculate lines
		int lines = 0;
		std::list<std::string> * objects = &frame.at(layer);
		std::list<std::string>::iterator it = objects->begin();
		while (it != objects->end()) {
			lines += (*it).size();
			it++;
		}
		//calculate threads needed
		const int threadCap = 8;
		int threadsToUse = lines / 150;
		if (threadsToUse > threadCap) threadsToUse = threadCap;
		else if (threadsToUse < 1) threadsToUse = 1;
		if (threadsToUse == 1) {
			calculateImages(layer, &frame, 0, (int)startSize, scale, width, height, x, y);
		}
		else {
			std::vector<std::thread> threads;
			//do tasks
			double increment = 1.0 / threadsToUse;
			double begin = 0;
			for (int i = 0; i < threadsToUse; i++) {
				threads.push_back(std::thread(calculateImages, layer, &frame, (int)startSize * begin, (int)startSize * (begin + increment), scale, width, height, x, y));
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

	//print underneth text
	std::cout << " " << getBottomTextBox() << " " << width << " : " << height;

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