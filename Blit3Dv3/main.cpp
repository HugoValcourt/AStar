/*
	Simple example of loading/rotating/displaying sprites in Blit3D
*/
//memory leak detection
#define CRTDBG_MAP_ALLOC
#ifdef _DEBUG
	#ifndef DBG_NEW
		#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
		#define new DBG_NEW
	#endif
#endif  // _DEBUG

#include <stdlib.h>
#include <crtdbg.h>

#include "Blit3D.h"
#include "AStar.h"
#include "Camera.h"
#include "Map.h"
#include "Button.h"

//////////////////////////////////////
//Author:  Hugo Valcourt
//Purpose: Simple A* pathfinding UI and functionality handling
//Date:    Tuesday, April 10, 2018
//////////////////////////////////////

//GLOBAL DATA
Blit3D *blit3D = NULL;
Sprite *highlight = NULL;																//Object pointer
Sprite *editTile = NULL;																//Object pointer									
Sprite *placingMarker = NULL;
AStar *astar = NULL;
Camera *camera = NULL;																	//Object pointer
AngelcodeFont *afont = NULL;

Button *startButton = NULL;																//UI buttons
Button *resetButton = NULL;																//UI buttons
Button *pauseButton = NULL;																//UI buttons
Button *quitButton = NULL;																//UI buttons
Button *placeButton = NULL;																//UI buttons
Button *button100 = NULL;																//UI buttons
Button *button60 = NULL;																//UI buttons
Button *button30 = NULL;																//UI buttons
Button *button10 = NULL;																//UI buttons
Button *button5 = NULL;																	//UI buttons
Button *editorButton = NULL;															//UI buttons
Button *pathFindingButton = NULL;														//UI buttons
Button *loadMapButton = NULL;															//UI buttons
Button *emptyMapButton = NULL;															//UI buttons
Button *map1Button = NULL;																//UI buttons
Button *map2Button = NULL;																//UI buttons
Button *map3Button = NULL;																//UI buttons
Button *loadMapCancelButton = NULL;														//UI buttons

std::vector<Button*> buttons;															//Vector that keeps the buttons

int tileX = 0;																			//Variable to get the tile
int tileY = 0;																			//Variable to get the tile
int tileNum = 0;																		//Tile under cursor
int leftMouseCounter = 0;
int tileCostPlacer = 0;

float cursor_x = 0;																		//mouse cursor
float cursor_y = 0;																		//mouse cursor
float mouseX = 0;																		//Real mouse cursor
float mouseY = 0;																		//Real mouse cursor
float timeElapsed = 0.f;																//Time since last update
float updatePerSeconds = 100.f;															//How many updates per seconds?

bool leftMouseHold = false;																//While we are holding left mouse button
bool rightMouseHold = false;															//While we are holding right mouse button
bool editingMap = false;																//While we are editing the map
bool astarStarted = false;																//Is the astar pathfinding started?
bool isPlacer = false;																	//Are we placing start and end node?
bool startNodePlaced = true;															//Did we placed the start node?
bool endNodePlaced = true;																//Did we placed the end node?
bool placingStartNode = false;															//Are we currently placing the start node

enum State { PATHFINDING, MAPEDITOR, LOADMAP };											//State of software : Editing map OR Astar pathfinding OR Loading a map
State state = PATHFINDING;

void ButtonUpdate(float seconds);														//Button updates
void UpdateState();																		//If we change state, update it
void UpdateTileOffsetCursor();															//If we're placing nodes or editing the map
void LoadButtons();
void ClearAndLoadMap(std::string filename);

void Init()//Initialization
{
	//Angelcode font
	afont = blit3D->MakeAngelcodeFontFromBinary32("whiteBlackStroke.bin");				//Load font

	astar = new AStar(0, 0, 1, 0, 1.f);													//Instantiante a new astar object
	astar->map.LoadMap("Media\\Map1.txt");												//Load a default map 100x100

	camera = new Camera();																//Make a new Camera object
	camera->PanTo(500, -325);															//Pan the camera to an offset

	LoadButtons();
	
	highlight = blit3D->MakeSprite(0, 0, 16, 16, "Media\\highlight.png");				//load the tile highlight sprite
	editTile = astar->map.spriteList.at(6);
	tileCostPlacer = 6;
}

