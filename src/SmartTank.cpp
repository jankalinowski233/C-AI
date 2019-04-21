#include "SmartTank.h"
#include <iostream>

SmartTank::SmartTank()
{
	//create map
	m = new Map();
	//beginning state
	state = IDLE;
}

SmartTank::~SmartTank()
{
	//delete map
	delete m;
}

void SmartTank::move()
{
	//FSM -- handles all states
	switch (state)
	{
		//Idle states
	case IDLE:
	{
		//clear movement
		clearMovement();
		//make sure the path is not out of range
		if (!path.empty())
		{
			path.clear();
		}
		//look for target
		SetMoveTarget();
		break;
	}
		
	case MOVE:
	{
		//make sure tank has the path
		if (path.empty())
		{
			SetMoveTarget();
		}
		//rotate tank and its turret
		RotationAngle();
		ResetTurretDir();
		//read an angle - go forward or rotate 
		if (angle < 5 || angle > 359)
		{
			goForward();
		}
		else if (angle >= 1 && angle <= 180)
		{
			goLeft();
		}
		else if (angle >= 180 && angle <= 360)
		{
			goRight();
		}

		//gets direction from current tank's position to destination
		sf::Vector2f tempVec = sf::Vector2f(path.front().getXPos(), path.front().getYPos()) - sf::Vector2f(pos.getX(), pos.getY());

		//distance between tank and destination
		float m = sqrt(tempVec.x * tempVec.x + tempVec.y * tempVec.y);

		//check if tank has reached destination
		if (m < distance)
		{
			//check if tank's reached final position
			if (path.size() == 1)
			{
				state = ROTATE_TURRET;
				path.clear();
			}
			//check if tank has reached any other node in the path
			else
			{
				path.pop_front();
				clearMovement();
			}
		}
		break;
	}

	case ROTATE_TURRET:
	{
		//rotate turret 360* around
		ScanForTarget();
		break;
	}
		

	case TARGET_FOUND:
	{
		//choose target
		selectTarget();
		break;
	}

	case AIM:
	{
		//aim at target
		RotateTurretToTarget();
		break;
	}
		
	case FIRE:
	{
		//check if tank is out of ammo
		if (OutOfAmmo() == false)
		{
			//get direction from tank to target
			float deltaX = eBaseCurrentTarget.x - getX();
			float deltaY = eBaseCurrentTarget.y - getY();

			float angleInDegree = atan2(deltaY, deltaX); //gets the angle of the player tank in relation to the enemy 
			angleInDegree = RAD2DEG(angleInDegree);

			//rotate turret
			float turretAngle = angleInDegree;
			turretAngle -= turretTh;

			//clamp degrees range between 0 and 360
			while (turretAngle < 0.f || turretAngle > 360.f)
			{
				if (turretAngle < 0.f)
				{
					turretAngle += 360.f;
				}
				else if (turretAngle > 360.f)
				{
					turretAngle -= 360.f;
				}
			}

			//check if turret is aiming at the target
			if (turretAngle < 1.f || turretAngle > 359.f)
			{
				//stop turret and fire
				stopTurret();
				firingFlag = true;
			}
			//reaim if needed
			else
			{
				firingFlag = false;
				state = AIM;
			}
			
		}
		//if tank's out of ammo - don't shoot
		else
		{
			firingFlag = false;
			state = IDLE;
		}
		break;
	}
		
	}

}

void SmartTank::reset()
{
	//stop firing
	firingFlag = false;

	//removes enemy buildings' position from tank's memory
	for (auto it = eBaseLocations.begin(); it != eBaseLocations.end(); ++it)
	{
		updateWalls(*it);
	}

	//clear the vector
	if (!eBaseLocations.empty())
	{
		eBaseLocations.clear();
	}

	//demark enemy
	DemarkEnemy();
	//reset state
	state = IDLE;
}

void SmartTank::collided()
{
	//prevent tank from being stuck
	SetMoveTarget();
}

void SmartTank::markEnemy(Position p)
{
	//set enemy's position to be current target
	eBaseCurrentTarget = sf::Vector2f(p.getX(), p.getY());

	//if not out of ammo
	if (tankSpottedFlag == false && OutOfAmmo() == false)
	{
		//aim and shoot
		std::cout << "tank vision" << std::endl;
		clearMovement();
		state = AIM;
		tankSpottedFlag = true;
	}
	
}

void SmartTank::markBase(Position p)
{
	//bool to controll if the base is already spotted
	bool match = false;

	//check if friendly base has been already marked
	for (auto it = fBaseLocations.begin(); it != fBaseLocations.end(); ++it)
	{
		sf::Vector2f temp = sf::Vector2f(p.getX(), p.getY());
		if (temp == *it)
		{
			match = true;
		}
	}

	//if it hasn't
	if (match == false)
	{
		//add it to the vector
		fBaseLocations.push_back(sf::Vector2f(p.getX(), p.getY()));

		//update walls
		m->notPath(p.getX(), p.getY());
		SetCurrentPos();
		//update current path
		m->updatePath(path, *current);
	}
}

