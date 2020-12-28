#pragma once
#include "GameObject.h"

void shutdown();
const int frameH = 48;
const int frameW = 96;
int getCameraX();
int getCameraY();
void teleportCamera(int x, int y);
void setCameraFollow(GameObject obj);
void cancelCameraFollow();
bool hasCameraObjectAttatched();
GameObject getCameraFollowObject();
void setCameraOffsetX(int amount);
void setCameraOffsetY(int amount);
bool isGameRunning();