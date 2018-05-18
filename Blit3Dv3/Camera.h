#pragma once

#include "Blit3D.h"
extern Blit3D *blit3D;

class Camera
{
private:
	int panX, panY, panZ; //use these to do pixel panning
	glm::vec3 moveDir; //Direction to pan
	float halfScreenWidth, halfScreenHeight; //Self-explanitory

public:
	float minX, minY, maxX, maxY, minZ, maxZ; //minimum and maximum offsets
	float panSpeed; // pixels per second
	glm::vec3 panOffset; //use this to pan the camera

	Camera();
	void Pan(float x, float y);
	void PanTo(float x, float y);
	void Update(float seconds);
	void Draw();
	void UnDraw();
};