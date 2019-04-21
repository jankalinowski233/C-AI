#include "..\include\Map.h"

Map::Map()
{
	//initialize map
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			n = new Node(xOffset + width * j, yOffset + height * i, width, height, i, j);
			mapGrid[i][j] = n;
		}
	}
}

void Map::notPath(int x, int y)
{
	//set the nodes around a point to be walls (prevents tank from getting too close to the wall and getting stuck)
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			if (x - 30 < (mapGrid[i][j]->getXPos() - width / 2) + width && x + 30 > mapGrid[i][j]->getXPos() - width / 2 &&
				y - 30 < (mapGrid[i][j]->getYPos() - height / 2) + height && y + 30 > mapGrid[i][j]->getYPos() - height / 2)
			{
				mapGrid[i][j]->setWall();
			}
		}
	}
}

void Map::setPath(int x, int y)
{
	//set the nodes around the point to be considered as a possible path
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			if (x - 30 < (mapGrid[i][j]->getXPos() - width / 2) + width && x + 30 > mapGrid[i][j]->getXPos() - width / 2 &&
				y - 30 < (mapGrid[i][j]->getYPos() - height / 2) + height && y + 30 > mapGrid[i][j]->getYPos() - height / 2)
			{
				mapGrid[i][j]->setPath();
			}
		}
	}
}

void Map::clearVisual()
{
	//clear colours
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			if (mapGrid[i][j]->isPath() == true)
			{
				mapGrid[i][j]->resetColour();
			}
		}
	}
}

void Map::updatePath(std::list<Node>& path, Node curr)
{
	//if path is not empty
	if (!path.empty())
	{
		//if path is of size 1
		if (path.back().getRow() == curr.getRow() && path.back().getColumn() == curr.getColumn())
		{
			std::cout << "finding new pos" << std::endl;

			//keep looking for new destination
			bool movementTargetFound = false;
			while (movementTargetFound == false)
			{
				int randomX = rand() % 24 + 1;
				int randomY = rand() % 17 + 1;
				if (mapGrid[randomY][randomX]->isPath() == true)
				{
					//destination found
					movementTargetFound = Astar(path, curr, *mapGrid[randomY][randomX]);
				}
			}

		}

		//if destination is valid path
		else if (mapGrid[path.back().getRow()][path.back().getColumn()]->isPath() == true)
		{
			//mark current position
			Node s = curr;
			//mark the destination position
			Node g = path.back();
			//clear the old path
			path.clear();
			//construct new path
			Astar(path, s, g);
		}

		//if the destination is not valid path
		else
		{
			//keep looking for destination
			bool movementTargetFound = false;
			while (movementTargetFound == false)
			{
				int randomX = rand() % 24 + 1;
				int randomY = rand() % 17 + 1;
				if (mapGrid[randomY][randomX]->isPath() == true)
				{
					//valid destination found
					movementTargetFound = Astar(path, curr, *mapGrid[randomY][randomX]);
				}
			}
		}
	}
}

void Map::draw(sf::RenderTarget & target) const
{
	//draw map in debug mode
	for (int i = 0; i < rows; i++) 
	{
		for (int j = 0; j < columns; j++) 
		{
			mapGrid[i][j]->drawNode(target);
		}
	}
}

