#include <iostream>
#include <cmath>

#include "SEngine.h"

Vector2D::Vector2D(double x, double y, double multiplier) {
	Vector2D::x = x * multiplier;
	Vector2D::y = y * multiplier;
}

Vector2D::Vector2D(double x, double y) {
	Vector2D::x = x;
	Vector2D::y = y;
}

Vector2D::Vector2D() {
	x = 0;
	y = 0;
}

double Vector2D::getLenght() {
	return sqrt(pow(x, 2) + pow(y, 2));
}

double Vector2D::getX() {
	return x;
}

double Vector2D::getY() {
	return y;
}

void Vector2D::setX(double newX) {
	x = newX;
}

void Vector2D::setY(double newY) {
	y = newY;
}

void Vector2D::multiply(double amount) {
	x *= amount;
	y *= amount;
}

void Vector2D::normalize() {
	double lenght = getLenght();
	x /= lenght;
	y /= lenght;
}