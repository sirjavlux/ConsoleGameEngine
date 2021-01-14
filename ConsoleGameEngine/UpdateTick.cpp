#define _USE_MATH_DEFINES
#include <iostream>
#include <exception>
#include <cmath>

#include "SEngine.h"

//this updates once every tick
void UpdateTick(SEngine * engine) {

	/*
	//set rotation
	if (engine->hasCameraObjectAttatched()) {
		GameObject* obj = engine->getCameraFollowObject();
		Vector2D vec = obj->getVelocity();
		if (vec.getLenght() > 0) {
			vec.normalize();
			double degrees = atan2(vec.getY(), vec.getX()) * 180 / M_PI;
			obj->setRotation(degrees);
		}
	}
	*/
}