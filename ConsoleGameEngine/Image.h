#pragma once
#include <vector>
#include <iostream>
#include <Windows.h>

class Image
{
private:
	std::vector<std::vector<char>> image;
public:
	Image();
	void addLine(std::string line);
	std::vector<std::vector<char>> getVector();
	int calcWidth();
	int calcHeight();
};

//defined background colors
#define b "\033[40m" /* Black */
#define R "\033[41m" /* Red */
#define G "\033[42m" /* Green */
#define Y "\033[43m" /* Yellow */
#define B "\033[44m" /* Blue */
#define M "\033[45m" /* Magenta */
#define C "\033[46m" /* Cyan */
#define W "\033[47m" /* White */

//text colors defined
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "[1m\033[30m"      /* Bold Black */
#define BOLDRED     "[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "[1m\033[37m"      /* Bold White */