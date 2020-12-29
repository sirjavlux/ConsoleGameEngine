#include <iostream>
#include <vector>

#include "SEngine.h"

using namespace std;

//constructor
Image::Image() { }
//set line
void Image::addLine(string line) {
	vector<char> chars;
	for (int i = 0; i < line.length(); i++) {
		char c = line.at(i);
		chars.push_back(c);
	}
	image.push_back(chars);
}
//get image vector
vector< vector<char> > Image::getVector() {
	return image;
}
//calculate width/height
int Image::calcWidth() {
	vector < vector<char> >::iterator iter = image.begin();
	int widest = 0; 
	while (iter != image.end()) {
		widest = (*iter).size() > widest ? (*iter).size() : widest;
		iter++;
	}
	return widest;
}
int Image::calcHeight() {
	return image.size();
}