#include "Button.h"

//////////////////////////////////////
//Author:  Hugo Valcourt
//Purpose: Button object 
//Date:    Tuesday, April 10, 2018
//////////////////////////////////////

Button::Button(int x, int y, int width, int height, std::string text, Sprite *bSprite, bool isActive)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->text = text;
	this->bSprite = bSprite;
	this->isActive = isActive;
}

void Button::Update(float seconds, short leftClick, float mouseX, float mouseY)
{
	if (isActive)
	{
		if (mouseX >= x - width * 2 &&
			mouseX <= x + width * 2 &&
			mouseY >= y - height * 2 &&
			mouseY <= y + height * 2)
		{
			isOver = true;
			if (leftClick == 1)
			{
				std::cout << "Button clicked!" << std::endl;
				isClicked = true;
			}
			else
				isClicked = false;
		}
		else
		{
			isClicked = false;
			isOver = false;
		}
	}
}

void Button::SetActive(bool active)
{
	isActive = active;
	if (active)
		isVisible = active;
}

void Button::SetVisible(bool visible)
{
	isVisible = visible;
	if (!visible)
		isActive = visible;
}

void Button::Draw(AngelcodeFont *font)
{
	if (isActive)
	{
		if (isOver && !isClicked)
			bSprite->Blit(x, y, width / 20, height / 10, 0.75f);
		else
			bSprite->Blit(x, y, width / 20, height / 10, 1.f);
	}
	else if(isVisible && !isActive)
		bSprite->Blit(x, y, width / 20, height / 10, 0.5f);

	font->BlitText(x - width * 1.3f, y + height, text);														//Display it
}