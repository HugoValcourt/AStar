#pragma once
#include "Map.h"

//////////////////////////////////////
//Author:  Hugo Valcourt
//Purpose: Simple A* pathfinding
//Date:    Tuesday, April 10, 2018
//////////////////////////////////////

class AStar
{
private:
	Node *startNode;											//Where it starts
	Node *endNode;												//Where it ends

	int offsetX[8] = { -1, 1, 0, 0, -1, 1, -1, 1 };				//8 direction checking
	int offsetY[8] = { 0, 0, -1, 1, -1, 1, 1, -1 };				//''
	
	int indexOpen = 0;											//Index reference for the open list
	int indexClosed = 0;										//Index reference for the closed list
	int currentX, currentY;										//Display x, y for the current checking node
	int startX, startY, endX, endY;
	float heuristicLevel = 1.0f;								//Heuristic level multiplier
	float finalPathCost = 0.f;									//The final path cost

	std::vector<Node*> closedList;								//List of nodes that have been checked
	std::vector<Node*> openList;								//List of nodes that can or will be checked
	std::vector<vec2> path;										//The final path
	std::vector<vec2> openNodes;								//Open list nodes' x and y to display
	std::vector<vec2> closedNodes;								//Closed list nodes' x and y to display	
	
	bool IsNodeInClosedList(Node *currentNode);					//Function that looks if a node is at the same x, y of a node in the closed list
	bool IsNodeInOpenList(Node *currentNode);  					//Function that looks if a node is at the same x, y of a node in the open list
	bool CanMove(int x, int y);									//Function that checks if the current node's neighbour is not out of bound
	int GoalEstimateDist(Node *currentNode);					//Gets the minimal cost to get to the end node

	std::vector<vec2> ConstructPath(Node *n);					//Construct the path found to display

public:
	bool pathFound = false;										//Have we found a path yet?
	Map map;													//A reference to the map

	AStar(int startX, int startY, int endX, int endY, float hLevel);//Constructor for A*
	~AStar();													//Deconstuctor for A*
	void FindPath();											//The actual algorithm
	void Draw(Camera *camera);									//Draw nodes
	void DrawT(AngelcodeFont *font);							//Draw text info
	void Reset(bool isReset);									//Delete every nodes and reset for next pathfinding if needed
	void SetStartNode(int x, int y);							//Set the start node x and y
	void SetEndNode(int x, int y);  							//Set the end node x and y
	void ClearDisplayNodes();									//Clears the path 
	bool AtGoal(Node *currentNode);								//Have we reached the goal yet?
	bool OpenListGreaterThanZero();								//Is the openList greater than 0?
};

