#include "SEngine.h"

Chunk::Chunk(std::pair<int, int> location) {
	loc = new std::pair<int, int>(location.first, location.second);
	objects = new std::map<int, std::list<GameObject*>*>();
}

Chunk::~Chunk() {
	delete loc;
	delete objects;
}

void Chunk::addGameObject(GameObject* obj, SEngine* engine) {
	std::list<GameObject*>* list = (*objects)[obj->getLayer()];
	if (list == nullptr) {
		list = new std::list<GameObject*>();
		list->push_back(obj);
		(*objects)[obj->getLayer()] = list;
	}
	else if (std::find(list->begin(), list->end(), obj) == list->end()) list->push_back(obj);
	obj->addChunkAddress(this);
}

void Chunk::removeGameObject(GameObject* obj) {
	std::list<GameObject*>* list = (*objects)[obj->getLayer()];
	if (list != nullptr) list->remove(obj);
}

std::map<int, std::list<GameObject*>*> Chunk::getGameObjects() {
	return *objects;
}

std::pair<int, int> * Chunk::getLocation() {
	return loc;
}

//update chunk <--------------- fix this for faster chunk updating, copy image byte map onto chunk color-map.
/*
void Chunk::updateChunkPixels(SEngine* engine) {
	if (updateColors && colors != nullptr) {
		std::map<int, std::list<GameObject*>*> tempObjects = *getGameObjects();
		const int mapSize = (int)tempObjects.size();
		int y = loc->first * 100;
		int x = loc->second * 100;

		// loop trough layers
		for (int i = 0; i < mapSize; i++) {
			std::list<GameObject*>* tempObjectList = tempObjects[i];
			if (tempObjectList == nullptr) continue; // go next if nullptr

			// loop trough every object in the chunk and layer
			std::list<GameObject*>::iterator iter = tempObjectList->begin();
			while (iter != tempObjectList->end()) {
				GameObject* obj = *iter;
				byte* byteMap = obj->getImage()->getByteImage();
				const double scale = (double)engine->getPixelScale();

				// object data
				double rotation = obj->getRotation();
				const double objLocX = (double)obj->getX();
				const double objLocY = (double)obj->getY();
				const double objWidth = (double)obj->getWidth() * scale;
				const double objHeight = (double)obj->getHeight() * scale;
				const int row = objWidth * 3;
				const int objSize = objWidth * objHeight * 3;

				// calculate image offset from chunk
				const double offsetY = objLocY - y;
				const double offsetX = objLocX - x;

				// calculate in chunk image parts
				const int minImageY = offsetY > 0 ? 0 : offsetY * -1;
				const int minImageX = offsetX > 0 ? 0 : offsetX * -1;
				int maxImageY = objLocY + objHeight - y;
				int maxImageX = objLocX + objWidth - x;
				int imageYIncrement = minImageY * row;
				int imageXIncrement = minImageX * 3;
				if (maxImageY - minImageY > 100) maxImageY += 100 - (maxImageY - minImageY);
				if (maxImageX - minImageX > 100) maxImageX += 100 - (maxImageX - minImageX);

				//int imageYIncrement = -scale / 2; // used for getting current image y loc
				//int imageXIncrement = -scale / 2; // used for getting current image x loc

				// calculate chunk start / end points
				const int startY = offsetY < 0 ? 0 : offsetY;
				const int startX = offsetX < 0 ? 0 : offsetX;
				const int endY = maxImageY - minImageY > 100 ? 100 : maxImageY - minImageY;
				const int endX = maxImageX - minImageX > 100 ? 100 : maxImageX - minImageX;

				for (int i = startY; i < endY; i++) {
					for (int i2 = startX; i2 < endX; i2++) {
						int loc = i * 300 + i2 * 3;
						int imageLoc = imageYIncrement + imageXIncrement;

						colors[loc] = byteMap[imageLoc];
						colors[loc + 1] = byteMap[imageLoc + 1];
						colors[loc + 2] = byteMap[imageLoc + 2];

						imageXIncrement += 3;
					}
					imageXIncrement = minImageX * 3;
					imageYIncrement += row;
				}

				iter++;
			}
		}
	}

	updateChunk = false;
}
*/
//update chunk with new object

/*///////////////////////////////////////////////////////////////////////////
 REPLACE THIS METHOD WITH THE ONE ABOVE, LOOPING TROUGH ALL OBJECTS
*////////////////////////////////////////////////////////////////////////////

/*
void Chunk::updateGameObjectPixels(GameObject* obj, SEngine * engine) {
	Image* image = obj->getImage();
	std::vector< std::vector<DrawPixel*> *> * pixelVec = image->getVector();
	int y = loc->first * 100;
	int x = loc->second * 100;
	const double scale = (double)engine->getPixelScale();
	// object data
	double rotation = obj->getRotation();
	const double objLocX = (double)obj->getX();
	const double objLocY = (double)obj->getY();
	const double objWidth = (double)obj->getWidth() * scale;
	const double objHeight = (double)obj->getHeight() * scale;
	int objWidthOriginal = obj->getWidth();
	int objHeightOriginal = obj->getHeight();
	int layer = obj->getLayer();
	obj->addChunkAddress(this); // add chunk address for movement chaning later

	if (rotation > 0.0) {

	}

	else {
		// calculate image offset from chunk
		const double offsetY = objLocY - y;
		const double offsetX = objLocX - x;

		// calculate in chunk image parts
		const int minImageY = offsetY > 0 ? 0 : offsetY * -1;
		const int minImageX = offsetX > 0 ? 0 : offsetX * -1;
		const int imageYReach = objLocY + objHeight - y;
		const int imageXReach = objLocX + objWidth - x;
		int imageYIncrement = - scale / 2; // used for getting vurrent image y loc
		int imageXIncrement = - scale / 2; // used for getting vurrent image x loc

		// calculate chunk start / end points
		const int startY = offsetY < 0 ? 0 : offsetY;
		const int startX = offsetX < 0 ? 0 : offsetX;
		const int endY = imageYReach > 100 ? 100 : imageYReach;
		const int endX = imageXReach > 100 ? 100 : imageXReach;
		
		double imageY = (int)round(minImageY / scale);
		double imageX = (int)round(minImageX / scale);
		// loop trough image
		for (int i = startY; i < endY + 5; i++) {
			int pixelImageLocY = imageY + (int)round((imageYIncrement < 0 ? 0 : imageYIncrement) / scale);
			int finalPixelImageLocY = objHeightOriginal - 1 - pixelImageLocY;
			if (pixelImageLocY >= objHeightOriginal) continue;
			else {
				for (int i2 = startX; i2 < endX; i2++) {
					int pixelImageLocX = imageX + (int)round((imageXIncrement < 0 ? 0 : imageXIncrement) / scale);
					if (pixelImageLocX >= objWidthOriginal) continue;
					else {
						DrawPixel* pixel = pixelVec->at(finalPixelImageLocY)->at(pixelImageLocX);
						imageXIncrement++;

						// set pixel to chunk
						if (pixel != nullptr) {
							DrawPixel* oldPixel = getPixel(i2, i);
							if (oldPixel != nullptr) {
								if (oldPixel->getLayer() > layer) continue;
							}
							pixel->setLayer(layer);
							setPixel(i2, i, pixel);
						}
					}
				}
				imageXIncrement = - scale / 2;
				imageYIncrement++;
			}
		}
	}
	updateColors();
}
*/