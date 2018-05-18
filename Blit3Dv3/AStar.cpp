#include "AStar.h"
#include <algorithm>

//////////////////////////////////////
//Author:  Hugo Valcourt
//Purpose: Simple A* pathfinding algorithm and display
//Date:    Tuesday, April 10, 2018
//////////////////////////////////////

AStar::~AStar()
{
	ClearDisplayNodes();
	Reset(false);																		//Call the function Reset with "false". It means that the program is closing
}

AStar::AStar(int startX, int startY, int endX, int endY, float hLevel)
{
	heuristicLevel = hLevel;															//Set heuristic level
	
	this->startX = startX;
	this->startY = startY;
	this->endX = endX;
	this->endY = endY;

	startNode = new Node;																//Create a new node and set values
	startNode->x = this->startX;
	startNode->y = this->startY;
	startNode->g = 0.f;
	startNode->f = 0.f;
	startNode->h = 0.f;
	startNode->parent = NULL;
	startNode->spriteID = 11;


	endNode = new Node;																	//Create a new node and set values
	endNode->x = this->endX;
	endNode->y = this->endY;
	endNode->g = 0.f;
	endNode->f = 0.f;
	endNode->h = 0.f;
	endNode->parent = NULL;
	endNode->spriteID = 12;

	startNode->h = GoalEstimateDist(startNode);											//Calculate h for start node
	openList.push_back(startNode);														//Add the start node to the open list
}

void AStar::FindPath()
{
	//OPEN - the set of nodes to be evaluated
	//CLOSED - the set of nodes already evaluated

	//Loop (Currently set on the main update in main)
	//while (openList.size() > 0)
	{
		
		//current = node in OPEN with lowest f_cost then remove current from OPEN
		Node *currentNode;
		currentNode = openList.back();
		openList.pop_back();
		//add current to CLOSED
		currentX = currentNode->x;
		currentY = currentNode->y;

		//if current is the target node
		if (AtGoal(currentNode))
		{
			//	return and construct path
			path = ConstructPath(currentNode);
			Reset(true);
			pathFound = true;
			delete currentNode;
			return;
		}

		//foreach neighbour of the current node
		//	if neighbour is not traversable or neighbour is in CLOSED
		//		skip to the next neighbour
		//	
		//	if new path to neighbour is shorter OR neighbour is not in OPEN
		//	set f_cost of neighbour
		//	set parent of neighbour to current
		//	if neighbour is not in OPEN
		//		add neighbour to OPEN

		for (int dir = 0; dir < 8; dir++)												//For each 8 direction of the current node
		{
			if (CanMove(currentNode->x + offsetX[dir], currentNode->y + offsetY[dir]))  //Check if the neighbour is passable and in bounds
			{
				Node *n = NULL;
				n = new Node;															//Create a new neighbour
				n->x = currentNode->x + offsetX[dir];									//Set its x 
				n->y = currentNode->y + offsetY[dir];									//and y
				
				if(dir > 3)																//Check if we're currently checking in diagonal
					n->g = currentNode->g + map.map.at(n->x + n->y * map.width) * 1.5f;	//If so, it cost half more than normal
				else                                                                    //Else,
					n->g = currentNode->g + map.map.at(n->x + n->y * map.width);		//It's a normal cost
				n->spriteID = map.map.at(n->x + n->y * map.width);						//Set the node's sprite id to draw with
				n->h = GoalEstimateDist(n);												//Get minimal distance based on end node
				n->f = n->g + n->h;														//Add up g and h for its f
				n->parent = currentNode;												//Set its parent to the current checking node
				
				finalPathCost = n->f;													//For info and display purpose of the user

				if (IsNodeInOpenList(n))												//Check if the neighbour is already in the open list
				{
					if (n->g < openList.at(indexOpen)->g)								//If so, check if the neighbour's g is lower than the one in the open list
					{
						openList.at(indexOpen)->g = n->g;								//If so, update the node's g in the open list
						openList.at(indexOpen)->f = n->f;								//Update the node's f in the open list
						openList.at(indexOpen)->parent = currentNode;					//Update the node's parent in the open list for the current checking node
					}
					delete n;															//If the neighbour's g is not lower than the one in the list, delete it
				}
				else if (IsNodeInClosedList(n))											//If it's not in the open list and not in the closed list
				{
					if (n->g < closedList.at(indexClosed)->g)							//So, same thing as above, check its g and if its lower
					{
						closedList.at(indexClosed)->g = n->g;							//Update the node in the closed list
						closedList.at(indexClosed)->f = n->f;
						closedList.at(indexClosed)->parent = currentNode;
						openList.push_back(closedList.at(indexClosed));					//Put the node just updated in the open list
						closedList.erase(closedList.begin() + indexClosed);				//And remove it from the closed list	
					}
					delete n;															//Delete the neighbour node
				}
				else																	//If we're here, it means that the neighbour is in the closed list
					openList.push_back(n);												//Add it to the open list
			}
		}
			
		closedList.push_back(currentNode);												//Add the current node to the closed list, we're done with it
		std::sort(openList.begin(), openList.end(), [](Node *&a, Node *&b) {return a->f > b->f; });//Sort the open list based on f cost (lowest last)
	}

	//If no path found
	if (openList.size() <= 0)
	{
		std::cout << "No path found!" << std::endl;
		pathFound = false;
		finalPathCost = 0.f;
		Reset(false);
	}
}

