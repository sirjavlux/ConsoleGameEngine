#include <iostream>
#include <exception>

#include "SEngine.h"

using namespace std;

const char* InvalidGameObjectIdentifier::what() const throw () {
	return "No gameobject with the specified identifier was found.";
}