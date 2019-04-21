#include "..\include\Node.h"

Node::Node()
{
}

Node::Node(int x, int y, int xSize, int ySize, int r, int col)
{
	//create a new node
	posX = x;
	posY = y;
	sizeX = xSize;
	sizeY = ySize;
	row = r;
	column = col;
	currentNode = false;

	//give it a rectangle shape
	shape.setSize(sf::Vector2f(sizeX, sizeY));
	shape.setOrigin(sizeX / 2, sizeY / 2);
	shape.setPosition(sf::Vector2f(posX + sizeX/2, posY+sizeY/2));
	shape.setFillColor(sf::Color(255, 255, 255, 50));
	shape.setOutlineThickness(1);
	shape.setOutlineColor(sf::Color(255, 255, 255, 100));
}

int Node::getXPos()
{
	//get the centre position on X axis
	return posX + sizeX / 2;
}

int Node::getYPos()
{
	//get the centre position on Y axis
	return posY + sizeY / 2;
}

int Node::getRow()
{
	//get row
	return row;
}

int Node::getColumn()
{
	//get column
	return column;
}

int Node::getParent()
{
	//get parent
	return parent;
}

float Node::getF()
{
	//get F
	return f;
}

float Node::getG()
{
	//get G
	return g;
}

float Node::getH()
{
	//get H
	return h;
}

bool Node::isCurrent()
{
	//get current node
	return currentNode;
}

bool Node::isPath()
{
	//get path
	return path;
}

void Node::setG(float newG)
{
	//set G
	g = newG;
}

void Node::setParent(int newParent)
{
	//set parent
	parent = newParent;
}

void Node::setPath(bool newPath)
{
	//set path
	path = newPath;
}

bool Node::lowerScore(const Node n) const
{
	//compare two nodes
	return f < n.f;
}

void Node::resetColour()
{
	//sets default colour
	shape.setFillColor(sf::Color(255, 255, 255, 50));
}

void Node::setWall()
{
	//sets wall colour
	path = false;
	shape.setFillColor(sf::Color(0, 0, 0, 100));
}

void Node::setPath()
{
	//sets path colour
	path = true;
	shape.setFillColor(sf::Color(255, 255, 255, 50));
}

void Node::setGoal()
{
	//sets goal colour
	shape.setFillColor(sf::Color(0, 255, 0, 100));
}

void Node::setCurrent(bool cont)
{
	//check if the node needs to update its colour to current or not
	if (currentNode == false && cont == true) {
		shape.setFillColor(sf::Color(255, 0, 255, 150));
		currentNode = true;
	}
	else if (currentNode == true && cont == false) {
		shape.setFillColor(sf::Color(255, 255, 255, 50));
		currentNode = false;
	}
}

void Node::setOpen()
{
	//set open node colour
	shape.setFillColor(sf::Color(0, 255, 255, 100));
}

void Node::setClosed()
{
	//set closed node colour
	shape.setFillColor(sf::Color(255, 0, 0, 100));
}

void Node::drawNode(sf::RenderTarget & target) const
{
	//draw node
	target.draw(shape);
}

void Node::score(int goalX, int goalY)
{
	// Calculate F score
	float dx, dy;

	//g = parent + 1.0f; // G is parent's G + 1
	dx = goalX - getXPos(); // distance on $x$ axis from current node to goal node;
	dy = goalY - getYPos(); // distance on $y$ axis from current node to goal node;
	h = sqrt((dx*dx) + (dy*dy)); // H is the Euclidean distance to the goal
	f = g + h; // F is just G + H
}
