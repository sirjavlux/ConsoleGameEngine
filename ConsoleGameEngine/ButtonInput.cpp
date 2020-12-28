#include <iostream>
#include <conio.h>
#include <Windows.h>

#define LOG(x) cout << x << endl;

using namespace std;

int lastKey = 0;

//get last key pressed
int * getLastKeyPressed() {
	return &lastKey; 
}

//start key checking loop
void startCheckingKeyInput() {
	while (true) {
		lastKey = _getch(); //look for button input
	}
}