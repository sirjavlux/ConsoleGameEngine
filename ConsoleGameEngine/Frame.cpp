#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <cmath>
#include <deque>
#include <Windows.h>
#include <thread>

#include "Frame.h"
#include "Engine.h"
#include "GameObject.h"
#include "Image.h"
#include "Utils.h"

using namespace std;

//frame constructor
Frame::Frame() {
	h = frameH;
	w = frameW;
	x = getCameraX() - w / 2;
	y = getCameraY() - h / 2;
}

//draw frame on console
void Frame::draw() {

	deque<string> frame;
	bool abort = false;

	try {
		frame = *getInFrame();
	}
	catch (exception) {
		abort = true;
	}
	
	//fix console writing/remove flicker and reset
	ClearScreen();

	//make this faster
	if (!abort) {
		deque<string>::iterator iter = frame.begin();
		while (iter != frame.end()) {
			//variables
			string objName = *iter;
			GameObject obj = getGameObject(objName);
			int xLoc = obj.getX();
			int yLoc = obj.getY();
			int height = obj.getHeight();
			int width = obj.getWidth();
			int layer = obj.getLayer();
			Image image = obj.getImage();
			vector< vector<char> > imageVector = image.getVector();

			iter++; //increase

			//loop trough y axis
			if (imageVector.size() > 0) {
				for (int i = 0; i < imageVector.size(); i++) {
					int yPixLoc = yLoc + height - i;
					if (!(yPixLoc < y + h && yPixLoc >= y)) continue;

					//loop trough x axis and place into pixelarray
					if (imageVector[i].size() > 0) {
						for (int i2 = 0; i2 < imageVector[i].size(); i2++) {
							int xPixLoc = xLoc + i2;
							if (!(xPixLoc < x + w && xPixLoc >= x)) continue;
							else {
								char c = imageVector[i].at(i2);
								char oldC = pixels[yPixLoc - y][xPixLoc - x];
								if (c == emptyPixel) continue;
								else if (oldC != emptyPixel) {
									int prevLayer = pixelsLayer[yPixLoc - y][xPixLoc - x];
									if (layer <= prevLayer) continue;
								}
								pixels[yPixLoc - y][xPixLoc - x] = c;
								pixelsLayer[yPixLoc - y][xPixLoc - x] = layer;
							}
						}
					}
				}
			}
		}
	}

	//loop trough pixels and put together a single line to print
	stringstream stm;

	for (int i = 0; i < w + 2; i++) stm << borderStr; //upper border
	stm << RESET << endl;

	//loop trough y axis of pixel array
	for (int yIn = 0; yIn < h; yIn++) {
		stm << borderStr;
		for (int xIn = 0; xIn < w; xIn++) {
			char c = pixels[h - (yIn + 1)][xIn];
			convertToColored(c, ' ', ' ', stm); //implement letter support / text support <-----------------
		}
		stm << borderStr << RESET << endl;
	}

	for (int i = 0; i < w + 2; i++) stm << borderStr; //lower border
	stm << RESET;

	cout << stm.str() << endl; //print frame

	//print underneth text
	cout << getBottomTextBox();
}

//convert to colored
char lastBackColor = 'b';
void convertToColored(char c, char letter, char textC, stringstream & stm) {
	//background color
	if (lastBackColor != c) {
		lastBackColor = c;
		switch (c) {
		case 'b': stm << b;
			break; //black
		case 'R': stm << R;
			break; //red
		case 'G': stm << G;
			break; //green
		case 'Y': stm << Y;
			break; //yellow
		case 'B': stm << B;
			break; //blue
		case 'M': stm << M;
			break; //magenta
		case 'C': stm << C;
			break; //cyan
		case 'W': stm << W;
			break; //white
		default: stm << b;
			break; //default is black
		}
	}
	//text color
	if (textC != ' ' && letter != ' ') {
		switch (textC) {
		case 'b': stm << BLACK;
			break; //black
		case 'R': stm << RED;
			break; //red
		case 'G': stm << GREEN;
			break; //green
		case 'Y': stm << YELLOW;
			break; //yellow
		case 'B': stm << BLUE;
			break; //blue
		case 'M': stm << MAGENTA;
			break; //magenta
		case 'C': stm << CYAN;
			break; //cyan
		case 'W': stm << WHITE;
			break; //white
		}
	}
	//set letter
	stm << letter;
	//reset colors after letter
	if (textC != ' ') {
		stm << RESET;
		lastBackColor = '0';
	}
}