#include <iostream>
#include <vector>

#include "SEngine.h"

using namespace std;

//constructor
Image::Image(ImageColorMap * colorMap) {
	image = new vector< vector<Pixel*>* >();
	cMap = colorMap;

}
Image::Image() {
	image = new vector< vector<Pixel*>* >();
	cMap = &ImageColorMap();

}
//set line
void Image::addLine(string line) {
	vector<Pixel*> * pixels = new vector<Pixel*>();
	char oldC = ' ';
	int offset = 0;
	int lenght = 0;
	for (int i = 0; i < line.length(); i++) {
		char c = line.at(i);

		//pre calculate pixel rows for performance
		if (oldC != ' ') {
			lenght++;
		}
		if (c != oldC && oldC != ' ') {
			//push and reset
			Pixel * pixel = new Pixel();
			pixel->setLenght(lenght);
			pixel->setColor(cMap->getColor(oldC));
			pixel->setOffset(offset);
			pixels->push_back(pixel);
			lenght = 0;
			offset = i;
		}
		else if (i + 1 == line.length()) {
			//push and reset
			Pixel* pixel = new Pixel();
			pixel->setLenght(lenght + 1);
			pixel->setColor(cMap->getColor(c));
			pixel->setOffset(offset);
			pixels->push_back(pixel);
			lenght = 0;
		}
		if (c == ' ') {
			offset = i + 1;
		}
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