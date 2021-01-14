#include "DrawPixel.h"

DrawPixel::DrawPixel(int layer, COLORREF * color) {
	DrawPixel::layer = layer;
	COLOR = color;
}

DrawPixel::DrawPixel() {
	layer = 0;
	*COLOR = RGB(0, 0, 0);
}

void DrawPixel::setLayer(int layer) {
	DrawPixel::layer = layer;
}

int DrawPixel::getLayer() {
	return layer;
}

void DrawPixel::setColor(COLORREF * color) {
	COLOR = color;
}

COLORREF * DrawPixel::getColor() {
	return COLOR;
}