void SmartTank::markShell(Position p)
{
}

void SmartTank::markTarget(Position p)
{
	//bool to controll if the base is already spotted
	bool match = false;

	//check if enemy's base has been already marked
	for (auto it = eBaseLocations.begin(); it != eBaseLocations.end(); ++it)
	{
		sf::Vector2f temp = sf::Vector2f(p.getX(), p.getY());
		if (temp == *it)
		{
			match = true;
		}
	}
	
	//if it hasn't
	if (match == false)
	{
		//add it to the vector
		eBaseLocations.push_back(sf::Vector2f(p.getX(), p.getY()));

		//update walls
		m->notPath(p.getX(), p.getY());
		SetCurrentPos();
		//update current path
		m->updatePath(path, *current);

		//if not out of ammo
		if (OutOfAmmo() == false)
		{
			//aim and fire
			eBaseSpottedFlag = true;
			clearMovement();

			state = TARGET_FOUND;
		}
	}
	
	
}

bool SmartTank::isFiring()
{
	//fire
	return firingFlag;
}

void SmartTank::score(int thisScore, int enemyScore)
{
}

void SmartTank::DeleteBase(Position p)
{
	//loop through the vector
	for (auto it = eBaseLocations.begin(); it != eBaseLocations.end(); ++it)
	{
		//convert position data to vector
		sf::Vector2f temp = sf::Vector2f(p.getX(), p.getY());

		//if the base's location was stored
		if (temp == *it)
		{
			//delete it from vector
			eBaseLocations.erase(it);
			//update walls
			updateWalls(p);

			//if enemy's tank is not in range of vision
			if (tankSpottedFlag == false)
			{
				//select next target
				selectTarget();
			}

			break;
		}

	}

	//loop through the vector
	for (auto it2 = fBaseLocations.begin(); it2 != fBaseLocations.end(); ++it2)
	{
		//convert position data to vector
		sf::Vector2f temp = sf::Vector2f(p.getX(), p.getY());

		//if the base's location was stored
		if (temp == *it2)
		{
			//delete it from vector
			fBaseLocations.erase(it2);
			//update walls
			updateWalls(p);

			break;
		}
	}


}

void SmartTank::DemarkEnemy()
{
	//if tank has gone out of view
	if (tankSpottedFlag == true)
	{
		std::cout << "lost tank" << std::endl;
		//stop firing
		firingFlag = false;
		//select next target
		selectTarget();
		//reset bool
		tankSpottedFlag = false;
	}
}

void SmartTank::RotateTurretToTarget()
{
	//reset bool
	firingFlag = false;
	//get direction
	float deltaX = eBaseCurrentTarget.x - getX();
	float deltaY = eBaseCurrentTarget.y - getY();

	float angleInDegree = atan2(deltaY, deltaX); //gets the angle of the player tank in relation to the enemy 
	angleInDegree = RAD2DEG(angleInDegree);

	//get current turret target
	float turretAngle = angleInDegree;
	turretAngle -= turretTh;

	//clamp angles between 0 and 360
	while (turretAngle < 0.f || turretAngle > 360.f)
	{
		if (turretAngle < 0.f)
		{
			turretAngle += 360.f;
		}
		else if (turretAngle > 360.f)
		{
			turretAngle -= 360.f;
		}
	}

	//rotate turret towards target
	if (turretAngle < 0.5f || turretAngle > 359.5f)
	{
		//stop and fire
		stopTurret();
		state = FIRE;
	}
	else if (turretAngle > 180)
	{
		turretGoLeft();
	}
	else if (turretAngle < 180)
	{
		turretGoRight();
	}
}

void SmartTank::ResetTurretDir()
{
	//get current turret direction
	float dir = turretTh;
	//get current tank orientation
	float currentTankOrientation = pos.getTh();

	//rotate turret towards the front of the tank
	if (dir > (-1 + currentTankOrientation) && dir < (1+ currentTankOrientation))
	{
		stopTurret();
	}
	else if (dir < currentTankOrientation)
	{
		turretGoRight();
	}
	else if (dir > currentTankOrientation)
	{
		turretGoLeft();
	}
}

void SmartTank::SetMoveTarget()
{
	//reset bools
	firingFlag = false;
	movementTargetFound = false;
	//mark current tank's position
	SetCurrentPos();
	//reset the path
	path.clear();
	std::cout << "commencing a*" << std::endl;

	//look for new destination and make sure it's big enough
	while (movementTargetFound == false && path.size() < 2)
	{
		path.clear();
		int randomX = rand() % 24 + 1;
		int randomY = rand() % 17 + 1;
		//if destination is a valid path (is allowed to move there)
		if (m->mapGrid[randomY][randomX]->isPath() == true)
		{
			//create A* path
			movementTargetFound = m->Astar(path, *current, *m->mapGrid[randomY][randomX]);
		}
	}

	std::cout << "completed a*. path size: " << path.size() << std::endl;
	//move
	state = MOVE;
	//rotate
	RotationAngle();
}

