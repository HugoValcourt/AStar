#include "Camera.h"

Camera::Camera()
{
	panOffset = glm::vec3(0, 0, 0);
	minX = minY = minZ = 0;
	maxX = maxY = maxZ = 0;
	moveDir = glm::vec3(0, 0, 0);
	panSpeed = 1000;
	halfScreenWidth = blit3D->screenWidth / 2;
	halfScreenHeight = blit3D->screenHeight / 2;

	maxX = 1024 * 16;
	minX = 0;
	maxY = 0;
	minY = -1024 * 16;
}

void Camera::Pan(float x, float y)
{
	moveDir.x = x;
	moveDir.y = y;

	if (!(x == 0 && y == 0))
	{
		moveDir = glm::normalize(moveDir);
	}
}

void Camera::PanTo(float x, float y)
{
	panOffset.x = x;
	panOffset.y = y;
}

void Camera::Update(float seconds)
{
	panOffset += moveDir * panSpeed * seconds;

	//keeps the camera within the area specified
	if (panOffset.x < minX) panOffset.x = minX;
	if (panOffset.x > maxX) panOffset.x = maxX;
	if (panOffset.y < minY) panOffset.y = minY;
	if (panOffset.y > maxY) panOffset.y = maxY;
	if (panOffset.z < minZ) panOffset.z = minZ;
	if (panOffset.z > maxZ) panOffset.z = maxZ;
}

void Camera::Draw()
{
	//convert to pixels
	panX = halfScreenWidth - panOffset.x;
	panY = halfScreenHeight - panOffset.y;
	panZ = panOffset.z;

	blit3D->viewMatrix = glm::mat4(1.f); //identity matrix
	blit3D->viewMatrix = glm::translate(blit3D->viewMatrix, glm::vec3(panX, panY, panZ)); //translate it based on the offset
	blit3D->shader2d->setUniform("viewMatrix", blit3D->viewMatrix); //send the translated matrix to the shader
}

void Camera::UnDraw()
{
	blit3D->viewMatrix = glm::mat4(1.f); //identity matrix
	blit3D->shader2d->setUniform("viewMatrix", blit3D->viewMatrix); //send it to shader
}