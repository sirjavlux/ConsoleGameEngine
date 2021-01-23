#include "SEngine.h"

Triangle::Triangle() {
	locations = new std::pair<int, int>[3];
}

Triangle::Triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
	locations = new std::pair<int, int>[3]{ std::pair<int, int>(x1, y1), std::pair<int, int>(x2, y2), std::pair<int, int>(x3, y3) };
}

void Triangle::setLocation(int locNr, int x, int y) {
	if (locNr < 3) locations[locNr] = std::pair<int, int>(x, y);
}

std::pair<int, int>* Triangle::getTriangleLocations() {
	return locations;
}