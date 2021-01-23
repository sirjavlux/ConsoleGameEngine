#include "SEngine.h"

Colider2D::Colider2D() {
	triangles = new Triangle[0];
}

void Colider2D::createTriangles(Image* image, double intensity, SEngine * engine) {
	int skipValue = (int)round(1.0 / intensity);

	//calculate vertecie count
	byte * byteImage = image->getByteImage();
	const int width = image->calcWidth();
	const int height = image->calcHeight();
	const int row = width * 3;
	const int scale = engine->getPixelScale();

	//increments
	int yIncrement = 0;
	int xIncrement = 0;

	//get vertecies
	std::deque<std::pair<int, int>> verts;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int loc = yIncrement + xIncrement;
			xIncrement += 3;

			byte blue = byteImage[loc];
			byte green = byteImage[loc + 1];
			byte red = byteImage[loc + 2];

			if (blue == 0 && green == 0 && red == 0) continue;

			//check if pixel is corner
			bool left = false, right = false, up = false, down = false;
			//left
			if (x > 0) {
				byte leftBlue = byteImage[loc - 3];
				byte leftGreen = byteImage[loc - 2];
				byte leftRed = byteImage[loc - 1];
				if (blue != 0 || green != 0 || red != 0) left = true;
			}
			//right
			if (x + 1 < width) {
				byte rightBlue = byteImage[loc + 3];
				byte rightGreen = byteImage[loc + 4];
				byte rightRed = byteImage[loc + 5];
				if (blue != 0 || green != 0 || red != 0) right = true;
			}
			//continue if betwean
			if (right && left) continue;
			//up
			if (y + 1 < height) {
				byte upBlue = byteImage[loc + row];
				byte upGreen = byteImage[loc + 1 + row];
				byte upRed = byteImage[loc + 2 + row];
				if (blue != 0 || green != 0 || red != 0) up = true;
			}
			//down
			if (y > 0) {
				byte downBlue = byteImage[loc - row];
				byte downGreen = byteImage[loc + 1 - row];
				byte downRed = byteImage[loc + 2 - row];
				if (blue != 0 || green != 0 || red != 0) down = true;
			}
			//continue if betwean
			if (up && down) continue;
			//push back verecie
			//if left up corner
			if (!left && !up) {
				int finalX = x * scale, finalY = y * scale;
				finalY += scale;
				verts.push_back(std::pair<int, int>(finalX, finalY));
			} 
			//if right up corner
			if (!right && !up) {
				int finalX = x * scale, finalY = y * scale;
				finalY += scale;
				finalX += scale;
				verts.push_back(std::pair<int, int>(finalX, finalY));
			}
			//if left down corner
			if (!left && !down) {
				int finalX = x * scale, finalY = y * scale;
				verts.push_back(std::pair<int, int>(finalX, finalY));
			}
			//if right down corner
			if (!left && !down) {
				int finalX = x * scale, finalY = y * scale;
				finalX += scale;
				verts.push_back(std::pair<int, int>(finalX, finalY));
			}
		}
		xIncrement = 0;
		yIncrement += row;
	}

	//generate triangles
	std::deque<std::pair<int, int>>::iterator iter = verts.begin();
	std::deque<Triangle> trianglesNew;

	//calculate first vert
	std::pair<int, int> firstVert;
	int firstVertX = 0, firstVertY = 0;
	std::vector< std::vector<DrawPixel*>* >::iterator imageIter = image->getVector()->begin();
	int widest = 0;
	int count = 0;
	//y value
	while (imageIter != image->getVector()->end()) {
		int size = (int)(*imageIter)->size();
		if (size > widest) {
			widest = size;
			firstVertY = image->calcHeight() - count;
		}
		count++;
		imageIter++;
	}
	//x value
	int heighest = 0;
	for (int i = 0; i < widest; i++) {
		int c = 0;
		for (int i2 = 0; i2 < image->calcHeight(); i2++) {
			int loc = i2 * widest * 3 + i * 3;
			byte blue = byteImage[loc];
			byte green = byteImage[loc + 1];
			byte red = byteImage[loc + 2];

			if (blue == 0 && green == 0 && red == 0) break;
			else {
				c++;
				firstVertX = i;
				if (heighest < c) heighest = c;
			}
		}
	}
	firstVert = std::pair<int, int>(firstVertX, firstVertY);

	std::pair<int, int> lastVert;
	count = 0;
	while (iter != verts.end()) {
		std::pair<int, int> vert = *iter;
		//if second
		if (count == 0) {
			lastVert = vert;
			iter++;
			count++;
			continue;
		}
		//create triangle if all previous verts are set
		else {
			Triangle tri(firstVert.first, firstVert.second, lastVert.first, lastVert.second, vert.first, vert.second);
			trianglesNew.push_back(tri);
			lastVert = vert;
		}
		iter++;
		count++;
	}

	//set triangles
	int triSize = (int)trianglesNew.size();
	delete[] triangles;
	triangles = new Triangle[triSize]();
	std::deque<Triangle>::iterator triIter = trianglesNew.begin();
	count = 0;
	while (triIter != trianglesNew.end()) {
		Triangle tri = *triIter;
		triangles[count] = tri;
		triIter++;
		count++;
	}
}