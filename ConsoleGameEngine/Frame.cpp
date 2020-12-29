#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <cmath>
#include <deque>
#include <list>
#include <Windows.h>
#include <thread>

#include "SEngine.h"
#include "Utils.h"
#include "DefinedImageColors.h"

using namespace std;

/*
FIX STACK PROBLEM
*/

//draw frame on console
void drawFrame() {
	
	//resetArrays();
	char pixels[frameH][frameW] = { {} };
	int pixelsLayer[frameH][frameW] = { {} };

	int x = getCameraX() - frameW / 2;;
	int y = getCameraY() - frameH / 2;;

	vector<string> frame;
	bool abort = false;

	try {
		frame = getInFrame();
	}
	catch (exception) {
		abort = true;
	}

	//make this faster
	if (!abort) {
		vector<string>::iterator iter = frame.begin();
		while (iter != frame.end()) {

			if ((*iter).empty()) {
				iter++; //increase
				continue;
			}
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
					if (!(yPixLoc < y + frameH && yPixLoc >= y)) continue;

					//loop trough x axis and place into pixelarray
					if (imageVector[i].size() > 0) {
						for (int i2 = 0; i2 < imageVector[i].size(); i2++) {
							int xPixLoc = xLoc + i2;
							if (!(xPixLoc < x + frameW && xPixLoc >= x)) continue;
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

	for (int i = 0; i < frameW + 2; i++) stm << borderStr; //upper border
	stm << RESET << endl;

	//loop trough y axis of pixel array
	for (int yIn = 0; yIn < frameH; yIn++) {
		stm << borderStr;
		for (int xIn = 0; xIn < frameW; xIn++) {
			char c = pixels[frameH - (yIn + 1)][xIn];
			convertToColored(c, ' ', ' ', stm); //implement letter support / text support <-----------------
		}
		stm << borderStr << RESET << endl;
	}

	for (int i = 0; i < frameW + 2; i++) stm << borderStr; //lower border
	stm << RESET;

	cout << stm.str() << endl; //print frame

	//print underneth text
	cout << getBottomTextBox();

	//fix console writing/remove flicker and reset
	ClearScreen();
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