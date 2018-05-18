#include "Map.h"

//////////////////////////////////////
//Author:  Hugo Valcourt
//Purpose: 2D map object
//Date:    Tuesday, April 10, 2018
//////////////////////////////////////

Map::Map()
{
	spriteList.push_back(blit3D->MakeSprite(0, 0, 16, 16, "Media\\tile1.png"));//Load tile sprites
	spriteList.push_back(blit3D->MakeSprite(0, 0, 16, 16, "Media\\tile2.png"));
	spriteList.push_back(blit3D->MakeSprite(0, 0, 16, 16, "Media\\tile3.png"));
	spriteList.push_back(blit3D->MakeSprite(0, 0, 16, 16, "Media\\tile4.png"));
	spriteList.push_back(blit3D->MakeSprite(0, 0, 16, 16, "Media\\tile5.png"));
	spriteList.push_back(blit3D->MakeSprite(0, 0, 16, 16, "Media\\tile6.png"));
	spriteList.push_back(blit3D->MakeSprite(0, 0, 16, 16, "Media\\tile7.png"));
	spriteList.push_back(blit3D->MakeSprite(0, 0, 16, 16, "Media\\openTile.png"));
	spriteList.push_back(blit3D->MakeSprite(0, 0, 16, 16, "Media\\closedTile.png"));
	spriteList.push_back(blit3D->MakeSprite(0, 0, 16, 16, "Media\\pathTile.png"));
	spriteList.push_back(blit3D->MakeSprite(0, 0, 16, 16, "Media\\currentTile.png"));
	spriteList.push_back(blit3D->MakeSprite(0, 0, 16, 16, "Media\\startTile.png"));
	spriteList.push_back(blit3D->MakeSprite(0, 0, 16, 16, "Media\\endTile.png"));
}

Map::~Map()
{

}

bool Map::LoadMap(std::string filename)
{
	int x = 0, y = 0;
	std::ifstream myfile(filename);									// open the file
	int nextInt;													// declare variable to store a character from text file
	if (myfile.is_open())											//If the file was found
	{
		myfile >> width;
		myfile >> height;
		
		while (myfile >> nextInt)									// while there is a line of text to get
		{
			map.push_back(nextInt);							//Set the int at x,y to the int in the file
		}
		myfile.close();												// close the file
	}
	else
	{
		//Print error to console if there is an issue opening file
		std::cout << "Unable to open file!";
		std::cout << std::endl;
	}

	return true;
}

bool Map::TileInScreen(Camera *camera, int x, int y)
{
	if (x * 16 >= camera->panOffset.x - (blit3D->screenWidth / 2) + 32 && x * 16 <= camera->panOffset.x + (blit3D->screenWidth / 2) - 32 &&
		-y * 16 <= camera->panOffset.y + (blit3D->screenHeight / 2) + 32 && -y * 16 >= camera->panOffset.y - (blit3D->screenHeight / 2) - 32)
		return true;
	return false;
}

void Map::Draw(Camera *camera)
{
	int tileDrawX = 0;												//Local int for double loop
	int tileDrawY = 0;												//local int for double loop
	
	for (tileDrawY = 0; tileDrawY < width; ++tileDrawY)				//loop over our map and draw it
	{
		for (tileDrawX = 0; tileDrawX < height; ++tileDrawX)
		{
			if(TileInScreen(camera, tileDrawX, tileDrawY))
				spriteList.at(map.at(tileDrawX + tileDrawY * width) - 1)->Blit(tileDrawX * 16 - 8, -tileDrawY * 16 - 8);	//Only draw the tile that are visible on the screen
		}
	}
}