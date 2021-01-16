#ifndef LOG_
#define LOG_

#include <iostream>
#include <map>
#include <Windows.h>

#define LOG(x) cout << x << endl;

//screen utils
void ClearScreen();
int getScreenWidth();
int getScreenHeight();

/*////////////////////////
* FrameMap Class
*/////////////////////////

template <int H, int W, class T, class U>
class PairMap {
private:
	std::pair<T, U> pairArr[H][W] = { {} };
	int size = H * W;
public:
	void setValue(std::pair<T, U> pair, int first, int second) {
		pairArr[first][second] = pair;
	}
	std::pair<T, U> getValues(int first, int second) {
		return pairArr[first][second];
	}
	void clear() {
		memset(pairArr, 0, sizeof(pairArr));
	}
};

/*////////////////////////
* Color Map Struct
*/////////////////////////

class ImageColorMap {
private:
	std::map<char, COLORREF> map;
public:
	ImageColorMap();
	COLORREF getColor(char c);
	void setColor(char c, COLORREF color);
};

/*/////////////////////////////////
* Enum Utils
*//////////////////////////////////

template <typename Enumeration>
auto getEnumInteger(Enumeration const value)->typename std::underlying_type<Enumeration>::type
{
	return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

#endif