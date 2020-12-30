#include <iostream>

#include "SEngine.h"

using namespace std;

Pixel::Pixel(COLORREF color, int lenght, int offset) {
	COLOR = color;
	l = lenght;
	xOffset = offset;
}

Pixel::Pixel(int lenght, int offset) {
	COLOR = RGB(255, 160, 100);
	l = lenght;
	xOffset = offset;
}

Pixel::Pixel() {
	COLOR = RGB(255, 160, 100);
	l = 0;
	xOffset = 0;
}

void Pixel::setLenght(int lenght) {
	l = lenght;
}

int Pixel::getLenght() {
	return l;
}

void Pixel::setOffset(int offset) {
	xOffset = offset;
}

int Pixel::getOffset() {
	return xOffset;
}

void Pixel::setColor(COLORREF color) {
	COLOR = color;
}

COLORREF Pixel::getColor() {
	return COLOR;
}