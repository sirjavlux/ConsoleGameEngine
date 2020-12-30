#ifndef LOG

#include <iostream>

#include "SEngine.h"

#define LOG(x) cout << x << endl;

//screen clear util
void ClearScreen();

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

#endif