#include <iostream>
#include <vector>

#include "SEngine.h"

using namespace std;

//constructor
Image::Image(ImageColorMap * colorMap) {
	image = new vector< vector<DrawPixel*>* >();
	cMap = colorMap;

}
Image::Image() {
	image = new vector< vector<DrawPixel*>* >();
	cMap = &ImageColorMap();

}
//set line
void Image::addLine(string line) {
	vector<DrawPixel*> * pixels = new vector<DrawPixel*>();
	for (int i = 0; i < line.length(); i++) {
		char c = line.at(i);
		if (c != ' ') {
			COLORREF color = cMap->getColor(c);
			pixels->push_back(new DrawPixel(0, new COLORREF(RGB(GetRValue(color), GetGValue(color), GetBValue(color)))));
		}
		else {
			pixels->push_back(nullptr);
		}
	}
	image->push_back(pixels);
}
//get image vector
vector< vector<DrawPixel * > * > *& Image::getVector() {
	return image;
}
//calculate width/height
int Image::calcWidth() {
	vector < vector<DrawPixel*> * >::iterator iter = image->begin();
	int widest = 0; 
	while (iter != image->end()) {
		int size = (*iter)->size();
		if (size > widest) widest = size;
		iter++;
	}
	return widest;
}
int Image::calcHeight() {
	return image->size();
}