void DeInit(void)
{	
	for (auto b : buttons) delete b;													//Delete buttons 
	if (astar != NULL) delete astar;													//Delete AStar objecy
	if (camera != NULL) delete camera;													//Delete the camera
}

void Update(double seconds)
{
	camera->Update((float)seconds);														//Update the camera
	timeElapsed += seconds;																//Update time elapsed for each update

	if (leftMouseHold)																	//This is for a left click on the mouse
		leftMouseCounter++;																//and not a hold


	if (astarStarted && timeElapsed >= 1.0f / updatePerSeconds)							//If astar is active and it's time to update based on the update rate
	{
		timeElapsed = 0.f;																//Reset timer
		if (astar->OpenListGreaterThanZero() && !astar->pathFound)						//If we still not have found path
			astar->FindPath();															//Continue to find the path
	}

	if(isPlacer || editingMap)															//If we're editing the map or placing start and end node
		UpdateTileOffsetCursor();
	ButtonUpdate((float)seconds);														//Update every buttons
}

void Draw(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	//clear colour: r,g,b,a 	
											// wipe the drawing surface clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//draw stuff here

	camera->Draw();																		//Draw the camera

	astar->map.Draw(camera);															//Draw the map

	switch (state)
	{
	case PATHFINDING:
		astar->Draw(camera);															//If we're in pathfinding mode, draw the astar nodes/process
		if (isPlacer)
		{
			if (cursor_x < astar->map.width * 16 && cursor_y < astar->map.height * 16 && cursor_x >= 0 && cursor_y >= 0)//If the tile is in the bounds of the map
				placingMarker->Blit((((int)cursor_x / 16) * 16 + 8) - 16, ((-(int)cursor_y / 16) * 16 - 8));//snaps the highlight sprite over the selectable tile
		}
		break;
	case MAPEDITOR:
		if (editingMap)
		{
			if (cursor_x < astar->map.width * 16 && cursor_y < astar->map.height * 16 && cursor_x >= 0 && cursor_y >= 0)//If the tile is in the bounds of the map
			{
				editTile->Blit((((int)cursor_x / 16) * 16 + 8) - 16, ((-(int)cursor_y / 16) * 16 - 8));	//snaps the start or end node sprite over the selectable tile
				highlight->Blit((((int)cursor_x / 16) * 16 + 8) - 16, ((-(int)cursor_y / 16) * 16 - 8));//snaps the start or end node sprite over the selectable tile
			}
		}
		break;
	}

	camera->UnDraw();																//Undraw the camera

	for each(Button *b in buttons)													//Draw every buttons
		b->Draw(afont);
	
	switch (state)
	{
	case PATHFINDING:
		afont->BlitText(875, 50, "Update rate: " + std::to_string((int)updatePerSeconds));//Display text
		astar->DrawT(afont);														//Draw font for Astar
		break;
	case MAPEDITOR:
		afont->BlitText(20, 650, "Y:" + std::to_string((int)(cursor_y / 16)));		//Display text
		afont->BlitText(20, 570, "X:" + std::to_string((int)(1 + (cursor_x / 16))));//Display text
		afont->BlitText(20, 100, "Choose tile cost with number 1 - 7");				//Display text
		break;
	}
}

