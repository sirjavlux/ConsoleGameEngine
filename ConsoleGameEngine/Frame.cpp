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

PairMap<frameH, frameW, char, int>* pixelMap = new PairMap<frameH, frameW, char, int>();

//draw frame on console
void drawFrame() {

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
			//variables
			if (!(*iter).empty()) {
				GameObject* obj = getGameObject(*iter);
				int xLoc = obj->getX();
				int yLoc = obj->getY();
				int layer = obj->getLayer();
				Image* image = obj->getImage();
				vector< vector<char> >* imageVector = image->getVector();

				iter++; //increase

				//loop trough y axis
				if (imageVector->size() > 0) {
					for (int i = 0; i < imageVector->size(); i++) {
						int yPixLoc = yLoc + obj->getHeight() - i;
						if (!(yPixLoc < y + frameH && yPixLoc >= y)) continue;

						//loop trough x axis and place into pixelarray
						if (imageVector->at(i).size() > 0) {
							for (int i2 = 0; i2 < imageVector->at(i).size(); i2++) {
								int xPixLoc = xLoc + i2;
								if (!(xPixLoc < x + frameW && xPixLoc >= x)) continue;
								else {
									char c = imageVector->at(i)[i2];
									char oldC = pixelMap->getValues(yPixLoc - y, xPixLoc - x).first;
									if (c == emptyPixel) continue;
									else if (oldC != emptyPixel) {
										int prevLayer = pixelMap->getValues(yPixLoc - y, xPixLoc - x).second;
										if (layer <= prevLayer) continue;
									}
									pixelMap->setValue(pair<char, int>(c, layer), yPixLoc - y, xPixLoc - x);
								}
							}
						}
					}
				}
			}
			else iter++;
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
			char c = pixelMap->getValues(frameH - (yIn + 1), xIn).first;
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

	pixelMap->clear();
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