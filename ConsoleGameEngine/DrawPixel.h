#ifndef _DRAWPIXEL_H
#define _DRAWPIXEL_H

#include <Windows.h>

/*///////////////////////////
* DRAW PIXEL
*////////////////////////////

class DrawPixel {
private:
	COLORREF* COLOR;
	int layer;
public:
	DrawPixel(int layer, COLORREF* color);
	DrawPixel();
	void setLayer(int layer);
	int getLayer();
	void setColor(COLORREF* color);
	COLORREF* getColor();
};

#endif