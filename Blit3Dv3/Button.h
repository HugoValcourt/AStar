#include <iostream>
#include "Blit3DBaseFiles\Blit3D\Blit3D.h"
#include "Camera.h"

class Button
{
private:
	int x, y, width, height;
	std::string text;
	Sprite *bSprite;
	bool isOver = false;
	bool isActive = true;
	bool isVisible = true;
public:
	bool isClicked = false;
	Button(int x, int y, int width, int height, std::string text, Sprite *bSprite, bool isActive);
	void SetActive(bool active);
	void SetVisible(bool visible);
	void Update(float seconds, short leftClick, float mouseX, float mouseY);
	void Draw(AngelcodeFont *font);
};