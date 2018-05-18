#pragma once
#include <vector>
#include "Blit3D.h"
#include "Camera.h"

//////////////////////////////////////
//Author:  Hugo Valcourt
//Purpose: 2D map object and node object for A*
//Date:    Tuesday, April 10, 2018
//////////////////////////////////////

class Node
{
public:
	int x, y, spriteID;
	float g, h, f;//G = Cost until this node (Always gettin bigger) | H = estimated cost to goal | F = G + H
	Node *parent;
};

class Map
{
public:
	int width = 20;
	int height = 20;

	std::vector<int> map;
	std::vector<Sprite *> spriteList;

	Map();
	~Map();
	bool LoadMap(std::string filename);
	bool TileInScreen(Camera *camera, int x, int y);
	void Draw(Camera *camera);
};