//////////////////////////////////////
//Author:  Hugo Valcourt
//Purpose: Load and create buttons
//Date:    Tuesday, April 10, 2018
//////////////////////////////////////
void LoadButtons()
{
	startButton = new Button(60, 600, 25, 12, "", blit3D->MakeSprite(0, 0, 100, 50, "Media\\startButton.png"), true);//Make buttons for UI
	pauseButton = new Button(60, 540, 25, 12, "", blit3D->MakeSprite(0, 0, 100, 50, "Media\\pauseButton.png"), true);
	resetButton = new Button(60, 480, 25, 12, "", blit3D->MakeSprite(0, 0, 100, 50, "Media\\resetButton.png"), true);
	placeButton = new Button(60, 420, 25, 12, "", blit3D->MakeSprite(0, 0, 100, 50, "Media\\placeButton.png"), true);
	quitButton = new Button(60, 180, 25, 12, "", blit3D->MakeSprite(0, 0, 100, 50, "Media\\quitButton.png"), true);
	button100 = new Button(blit3D->screenWidth - 60, 600, 25, 12, "", blit3D->MakeSprite(0, 0, 100, 50, "Media\\100Button.png"), true);
	button60 = new Button(blit3D->screenWidth - 60, 540, 25, 12, "", blit3D->MakeSprite(0, 0, 100, 50, "Media\\60Button.png"), true);
	button30 = new Button(blit3D->screenWidth - 60, 480, 25, 12, "", blit3D->MakeSprite(0, 0, 100, 50, "Media\\30Button.png"), true);
	button10 = new Button(blit3D->screenWidth - 60, 420, 25, 12, "", blit3D->MakeSprite(0, 0, 100, 50, "Media\\10Button.png"), true);
	button5 = new Button(blit3D->screenWidth - 60, 360, 25, 12, "", blit3D->MakeSprite(0, 0, 100, 50, "Media\\5Button.png"), true);
	editorButton = new Button(60, 360, 25, 12, "", blit3D->MakeSprite(0, 0, 100, 50, "Media\\editButton.png"), true);
	pathFindingButton = new Button(60, 360, 25, 12, "", blit3D->MakeSprite(0, 0, 100, 50, "Media\\okButton.png"), true);
	loadMapButton = new Button(60, 300, 25, 12, "", blit3D->MakeSprite(0, 0, 100, 50, "Media\\loadMapButton.png"), true);
	emptyMapButton = new Button(60, 600, 25, 12, "", blit3D->MakeSprite(0, 0, 100, 50, "Media\\emptyMapButton.png"), true);
	map1Button = new Button(60, 540, 25, 12, "", blit3D->MakeSprite(0, 0, 100, 50, "Media\\map1Button.png"), true);
	map2Button = new Button(60, 480, 25, 12, "", blit3D->MakeSprite(0, 0, 100, 50, "Media\\map2Button.png"), true);
	map3Button = new Button(60, 420, 25, 12, "", blit3D->MakeSprite(0, 0, 100, 50, "Media\\map3Button.png"), true);
	loadMapCancelButton = new Button(60, 360, 25, 12, "", blit3D->MakeSprite(0, 0, 100, 50, "Media\\cancelButton.png"), true);
	
	pathFindingButton->SetVisible(false);
	emptyMapButton->SetVisible(false);
	map1Button->SetVisible(false);
	map2Button->SetVisible(false);
	map3Button->SetVisible(false);
	loadMapCancelButton->SetVisible(false);

	buttons.push_back(quitButton);													//Push back buttons into a vector of buttons
	buttons.push_back(startButton);
	buttons.push_back(pauseButton);
	buttons.push_back(resetButton);
	buttons.push_back(placeButton);
	buttons.push_back(button100);
	buttons.push_back(button60);
	buttons.push_back(button30);
	buttons.push_back(button10);
	buttons.push_back(button5);
	buttons.push_back(editorButton);
	buttons.push_back(pathFindingButton);
	buttons.push_back(loadMapButton);
	buttons.push_back(emptyMapButton);
	buttons.push_back(map1Button);
	buttons.push_back(map2Button);
	buttons.push_back(map3Button);
	buttons.push_back(loadMapCancelButton);
}

//////////////////////////////////////
//Author:  Hugo Valcourt
//Purpose: Clear and load a new map
//Date:    Tuesday, April 10, 2018
//////////////////////////////////////
void ClearAndLoadMap(std::string filename)
{
	astar->map.map.clear();
	astar->Reset(true);
	astar->map.LoadMap(filename);
	state = PATHFINDING;
	UpdateState();
}

