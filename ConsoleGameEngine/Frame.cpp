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

#include "SEngine.h"
#include "Utils.h"
#include "DefinedImageColors.h"

using namespace std;

//draw frame on console
void drawFrame(SEngine * engine) {

	int width = getScreenWidth();
	int height = getScreenHeight();
	int scale = engine->getPixelScale();

	int x = engine->getCameraX() - width / 2;
	int y = engine->getCameraY() - height / 2;

	vector<string> * frame = getInFrame();

	const int startSize = frame->size();

	const int size = width * height * 3;
	byte* data = new byte[size]{};

	//make this faster 1160
	for (int it = 0; it < startSize; it++) {
		//variables
		GameObject* obj;
		try {
			obj = getGameObject(frame->at(it));
		} 
		catch (exception) { continue; }

		int xLoc = obj->getX();
		int yLoc = obj->getY();
		int layer = obj->getLayer();
		Image* image = obj->getImage();
		vector< vector<Pixel* >* >* imageVector = image->getVector();

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
						int red = GetRValue(color);
						int green = GetGValue(color);
						int blue = GetBValue(color);
						for (int p = 0; p < pixel->getLenght(); p++) {
							int xPixLoc = (xLoc + pixel->getOffset() + p) * scale;
							if (xPixLoc < x + width + scale && xPixLoc + scale > x) {
								for (int i3 = 0; i3 < scale; i3++) {
									for (int i4 = 0; i4 < scale; i4++) {
										int printLocX = xPixLoc - x + i4;
										int printLocY = yPixLoc - y + i3;
										if (printLocY >= height - 1 || printLocY <= 0) continue;
										if (printLocX >= width - 1 || printLocX <= 0) continue;
										//allocate pixels
										data[printLocY * 3 * width + printLocX * 3] = blue; // blue
										data[printLocY * 3 * width + printLocX * 3 + 1] = green; // green
										data[printLocY * 3 * width + printLocX * 3 + 2] = red; // red
									}
								}
							}
						}
					}
				}
			}
		}
	}

	//draw pixels
	BITMAPINFOHEADER bmih;
	bmih.biBitCount = 24;
	bmih.biClrImportant = 0;
	bmih.biClrUsed = 0;
	bmih.biCompression = BI_RGB;
	bmih.biWidth = width;
	bmih.biHeight = height;
	bmih.biPlanes = 1;
	bmih.biSize = 40;
	bmih.biSizeImage = size;

	BITMAPINFO bmpi;
	bmpi.bmiHeader = bmih;
	SetDIBitsToDevice(engine->getDC(), 0, 0, width, height, 0, 0, 0, height, data, &bmpi, DIB_RGB_COLORS);
	delete[] data;

	//print underneth text
	cout << getBottomTextBox() << " " << width << " : " << height;

	//fix console writing/remove flicker and reset
	ClearScreen();
}

//frame drawing loop
void startConsoleDraw(SEngine* engine) {
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

		stringstream stm;
		stm << "FPS " << (int)(1000 / elapsedTime) << " : Load Time " << elapsedTime << endl;
		setBottomTextBox(stm.str());
	}
}