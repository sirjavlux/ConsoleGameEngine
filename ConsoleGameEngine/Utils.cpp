#include <Windows.h>
#include <iostream>

using namespace std;

HANDLE	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

void ClearScreen() {
    cout << flush;
    COORD Position = { 0, 0 };
    SetConsoleCursorPosition(hOut, Position);
}

int getScreenHeight() {
	HWND myconsole = GetConsoleWindow();
	//window size
	RECT r;
	GetWindowRect(myconsole, &r);
	return r.bottom - r.top;
}

int getScreenWidth() {
	HWND myconsole = GetConsoleWindow();
	//window size
	RECT r;
	GetWindowRect(myconsole, &r);
	return r.right - r.left;
}

void printPixels() {

}