//////////////////////////////////////
//Author:  Hugo Valcourt
//Purpose: Updates every button
//Date:    Tuesday, April 10, 2018
//////////////////////////////////////
void ButtonUpdate(float seconds)
{
	for each(Button *b in buttons)
		b->Update((float)seconds, leftMouseCounter, mouseX, mouseY);					//Update every buttons in the vector

	switch (state)
	{
	case PATHFINDING:
		if (startButton->isClicked)														//If the start button is clicked
		{
			if(astar->pathFound)
				astar->ClearDisplayNodes();
			astarStarted = true;														//Start pathfinding	
			astar->pathFound = false;
			startButton->isClicked = false;
		}
		if (pauseButton->isClicked)														//If the pause button is clicked
		{
			astarStarted = false;														//Pause pathfinding
			pauseButton->isClicked = false;
		}
		if (resetButton->isClicked)														//If the reset button is clicked
		{
			astarStarted = false;														//Stop pathfinding
			astar->Reset(true);															//Delete nodes anf reset everything
			astar->ClearDisplayNodes();													//Clear the vectors to display
			resetButton->isClicked = false;
		}

		if (placeButton->isClicked)														//If the place button is clicked
		{
			placingStartNode = true;													//We're currently placing the start node
			startNodePlaced = false;													//Start node is not placed
			endNodePlaced = false;														//End node is not placed
			astarStarted = false;														//Stop pathfinding
			astar->Reset(true);															//Delete nodes anf reset everything
			astar->ClearDisplayNodes();													//Clear the vectors to display
			startButton->SetActive(false);												//The start button is disable while placing start and end node
			isPlacer = true;															//We're placing start and end node
			placingMarker = astar->map.spriteList[11];									//Set the placing sprite to the start node sprite	
			placeButton->isClicked = false;
		}
		for (int i = 5; i < 10; i++)													//For each update rate buttons
		{
			if (buttons.at(i)->isClicked)												//Check if it have been clicked
			{
				switch (i)																//Switch on the button
				{
				case 5: updatePerSeconds = 100.f; break;								//The 100 button
				case 6: updatePerSeconds = 60.f; break;									//The 60 button
				case 7: updatePerSeconds = 30.f; break;									//The 30 button
				case 8: updatePerSeconds = 10.f; break;									//The 10 button
				case 9: updatePerSeconds = 5.f;	break;									//The 5 button
				default: updatePerSeconds = 100.f; break;
				}
			}
		}
		if (loadMapButton->isClicked)													//If the load map option is clicked
		{
			state = LOADMAP;															//Change the mode to loading map
			UpdateState();																//Update state
			loadMapButton->isClicked = false;											//Set the button clicked bool
		}
		if (editorButton->isClicked && !pathFindingButton->isClicked && !loadMapCancelButton->isClicked)//If the edit button is clicked
		{
			state = MAPEDITOR;															//Change the state
			UpdateState();																//Update the state
			editorButton->isClicked = false;											//Set the button clicked bool
		}
		break;

	case MAPEDITOR:
		if (pathFindingButton->isClicked && !editorButton->isClicked)					//If the "OK button is clicked		
		{
			state = PATHFINDING;														//Change the state
			UpdateState();																//Update the state
			pathFindingButton->isClicked = false;										//Set the button clicked bool
		}
		break;

	case LOADMAP:
		for (int i = 13; i <= 16; i++)													//For each load map buttons
		{
			if (buttons.at(i)->isClicked)												//Check if it have been clicked
			{
				astar->SetStartNode(0, 0);												//Reset start node
				astar->SetEndNode(1, 0);  												//Reset end node
				switch (i)																//Switch on the button
				{
				case 13: ClearAndLoadMap("Media\\EmptyMap.txt"); break;					//Load corresponding maps
				case 14: ClearAndLoadMap("Media\\Map1.txt"); break;
				case 15: ClearAndLoadMap("Media\\Map2.txt"); break;
				case 16: ClearAndLoadMap("Media\\Map3.txt"); break;
				}
			}
		}

		if (loadMapCancelButton->isClicked && !editorButton->isClicked)					//If the cancel button is hit (for loading map option)
		{
			state = PATHFINDING;														//Change the mode to loading map
			UpdateState();																//Update state
			loadMapCancelButton->isClicked = false;										//Set the button clicked bool
		}
		break;
	}

	if (quitButton->isClicked)															//If quit button is clicked
		blit3D->Quit();																	//Exit the program
}

