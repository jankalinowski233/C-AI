#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>

class Node
{
private:
	//size of the node
	int sizeX; 
	int sizeY; 
	//position of the node
	int posX;
	int posY;

	//amount of rows and columns
	int row;
	int column;

	bool currentNode; //current tank pos
	bool path = true; //can it be a path?

	int parent;
	float f; //total cost of the node
	float g; //current distance to start
	float h; //heuristic distance to goal

	//node's shape
	sf::RectangleShape shape;

public:
	Node(); //!< Default constructor
	Node(int x, int y, int xSize, int ySize, int r, int col); //!< Custom constructor
	int getXPos(); //!< Get node's X pos
	int getYPos(); //!< Get node's Y pos
	int getRow(); //!< Get node's row
	int getColumn(); //!< Get node's column
	int getParent(); //!< Get node's parent
	float getF(); //!< Get F score
	float getG(); //!< Get G score
	float getH(); //!< Get H score
	bool isCurrent(); //!< Is the node current node
	bool isPath(); //!< Is the node a path
	bool lowerScore(const Node n) const; //!< Compare scores of 2 nodes
	void resetColour(); //!< Reset colours
	void setG(float newG); //!< Set G score
	void setParent(int newParent); //!< Set parent score
	void setPath(bool newPath); //!< Set path
	void setWall(); //!< Set wall
	void setPath(); //!< Set path colour
	void setGoal(); //!< Set goal colour
	void setCurrent(bool cont); //!< Set current colour
	void setOpen(); //!< Set open node colour
	void setClosed(); //!< Set closed node colour
	void drawNode(sf::RenderTarget &target) const; //!< Draw node
	void score(int goalX, int goalY); //!< Calculate score

};