void SmartTank::SetCurrentPos()
{
	//map's size
	const int xwidth = 780;
	const int yheight = 570;
	//current tank position
	float x = getX();
	float y = getY();
	//size of the node
	int nodeSize = xwidth / 26;
	//current node the tank is at (converted to rows and columns)
	int mx = (x / nodeSize);
	int my = (y / nodeSize);

	//set all the nodes to be initially not current
	for (int i = 0; i < 19; i++) {
		for (int j = 0; j < 26; j++) {
			m->mapGrid[i][j]->setCurrent(false);
		}
	}
	//mark the current node
	m->mapGrid[my][mx]->setCurrent(true);

	//save the current node position
	current = m->mapGrid[my][mx];
}

void SmartTank::ScanForTarget()
{
	//stop moving
	clearMovement();
	//get current turret rotation
	float currentRot = turretTh;
	
	//if the scanning has just begun
	if (scanFlag == false)
	{
		//start point is at current rotation
		startPoint = currentRot;
		//checkpoint is 180* away
		checkpoint = startPoint + 180.f;
		//make sure checkpoint is not going out of range
		if (checkpoint > 360.f)
		{
			checkpoint -= 360.f;
		}
		//start scanning
		scanFlag = true;
	}
	else
	{
		//scan to right
		turretGoRight();
		//check if the turret has reached the checkpoint
		if (scanCheckpoint == false)
		{
			//if so mark it to true
			if (currentRot > checkpoint - 1.f && currentRot < checkpoint + 1.f)
			{
				scanCheckpoint = true;
			}
		}
		//now go from checkpoint to startpoint
		else 
		{
			//if the rotation has ended and target's not been found
			if (currentRot > startPoint - 1.f && currentRot < startPoint + 1.f)
			{
				//stop scanning
				scanFlag = false;
				scanCheckpoint = false;
				//stop turret
				stopTurret();
				//look for new move destination
				SetMoveTarget();
			}
		}
	}

}

void SmartTank::RotationAngle()
{
	//if tank has the path
	if (path.size() > 0)
	{
		//destination
		sf::Vector2f tempMovePoint = sf::Vector2f(path.front().getXPos(), path.front().getYPos());
		//current tank's position
		sf::Vector2f tempTankPos = sf::Vector2f(pos.getX(), pos.getY());

		//vector between destination and tank's position
		sf::Vector2f length = tempMovePoint - tempTankPos;

		//magnitude of the vector
		float magnitude = sqrt(length.x * length.x + length.y * length.y);
		//current tank's angle
		float tankAngle = pos.getTh();

		//current direction
		sf::Vector2f direction = length / magnitude;

		//calculate angle
		angle = atan2f(-direction.x, -direction.y);
		angle = 90.f + RAD2DEG(angle);
		//rotate towards angle
		angle += tankAngle;

		//clamp angle
		if (angle > 360.f)
		{
			angle -= 360.f;
		}
	}

}

void SmartTank::selectTarget()
{
	//distance threshold
	float dis = 10000.f;
	//current distance
	float tempDis = 0.f;
	//direction from tank to the base on the X axis
	float deltaX;
	//direction from tank to the base on the Y axis
	float deltaY;

	//loop through the bases
	for (int i = 0; i < eBaseLocations.size(); i++)
	{
		//calculate directions
		deltaX = getX() - eBaseLocations[i].x;
		deltaY = getY() - eBaseLocations[i].y;

		//get the magnitude of each of them
		tempDis = sqrt((deltaX * deltaX) + (deltaY * deltaY));

		//evaluate which base is the closest
		if (tempDis < dis)
		{
			//set the distance
			dis = tempDis;
			//set current target to be the closest base
			eBaseCurrentTarget = eBaseLocations[i];
		}
	}
	//aim at the closest base
	state = AIM;

	//if no more bases in vision
	if (eBaseLocations.size() == 0)
	{
		//stop firing
		firingFlag = false;
		//if tank has a path - continue moving
		if (!path.empty())
		{
			state = MOVE;
		}
		//else carry on scanning the area
		else
		{
			state = ROTATE_TURRET;
		}
	}
}

bool SmartTank::OutOfAmmo()
{
	//check if the tank is out ammo - if so, ignore everything, just carry on moving around
	if (getNumberOfShells() == 0)
	{
		state = MOVE;
		return true;
	}
	return false;
}

void SmartTank::updateWalls(Position p)
{
	//reset the node to be a possible path
	m->setPath(p.getX(), p.getY());

	//set the bases' nodes to a wall
	for (auto it = eBaseLocations.begin(); it != eBaseLocations.end(); ++it)
	{
		m->notPath(it->x, it->y);
	}

	//set the bases' nodes to a wall
	for (auto it2 = fBaseLocations.begin(); it2 != fBaseLocations.end(); ++it2)
	{
		m->notPath(it2->x, it2->y);
	}

	//mark current tank's position
	SetCurrentPos();
	//update path
	m->updatePath(path, *current);
}

void SmartTank::updateWalls(sf::Vector2f pos)
{
	//reset the node to be a possible path
	m->setPath(pos.x, pos.y);
}