bool Map::Astar(std::list<Node>& path, Node s, Node g)
{
	clearVisual(); //clear the colours
	std::list<Node> openNodes; //list to be explored
	std::list<Node> closedNodes; //list already explored
	closedNodes.empty(); //empty the list

	Node current = s;
	Node goal = g;

	//set the current position
	current.setParent(-1);
	current.setG(0.f); 
	current.score(goal.getXPos(), goal.getYPos());
	openNodes.push_back(current);

	while (!openNodes.empty())
	{
		std::stable_sort(openNodes.begin(), openNodes.end(), std::mem_fun_ref(&Node::lowerScore)); //sort the list
		//if the path has been found
		if (current.getRow() == goal.getRow() && current.getColumn() == goal.getColumn())
		{
			//construct path
			path = constructPath(path, closedNodes, &goal);
			return true;
		}

		current = openNodes.front(); //node that scored best
		openNodes.pop_front(); //take it off open list
		closedNodes.push_back(current); //add it to the closed list

		mapGrid[current.getRow()][current.getColumn()]->setGoal(); //colorize the goal

		//explore the neighbours
		for (Node* neighbour : getNeighbour(&current))
		{
			bool closedNeighbour = false;
			//loop through the closed nodes
			for (auto closedIter = closedNodes.begin(); closedIter != closedNodes.end(); ++closedIter)
			{
				//if the neighbour is there - it means it has been already explored and does not need to be explored again
				if (neighbour->getRow() == closedIter->getRow() && neighbour->getColumn() == closedIter->getColumn())
				{
					closedNeighbour = true;
				}
			}

			//else explore this neighbour
			if (closedNeighbour == false)
			{
				//set the score
				neighbour->score(goal.getXPos(), goal.getYPos());
				bool openNeighbour = false;
				//loop through the open nodes
				for (auto openIter = openNodes.begin(); openIter != openNodes.end(); ++openIter) 
				{
					//if the neighbour had been already explored
					if (neighbour->getRow() == openIter->getRow() && neighbour->getColumn() == openIter->getColumn())
					{
						openNeighbour = true;
					}
				}
				//if the neighbour hadn't been explored
				if (openNeighbour == false)
				{
					//add it to the open list
					openNodes.push_back(*neighbour);
					mapGrid[neighbour->getRow()][neighbour->getColumn()]->setClosed();
				}
			}
		}
	}

	return false;
}

std::vector<Node*> Map::getNeighbour(Node * n)
{
	//get row and columng
	int rIndex = n->getRow();
	int cIndex = n->getColumn();
	//max diagonal cost
	float diagonalCost = sqrt(2);
	//minimum cost
	float cost = 1.f;

	//number of directions a tank can go to
	const int directions = 8;
	const int checkers = 3;

	//construct the matrix - Row, Column, IsDiagonal
	int matrix[directions][checkers] = 
	{
		//non diagonal
		{ rIndex + 1, cIndex, 0 },
		{ rIndex - 1, cIndex, 0 },
		{ rIndex, cIndex - 1, 0 },
		{ rIndex, cIndex + 1, 0 },

		//diagonal
		{ rIndex + 1, cIndex + 1, 1 },
		{ rIndex - 1, cIndex + 1, 1 },
		{ rIndex - 1, cIndex - 1, 1 },
		{ rIndex + 1, cIndex - 1, 1 }
	};

	//vector of nodes
	std::vector<Node*> neighbours;
	//vector of bools
	std::vector<bool> diagonals;

	//loop through all directions
	for (int i = 0; i < directions; i++)
	{
		//if it is in the matrix
		if (matrix[i][0] < rows && matrix[i][1] < columns)
		{
			//if neighbour location is a path
			if (mapGrid[matrix[i][0]][matrix[i][1]]->isPath() == true)
			{
				//add the node
				neighbours.push_back(mapGrid[matrix[i][0]][matrix[i][1]]);
				//stores whether or not it is diagonal
				diagonals.push_back(matrix[i][2]);
			}
		}
	}

	//loop through the neighbours
	for (int j = 0; j < neighbours.size(); j++)
	{
		//if it is diagonal
		if (diagonals[j] == true)
		{
			//set the correct G cost
			neighbours[j]->setG(n->getG() + diagonalCost);
		}
		//if it is not diagonal
		else
		{
			//set the correct G cost
			neighbours[j]->setG(n->getG() + cost);
		}
	}

	//get the neighbours list
	return neighbours;
}

std::list<Node> Map::constructPath(std::list<Node>& path, std::list<Node>& closed, Node * n)
{
	int p; //temp parent
	//iterator
	std::list<Node>::iterator iter;

	//current node
	Node* current = n;
	//current's node parent
	p = current->getParent();

	//add the current node to the front of the list
	path.push_front(*current);
	//remove the last element from the closed list
	closed.pop_back();

	//if the closed list is still larger than 1
	if (closed.size() > 1)
	{
		//iteratate through the rest of the nodes in the closed list
		for (iter = closed.end(), iter--; iter != closed.begin(); --iter)
		{
			//construct path
			path.push_front(*iter);
			//delete the node from the closed list
			closed.erase(iter);
			//reassign iterator
			iter = closed.end();
		}
	}
	//if closed list is of size 1
	else
	{
		//empty the path
		path.empty();
	}

	//get the path
	return path;
}

Map::~Map()
{
	//delete nodes in the map
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			delete mapGrid[i][j];
		}
	}
}
