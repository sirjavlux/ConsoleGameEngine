#pragma once
#include "GameObject.h"
#include <list>
#include "Engine.h"
#include <deque>

//default pixel placeholders
const std::string borderStr = "\033[47m \033[0m";
const char emptyPixel = ' ';

class Frame {
private:
	int h, w, x, y;
	int pixels[frameH][frameW] = { { } };
	int pixelsLayer[frameH][frameW] = { {0} };
public:
	Frame();
	void draw();
};

void setBottomTextBox(std::string str);
std::string getBottomTextBox();
std::deque<std::string> * getInFrame();
bool overlapsFrame(GameObject obj);
void registerObjectToFrame(GameObject obj);
void updateObjectToFrame(GameObject obj);
void updateCameraMovementFrame();
void findObjectsInFrame();
void convertToColored(char c, char letter, char textC, std::stringstream& stm);