bool AStar::OpenListGreaterThanZero()													
{
	if (openList.size() > 0) return true;												//If the open list is greater than 0, return true
	return false;																		//Else return false
}

bool AStar::CanMove(int x, int y)
{
	if (x >= 0 && y >= 0 && x < map.width && y < map.height && map.map.at(x + y * map.width) != 7)
		return true;																	//If we're in the map boundaries and its not a unpassable tile, return true
	return false;
}

int AStar::GoalEstimateDist(Node *currentNode)
{
	int tempX = std::abs(endNode->x - currentNode->x);									//Get the minimal cost to get to the end node (H in the node class)
	int tempY = std::abs(endNode->y - currentNode->y);

	return (tempX + tempY) * heuristicLevel;											//Scale it with heuristic and return it
}

std::vector<vec2> AStar::ConstructPath(Node *n)
{
	std::vector<vec2> p;																//Temp vector of vec2 to return
	vec2 tempV;																			//Temp vec2
	finalPathCost = n->f;																//Update the final path cost to display
	tempV.x = endX;																		//Add the end node x, y in the returning vector
	tempV.y = endY;
	p.push_back(tempV);
	while (n->parent != startNode)														//While the current node's parent is not hte start node
	{
		tempV.x = n->parent->x;															//Add the x, y of the parent's node to the returning vector	
		tempV.y = n->parent->y;
		p.push_back(tempV);
		n = n->parent;																	//Set the current node to its parent to create a loop															
	}
	tempV.x = startX;																	//Add the start node's x, y to the returning vector
	tempV.y = startY;
	p.push_back(tempV);
	return p;																			//Return the vector, this is the path
}

bool AStar::AtGoal(Node *currentNode)
{
	if (currentNode->x == endNode->x && currentNode->y == endNode->y)					//Are we at the end goal?
		return true;																	//If so return true
	return false;																		//Else return false
}

bool AStar::IsNodeInClosedList(Node *currentNode)
{
	int i = 0;
	for each (Node  *n in closedList)													//Check each node in the list
	{
		if (n->x == currentNode->x && n->y == currentNode->y)							//If the current checking node is the closed list, return true
		{
			indexClosed = i;															//We've found it, set an indexing reference used in the algorithm	
			return true;																//Return true 
		}
		i++;
	}
	return false;
}

bool AStar::IsNodeInOpenList(Node *currentNode)
{
	int i = 0;
	for each (Node  *n in openList)														//Check each node in the list
	{
		if (n->x == currentNode->x && n->y == currentNode->y)							//If the current checking node is the closed list, return true
		{
			indexOpen = i;																//We've found it, set an indexing reference used in the algorithm
			return true;																//Return true 
		}
		i++;
	}
	return false;
}

