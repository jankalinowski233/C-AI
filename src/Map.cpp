#include "..\include\Map.h"

Map::Map()
{
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
	if (!path.empty())
	{
		if (mapGrid[path.back().getRow()][path.back().getColumn()]->isPath() == true)
		{
			Node s = curr;
			Node g = path.back();
			path.clear();
			Astar(path, s, g);
		}

		else
		{
			bool movementTargetFound = false;
			while (movementTargetFound == false)
			{
				int randomX = rand() % 16 + 3;
				int randomY = rand() % 13 + 3;
				if (mapGrid[randomX][randomY]->isPath() == true)
				{
					movementTargetFound = Astar(path, curr, *mapGrid[randomX][randomY]);
				}
			}
		}
	}
}

void Map::draw(sf::RenderTarget & target) const
{
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
	clearVisual();
	std::list<Node> openNodes; //list to be explored
	std::list<Node> closedNodes; //list already explored
	closedNodes.empty();

	Node current = s;
	Node goal = g;

	current.setParent(-1);
	current.setG(0.f); 
	current.score(goal.getXPos(), goal.getYPos());
	openNodes.push_back(current);

	while (!openNodes.empty())
	{
		std::stable_sort(openNodes.begin(), openNodes.end(), std::mem_fun_ref(&Node::lowerScore)); //sort the list
		if (current.getRow() == goal.getRow() && current.getColumn() == goal.getColumn())
		{
			path = constructPath(path, closedNodes, &goal);
			return true;
		}

		current = openNodes.front(); //node that scored best
		openNodes.pop_front(); //take it off open list
		closedNodes.push_back(current); //add it to the closed list

		mapGrid[current.getRow()][current.getColumn()]->setGoal(); //colorize the goal

		//std::list<Node>::iterator openNodesIt;
		//std::list<Node>::iterator closedNodesIt;

		for (Node* neighbour : getNeighbour(&current))
		{
			bool closedNeighbour = false;
			for (auto closedIter = closedNodes.begin(); closedIter != closedNodes.end(); ++closedIter)
			{
				if (neighbour->getRow() == closedIter->getRow() && neighbour->getColumn() == closedIter->getColumn())
				{
					closedNeighbour = true;
				}
			}

			if (closedNeighbour == false)
			{
				neighbour->score(goal.getXPos(), goal.getYPos());
				bool openNeighbour = false;
				for (auto openIter = openNodes.begin(); openIter != openNodes.end(); ++openIter) 
				{
					if (neighbour->getRow() == openIter->getRow() && neighbour->getColumn() == openIter->getColumn())
					{
						openNeighbour = true;
					}
				}
				if (openNeighbour == false)
				{
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
	int rIndex = n->getRow();
	int cIndex = n->getColumn();
	float diagonalCost = sqrt(2);
	float cost = 1.f;

	const int directions = 8;
	const int checkers = 3;

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

	std::vector<Node*> neighbours;
	std::vector<bool> diagonals;

	for (int i = 0; i < directions; i++)
	{
		if (matrix[i][0] < rows && matrix[i][1] < columns)
		{
			if (mapGrid[matrix[i][0]][matrix[i][1]]->isPath() == true)
			{
				neighbours.push_back(mapGrid[matrix[i][0]][matrix[i][1]]);
				diagonals.push_back(matrix[i][2]);
			}
		}
	}

	for (int j = 0; j < neighbours.size(); j++)
	{
		if (diagonals[j] == true)
		{
			neighbours[j]->setG(n->getG() + diagonalCost);
		}
		else
		{
			neighbours[j]->setG(n->getG() + cost);
			//neighbours[j]->setParent(n->getParent());
		}
	}

	return neighbours;
}

std::list<Node> Map::constructPath(std::list<Node>& path, std::list<Node>& closed, Node * n)
{
	int p;
	std::list<Node>::iterator iter;

	Node* current = n;
	p = current->getParent();

	path.push_front(*current);
	closed.pop_back();

	for (iter = closed.end(), iter--; iter != closed.begin(); --iter)
	{
		path.push_front(*iter);
		closed.erase(iter);
		iter = closed.end();
	}

	return path;
}

Map::~Map()
{
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			delete mapGrid[i][j];
		}
	}
}
