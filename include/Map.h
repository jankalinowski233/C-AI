#pragma once

#include "Node.h"
#include <vector>
#include <list>
#include <iostream>
#include <algorithm>
#include <functional>

class Map
{
private:
	//Pointer to a node
	Node* n;
	//grid size
	static const unsigned int rows = 19;
	static const unsigned int columns = 26;
	//single node size
	int width = 30;
	int height = 30;
	//actual walkable world area
	const unsigned int worldWidth = 780;
	const unsigned int worldHeight = 570;
	//so the grid will not start on the wall
	int xOffset = 10;
	int yOffset = 5;

	//total amount of nodes
	static const unsigned int numberOfNodes = rows * columns;

public:
	Map();
	void notPath(int x, int y); //look for obstacles
	void setPath(int x, int y);
	void clearVisual();
	void updatePath(std::list<Node>& path, Node curr);
	void draw(sf::RenderTarget &target) const; //draw stuff
	bool Astar(std::list<Node>& path, Node s, Node g); //a star 
	std::vector<Node*> getNeighbour(Node* n); //returns neighbours of a node
	std::list<Node> constructPath(std::list<Node>& path, std::list<Node>& closed, Node *n); //constructs the path

	Node* mapGrid[rows][columns]; //array to represent nodes

	~Map();
};