//////////////////////////////////////
//Author:  Hugo Valcourt
//Purpose: Update the state if we change it
//Date:    Tuesday, April 10, 2018
//////////////////////////////////////
void UpdateState()
{
	switch (state)
	{
	case PATHFINDING:																	//If we're in pathfinding mode
		for each(Button *b in buttons)													//Set every button to active
			b->SetActive(true);
		pathFindingButton->SetVisible(false);											//Deactivate the "OK" button
		emptyMapButton->SetVisible(false);												//Deactivate the emptyMap button
		map1Button->SetVisible(false);													//Deactivate the map1 button
		map2Button->SetVisible(false);													//Deactivate the map2 button
		map3Button->SetVisible(false);													//Deactivate the map3 button
		loadMapCancelButton->SetVisible(false);											//Set the cancel button invisible
		isPlacer = false;																//Reset placing start and end node to not placing
		break;

	case MAPEDITOR:																		//If we're in map editor mode
		for each(Button *b in buttons)													//Set every button to active	
			b->SetVisible(false);				
		astarStarted = false;															//Astar pathfinding is stopped
		astar->Reset(true);																//Reset stuff for Astar
		astar->ClearDisplayNodes();														//Clear the vectors to display
		quitButton->SetActive(true);													//Activate the quit button
		editorButton->SetActive(false);													//Deactivate the editor button
		pathFindingButton->SetActive(true);												//Activate the "OK" button					
		editingMap = true;																//We're editing the map
		isPlacer = false;																//Reset placing start and end node to not placing
		break;
	
	case LOADMAP:
		for each(Button *b in buttons)													//Set every button to active	
			b->SetVisible(false);
		astarStarted = false;															//Astar pathfinding is stopped
		astar->Reset(true);																//Reset stuff for Astar
		astar->ClearDisplayNodes();														//Clear the vectors to display
		quitButton->SetActive(true);													//Activate the quit button
		loadMapCancelButton->SetActive(true);											//Activate the cancel button
		emptyMapButton->SetActive(true);												//Activate the emptyMap button
		map1Button->SetActive(true);													//Activate the load map 1 button
		map2Button->SetActive(true);													//Activate the load map 2 button
		map3Button->SetActive(true);													//Activate the load map 3 button
		isPlacer = false;																//Reset placing start and end node to not placing
		break;
	}
}

//////////////////////////////////////
//Author:  Hugo Valcourt
//Purpose: If we're placing a tile/node
//Date:    Tuesday, April 10, 2018
//////////////////////////////////////
void UpdateTileOffsetCursor()
{
	if (editingMap || isPlacer)
	{
		if (cursor_x < astar->map.width * 16 && cursor_y < astar->map.height * 16 && cursor_x >= 0.f && cursor_y >= 0.f)	//If we are in the screen bound
		{
			tileX = cursor_x / 16;														//Get the x coords of the tile
			tileY = cursor_y / 16;														//Get the y coords of the tile
			tileNum = tileY * astar->map.width + tileX;									//Do math to get the tile under the cursor
			
			switch (state)
			{
			case PATHFINDING:
				if (isPlacer && leftMouseCounter == 1)									//If we're placing start and end node and the left mouse is CLICKED
				{
					if (!placingStartNode)												//If the end node has node been placed
					{
						astar->SetEndNode(tileX, tileY);								//Set the end node to the cursor position based on tile
						endNodePlaced = true;											//End node has been placed
					}
					if (placingStartNode)												//If the start node has node been placed
					{
						astar->SetStartNode(tileX, tileY);								//Set the start node to the cursor position based on tile
						placingMarker = astar->map.spriteList[12];						//Set the placing sprite to the end node sprite
						startNodePlaced = true;											//Start node has been placed
						placingStartNode = false;										//We're now placing the end node
					}
					if (startNodePlaced && endNodePlaced)								//Is both start and end node placed?
					{
						isPlacer = false;												//We're not placing anymore
						startButton->SetActive(true);									//And can now start the pathfinding sequence
					}
				}
				break;

			case MAPEDITOR:
				if (leftMouseHold)														//If we're editing the map and holding the left mouse button
					astar->map.map.at(tileX + tileY * astar->map.width) = tileCostPlacer + 1;			//Change the map's tile at x and y 
				break;
			}
		}
	}
}

