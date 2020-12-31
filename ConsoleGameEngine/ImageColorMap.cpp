#include <vector>
#include <iostream>

#include "Utils.h"

using namespace std;

ImageColorMap::ImageColorMap() {

}

COLORREF ImageColorMap::getColor(char c) {
	COLORREF color = RGB(0, 0, 0);
	if (map.find(c) != map.end()) {
		color = map.at(c);
	}
	return color;
}

void ImageColorMap::setColor(char c, COLORREF color) {
	map[c] = color;
}