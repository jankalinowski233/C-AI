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

	sf::RectangleShape shape;

public:
	Node();
	Node(int x, int y, int xSize, int ySize, int r, int col);
	int getXPos();
	int getYPos();
	int getRow();
	int getColumn();
	int getParent();
	float getF();
	float getG();
	float getH();
	bool isCurrent();
	bool isPath();
	bool lowerScore(const Node n) const;
	void resetColour();
	void setG(float newG);
	void setParent(int newParent);
	void setPath(bool newPath);
	void setWall();
	void setPath();
	void setGoal();
	void setCurrent(bool cont);
	void setOpen();
	void setClosed();
	void drawNode(sf::RenderTarget &target) const;
	void score(int goalX, int goalY);

};