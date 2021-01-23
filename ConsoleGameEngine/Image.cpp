#include <iostream>
#include <vector>

#include "SEngine.h"

using namespace std;

//constructor
Image::Image(ImageColorMap * colorMap) {
	image = new vector< vector<DrawPixel*>* >();
	cMap = colorMap;
	byteImage = { };
}
Image::Image() {
	image = new vector< vector<DrawPixel*>* >();
	cMap = new ImageColorMap();
	byteImage = { };
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
//create byte image
void Image::createByteImage(SEngine * engine) {
	delete[] byteImage;
	const int scale = engine->getPixelScale();
	const int w = calcWidth();
	const int h = calcHeight();
	const int row = w * 3;
	const int size = h * row;
	byteImage = new byte[size]();

	int currentRow = 0;
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			if (image->at(y)->size() <= x) continue;
			DrawPixel* pixel = image->at(y)->at(x);
			if (pixel == nullptr) continue;
			COLORREF* colorPtr = pixel->getColor();
			COLORREF color = *colorPtr;
			const byte red = GetRValue(color);
			const byte green = GetGValue(color);
			const byte blue = GetBValue(color);

			int loc = y * row + x * 3;

			byteImage[loc] = blue;
			byteImage[(int)(loc) + (int)1] = green;
			byteImage[(int)(loc) + (int)2] = red;
		}
	}
}
//get byte image
byte* Image::getByteImage() {
	return byteImage;
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
		int size = (int)(*iter)->size();
		if (size > widest) widest = size;
		iter++;
	}
	return widest;
}
int Image::calcHeight() {
	return (int)image->size();
}