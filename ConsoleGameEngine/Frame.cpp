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
#include <Bits.h>

#include "SEngine.h"
#include "Utils.h"
#include "DefinedImageColors.h"

using namespace std;

COLORREF COLOR = RGB(255, 160, 100);

//draw frame on console
void drawFrame(SEngine * engine) {

	int width = getScreenWidth();
	int height = getScreenHeight();
	int scale = engine->getPixelScale();

	int x = engine->getCameraX() - width / 2;
	int y = engine->getCameraY() - height / 2;

	vector<string> * frame = getInFrame();

	const int startSize = frame->size();

	HWND myconsole = GetConsoleWindow();
	HDC mydc = GetDC(myconsole);

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
			for (int i = 0; i < imageVector->size(); i++) {
				int yPixLoc = yLoc + obj->getHeight() - i;
				if (!(yPixLoc < y + height && yPixLoc >= y)) continue;
				if (imageVector->at(i)->size() > 0) {
					for (int i2 = 0; i2 < imageVector->at(i)->size(); i2++) {
						//render pixels
						Pixel* pixel = imageVector->at(i)->at(i2);
						COLORREF color = pixel->getColor();
						for (int p = 0; p < pixel->getLenght(); p++) {
							int xPixLoc = xLoc + pixel->getOffset() + p;
							if (xPixLoc < x + width && xPixLoc >= x) {
								for (int i3 = 0; i3 < scale; i3++) {
									for (int i4 = 0; i4 < scale; i4++) {
										int printLocX = xPixLoc * scale - x + i4;
										int printLocY = yPixLoc * scale - y + i3;
										//render
										SetPixel(mydc, printLocX, printLocY, color);

										/*//////////////////////////////////
										Need to fix bitmap rendering
										*///////////////////////////////////
									}
								}
							}
						}
					}
				}
			}
		}
	}

	//print underneth text
	cout << getBottomTextBox();

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