void AStar::Reset(bool isReset)
{
	openNodes.clear();																	//Clear both node display list
	closedNodes.clear();
	for (auto n : openList) { openNodes.push_back(vec2(n->x, n->y)); delete n; }		//Build the node display list and delete elements in the actual node list
	for (auto n : closedList) { closedNodes.push_back(vec2(n->x, n->y)); delete n; }	//Build the node display list and delete elements in the actual node list
	openList.clear();																	//Clear node list
	closedList.clear(); 																//Clear node list
		
	if(endNode->spriteID < 0) delete endNode;											//Check if node is not already deleted then delete it

	if (isReset)																		//If it is a reset to continue to use the application
	{
		startNode = new Node;															//Re-build the start and end node and push the start node in the open list
		endNode = new Node;																//for next path finding sequence
		startNode->x = startX;
		startNode->y = startY;
		startNode->spriteID = 11;
		startNode->g = 0.f;
		endNode->x = endX;
		endNode->y = endY;
		endNode->spriteID = 12;
		endNode->g = 0.f;
		openList.push_back(startNode);
	}
}

void AStar::SetStartNode(int x, int y)
{
	startX = x;																			//Set the x, y of the start node
	startY = y;
	startNode->x = startX;
	startNode->y = startY;
}

void AStar::SetEndNode(int x, int y)
{
	endX = x; 																			//Set the x, y of the end node
	endY = y;
	endNode->x = endX;
	endNode->y = endY;
}

void AStar::ClearDisplayNodes()
{
	path.clear();																		//Clear vector of vec2 used to display nodes
	openNodes.clear();
	closedNodes.clear();
}

void AStar::Draw(Camera *camera)
{
	if (openList.size() > 1)															//if the open list's size is greater than 1
	{
		for each (Node *n in openList)													//For each node in the list
		{
			if (map.TileInScreen(camera, n->x, n->y))									//Check if the node in the screen
				map.spriteList[7]->Blit(n->x * 16 - 8, -n->y * 16 - 8);					//If so, display it
		}
		for each (Node *n in closedList)												//For each node in the list
		{
			if (map.TileInScreen(camera, n->x, n->y))									//Check if the node in the screen
				map.spriteList[8]->Blit(n->x * 16 - 8, -n->y * 16 - 8);					//If so, display it
		}
	}
	else if(closedList.size() <= 0 && closedNodes.size() > 1)							//This means 'If the algorithm is done and there is node to display
	{
		for each (vec2 v in openNodes)													//For each node's x, y in the list											
		{
			if (map.TileInScreen(camera, v.x, v.y))										//Check if the node's x, y in the screen
				map.spriteList[7]->Blit(v.x * 16 - 8, -v.y * 16 - 8);					//If so, display it
		}
		for each (vec2 v in closedNodes)												//For each node's x, y in the list
		{
			if (map.TileInScreen(camera, v.x, v.y))										//Check if the node's x, y in the screen
				map.spriteList[8]->Blit(v.x * 16 - 8, -v.y * 16 - 8);					//If so, display it
		}
	}
	for each (vec2 v in path)															//For each node x, y of the path
	{
		if (map.TileInScreen(camera, v.x, v.y))											//Check if the node's x, y in the screen	
			map.spriteList[9]->Blit(v.x * 16 - 8, -v.y * 16 - 8);						//If so, display it
	}

	if (map.TileInScreen(camera, startX, startY))									//Check if the node's x, y in the screen (start node)
		map.spriteList[11]->Blit(startX * 16 - 8, -startY * 16 - 8);				//If so, display it
	if (map.TileInScreen(camera, endX, endY))										//Check if the node's x, y in the screen (end node)
		map.spriteList[12]->Blit(endX * 16 - 8, -endY * 16 - 8);					//If so, display it
	if(closedList.size() >= 1)
		if (map.TileInScreen(camera, currentX, currentY))							//Check if the node's x, y in the screen
			map.spriteList[10]->Blit(currentX * 16 - 8, -currentY * 16 - 8);		//If so, display it
}

void AStar::DrawT(AngelcodeFont *font)
{
	font->BlitText(20, 50, "Path cost: " + std::to_string(finalPathCost));			//Display the path cost
																					
	if (!pathFound && openList.size() < 1)											//If no path found
		font->BlitText((blit3D->screenWidth/2) - 100, (blit3D->screenHeight/2), "No path found!");//Display the that no path was found
}