//the key codes/actions/mods for DoInput are from GLFW: check its documentation for their values
void DoInput(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		blit3D->Quit();
																						//the user can move the camera around
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		camera->Pan(-1, 0);																//Pan left

	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		camera->Pan(1, 0);																//Pan right

	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		camera->Pan(0, -1);																//Pan down

	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		camera->Pan(0, 1);																//Pan up

	if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE)									//Stop the panning
		camera->Pan(0, 0);

	if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE)
		camera->Pan(0, 0);

	if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE)
		camera->Pan(0, 0);

	if (key == GLFW_KEY_UP && action == GLFW_RELEASE)
		camera->Pan(0, 0);


	switch (state)
	{
	case PATHFINDING:
		if (key == GLFW_KEY_F2 && action == GLFW_PRESS)									//Fine tune the update rate for astar
			updatePerSeconds++;
		if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
		{
			updatePerSeconds--;
			if (updatePerSeconds <= 0)
				updatePerSeconds = 0.f;
		}
		break;

	case MAPEDITOR:
		if (key == GLFW_KEY_1 && action == GLFW_PRESS)									//While editing, select the tile to place
			tileCostPlacer = 0;
		if (key == GLFW_KEY_2 && action == GLFW_PRESS)
			tileCostPlacer = 1;
		if (key == GLFW_KEY_3 && action == GLFW_PRESS)
			tileCostPlacer = 2;
		if (key == GLFW_KEY_4 && action == GLFW_PRESS)
			tileCostPlacer = 3;
		if (key == GLFW_KEY_5 && action == GLFW_PRESS)
			tileCostPlacer = 4;
		if (key == GLFW_KEY_6 && action == GLFW_PRESS)
			tileCostPlacer = 5;
		if (key == GLFW_KEY_7 && action == GLFW_PRESS)
			tileCostPlacer = 6;

		editTile = astar->map.spriteList.at(tileCostPlacer);							//Set the sprite to the tile just selected
		break;
	}
}


void DoCursor(double x, double y)
{
	//convert values in case we are in a "fake" fullscreen,
	//becuase the OS reports mouse positions based on ACTUAL screen resolution.
	cursor_x = blit3D->screenWidth / blit3D->trueScreenWidth * (float)x;				//Convert the x and y mouse cursor coords for real usage
	cursor_y = blit3D->screenHeight / blit3D->trueScreenHeight * (float)y;

	cursor_x += camera->panOffset.x - blit3D->screenWidth / 2;							//Convert the x and y mouse cursor coords when the camera is panned
	cursor_y -= camera->panOffset.y - blit3D->screenHeight / 2;

	cursor_x = ((cursor_x / 16) + 1) * 16;												//Convert the x and y mouse coords to 16x16 tile
	cursor_y = (cursor_y / 16) * 16;
	cursor_y -= 45.f * 16;

	mouseX = blit3D->screenWidth / blit3D->trueScreenWidth * (float)x;					//Convert the x and y mouse cursor coords for real usage
	mouseY = blit3D->screenHeight / blit3D->trueScreenHeight * (float)y;
	mouseY = blit3D->screenHeight - mouseY;
}

void DoMouseButton(int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		leftMouseHold = true;															//Left mouse button hold
		if (leftMouseCounter >= 1)														//If we only want a click and not a hold
			leftMouseHold = false;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		leftMouseHold = false;
		leftMouseCounter = 0;
	}
}

int main(int argc, char *argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);						//memory leak detection

	blit3D = new Blit3D(Blit3DWindowModel::DECORATEDWINDOW, 1280, 720);					//Creates a 1920x1080 window
	//_crtBreakAlloc = 2589;
																						//set our callback funcs
	blit3D->SetInit(Init);
	blit3D->SetDeInit(DeInit);
	blit3D->SetUpdate(Update);
	blit3D->SetDraw(Draw);
	blit3D->SetDoInput(DoInput);
	blit3D->SetDoCursor(DoCursor);
	blit3D->SetDoMouseButton(DoMouseButton);

	//Run() blocks until the window is closed
	blit3D->Run(Blit3DThreadModel::SINGLETHREADED);
	if (blit3D) delete blit3D;															//delete the pointer when we shutdown
}