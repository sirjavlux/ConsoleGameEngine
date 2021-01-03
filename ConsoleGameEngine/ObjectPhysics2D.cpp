#include <iostream>

#include "SEngine.h"

ObjectPhysics2D::ObjectPhysics2D() {
	vector2d = Vector2D();
}

void ObjectPhysics2D::addForce(Vector2D velocity) {

}

Vector2D * ObjectPhysics2D::getVelocity() {
	return &vector2d;
}