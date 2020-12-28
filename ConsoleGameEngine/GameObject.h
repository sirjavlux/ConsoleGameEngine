#pragma once
#include <map>
#include <list>
#include <iostream>
#include "Image.h"

class GameObject
{
private:
	int x, y, l, h, w;
	std::string name;
	Image image;
public:
	GameObject(int xLoc, int yLoc, int layer, std::string name, Image image);
	GameObject(int xLoc, int yLoc, int layer, std::string name);
	GameObject(std::string name);
	GameObject();
	void teleport(int xLoc, int yLoc);
	std::string getName();
	void erase();
	int getX();
	int getY();
	int getLayer();
	int getHeight();
	int getWidth();
	void updateImage(Image image);
	Image getImage();
};

void registerGameObject(GameObject obj);
std::list<std::string> getObjects();
GameObject getGameObject(std::string name);