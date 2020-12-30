#include <iostream>
#include <vector>

#include "SEngine.h"

using namespace std;

//constructor
Image::Image() { 
	image = new vector< vector<Pixel*>* >();
}
//set line
void Image::addLine(string line) {
	vector<Pixel*> * pixels = new vector<Pixel*>();
	Pixel * pixel = new Pixel();
	char oldC = ' ';
	int offset = 0;
	for (int i = 0; i < line.length(); i++) {
		char c = line.at(i);

		//pre calculate pixel rows for performance
		if (c != ' ') {
			pixel->setLenght(pixel->getLenght() + 1);
		}
		if (c != oldC && oldC != ' ') {
			//push and reset
			pixel->setOffset(offset);
			pixels->push_back(pixel);
			pixel = new Pixel();
		}
		else if (i + 1 == line.length()) {
			//push and reset
			pixel->setOffset(offset);
			pixels->push_back(pixel);
			pixel = new Pixel();
		}

		if (c == ' ') offset = i + 1;
		oldC = c;
	}
	image->push_back(pixels);
}
//get image vector
vector< vector<Pixel * > * > *& Image::getVector() {
	return image;
}
//calculate width/height
int Image::calcWidth() {
	vector < vector<Pixel*> * >::iterator iter = image->begin();
	int widest = 0; 
	while (iter != image->end()) {
		widest = (*iter)->size() > widest ? (*iter)->size() : widest;
		iter++;
	}
	return widest;
}
int Image::calcHeight() {
	return image->size();
}