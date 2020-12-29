#include <Windows.h>
#include <iostream>

using namespace std;

HANDLE	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

void ClearScreen() {
    cout << flush;
    COORD Position = { 0, 0 };
    SetConsoleCursorPosition(hOut, Position);
}

