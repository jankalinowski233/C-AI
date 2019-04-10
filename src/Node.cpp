#include "..\include\Node.h"

Node::Node()
{
}

Node::Node(int x, int y, int xSize, int ySize, int r, int col)
{
	posX = x;
	posY = y;
	sizeX = xSize;
	sizeY = ySize;
	row = r;
	column = col;
	currentNode = false;

	shape.setSize(sf::Vector2f(sizeX, sizeY));
	shape.setOrigin(sizeX / 2, sizeY / 2);
	shape.setPosition(sf::Vector2f(posX + sizeX/2, posY+sizeY/2));
	shape.setFillColor(sf::Color(255, 255, 255, 50));
	shape.setOutlineThickness(1);
	shape.setOutlineColor(sf::Color(255, 255, 255, 100));
}

int Node::getXPos()
{
	return posX + sizeX / 2;
}

int Node::getYPos()
{
	return posY + sizeY / 2;
}

int Node::getRow()
{
	return row;
}

int Node::getColumn()
{
	return column;
}

int Node::getParent()
{
	return parent;
}

float Node::getF()
{
	return f;
}

float Node::getG()
{
	return g;
}

float Node::getH()
{
	return h;
}

bool Node::isCurrent()
{
	return currentNode;
}

bool Node::isPath()
{
	return path;
}

void Node::setG(float newG)
{
	g = newG;
}

void Node::setParent(int newParent)
{
	parent = newParent;
}

void Node::setPath(bool newPath)
{
	path = newPath;
}

bool Node::lowerScore(const Node n) const
{
	return f < n.f;
}

void Node::resetColour()
{
	shape.setFillColor(sf::Color(255, 255, 255, 50));
}

void Node::setWall()
{
	path = false;
	shape.setFillColor(sf::Color(0, 0, 0, 100));
}

void Node::setPath()
{
	path = true;
	shape.setFillColor(sf::Color(255, 255, 255, 50));
}

void Node::setGoal()
{
	shape.setFillColor(sf::Color(255, 0, 0, 100));
}

void Node::setCurrent(bool cont)
{
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
	shape.setFillColor(sf::Color(255, 255, 255, 100));
}

void Node::setClosed()
{
	shape.setFillColor(sf::Color(255, 255, 0, 100));
}

void Node::drawNode(sf::RenderTarget & target) const
{
	target.draw(shape);
}

void Node::score(int goalX, int goalY)
{
	// Calculate F score
	float dx, dy;

	//g = parent + 1.0f; // G is parent's G + 1
	dx = goalX - getXPos();// distance on $x$ axis from current node to goal node;
	dy = goalY - getYPos();// distance on $y$ axis from current node to goal node;
	h = sqrt((dx*dx) + (dy*dy)); // H is the Euclidean distance to the goal
	f = g + h; // F is just G + H
}
