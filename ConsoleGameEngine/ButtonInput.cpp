#include <iostream>
#include <conio.h>
#include <Windows.h>

#include "SEngine.h"

using namespace std;

int lastKey = 0;

//get last key pressed
int * getLastKeyPressed() {
	return &lastKey; 
}

//start key checking loop
void startCheckingKeyInput() {
	while (isGameRunning()) {
		lastKey = _getch(); //look for button input
	}
}