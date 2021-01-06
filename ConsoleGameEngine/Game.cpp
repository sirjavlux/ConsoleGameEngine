/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////
* Here you can create objects and modify the game before the game starts by using the onEnable() function.
* You can also decide what will happen when the game ends, using the onDissable() function.
* For demonstration purposes, an example game is provided.
*//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cmath>
#include <sstream>

#include "SEngine.h"

// this function gets called before the game starts.
void onEnable(SEngine* engine) {
	//setup engine values
	engine->setPixelScale(10); // size on the pixels
	engine->setName("Example Starship Game"); // sets title of the console window
	engine->setTickSpeed(20); // how many times per second should events and such be called
	//add scene
	Scene* scene = new Scene("star_map_1", 0, 0);
	//set start scene and active
	setActiveStartScene(scene);

	//create color map
	//are used to assign colors to chars
	ImageColorMap * cMap = new ImageColorMap();
	cMap->setColor('W', RGB(203, 237, 255));
	cMap->setColor('H', RGB(0, 98, 209));
	cMap->setColor('T', RGB(0, 73, 156));
	cMap->setColor('D', RGB(0, 50, 114));
	cMap->setColor('G', RGB(0, 33, 70));
	cMap->setColor('B', RGB(0, 150, 238));
	cMap->setColor('R', RGB(255, 150, 238));

	//ship object
	GameObject* ship = new GameObject(-7, -5, 2, "ship", engine->getPixelScale());
	Image * shipImage = new Image(cMap);
	shipImage->addLine("       H       ");
	shipImage->addLine("       H       ");
	shipImage->addLine("       H       ");
	shipImage->addLine("       H       ");
	shipImage->addLine("      HTT      ");
	shipImage->addLine("      HTD      ");
	shipImage->addLine("      HTD      ");
	shipImage->addLine("   G  HTD  G   ");
	shipImage->addLine("   G HTHTD G   ");
	shipImage->addLine("   G HTWTD G   ");
	shipImage->addLine("   HHTHWTTTH   ");
	shipImage->addLine("HHHTTTHTDTTTTHH");
	shipImage->addLine("  B  HT HT  B  ");
	shipImage->addLine("     B   B     ");
	ship->updateImage(shipImage);
	ship->setRotation(0);
	ship->setMaxVelocity(20);
	registerGameObject(ship, scene);

	//set camera to follow this object
	engine->setCameraFollow(ship);
	//center camera on the middle of the object
	engine->setCameraFollowOffsetY((int) (ship->getHeight() * engine->getPixelScale() / 2));
	engine->setCameraFollowOffsetX((int) (ship->getWidth() * engine->getPixelScale() / 2));

	//create space background

	//place stars randomly based on predefined variables
	int starGenLocationX = -2000;
	int starGenLocationY = -2000;
	int starGenWidth = 4000;
	int starGenHeight = 12000;

	int smallStars = 50000;
	int mediumStars = 10000;
	int largeStars = 4000;

	//small stars
	Image * smallStarImage = new Image(cMap);
	smallStarImage->addLine("W");
	for (int i = 0; i < smallStars; i++) {

		int x = starGenLocationX + (std::rand() % (starGenWidth - starGenLocationX + 1));
		int y = starGenLocationY + (std::rand() % (starGenHeight - starGenLocationY + 1));

		std::stringstream stm;
		stm << "star_small_" << i;

		GameObject* smallStar = new GameObject(x, y, 0, stm.str());
		smallStar->updateImage(smallStarImage);
		registerGameObject(smallStar, scene);
	}

	//medium stars
	Image * mediumStarImage = new Image(cMap);
	mediumStarImage->addLine(" W ");
	mediumStarImage->addLine("WWW");
	mediumStarImage->addLine(" W ");
	for (int i = 0; i < mediumStars; i++) {

		int x = starGenLocationX + (std::rand() % (starGenWidth - starGenLocationX + 1));
		int y = starGenLocationY + (std::rand() % (starGenHeight - starGenLocationY + 1));

		std::stringstream stm;
		stm << "star_medium_" << i;

		GameObject* mediumStar = new GameObject(x, y, 0, stm.str());
		mediumStar->updateImage(mediumStarImage);
		registerGameObject(mediumStar, scene);
	}

	//large stars
	Image * largeStarImage = new Image(cMap);
	largeStarImage->addLine("  W  ");
	largeStarImage->addLine(" WWW ");
	largeStarImage->addLine("WWWWW");
	largeStarImage->addLine(" WWW ");
	largeStarImage->addLine("  W  ");
	for (int i = 0; i < largeStars; i++) {

		int x = starGenLocationX + (std::rand() % (starGenWidth - starGenLocationX + 1));
		int y = starGenLocationY + (std::rand() % (starGenHeight - starGenLocationY + 1));

		std::stringstream stm;
		stm << "star_large_" << i;

		GameObject* largeStar = new GameObject(x, y, 0, stm.str());
		largeStar->updateImage(largeStarImage);
		registerGameObject(largeStar, scene);
	}
}

// this function is called before the game shuts down.
void onDisable() {

}