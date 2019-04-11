#include "SmartTank.h"
#include <iostream>

SmartTank::SmartTank()
{
	m = new Map();
	state = IDLE;
}

SmartTank::~SmartTank()
{
	delete m;
}

void SmartTank::move()
{

	switch (state)
	{
	case IDLE:
	{
		clearMovement();
		if (!path.empty())
		{
			path.clear();
		}
		SetMoveTarget();
		break;
	}
		
	case MOVE:
	{
		if (path.empty())
		{
			SetMoveTarget();
		}
		RotationAngle();
		ResetTurretDir();
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

		sf::Vector2f tempVec = sf::Vector2f(path.front().getXPos(), path.front().getYPos()) - sf::Vector2f(pos.getX(), pos.getY());

		float m = sqrt(tempVec.x * tempVec.x + tempVec.y * tempVec.y);

		if (m < distance)
		{
			if (path.size() == 1)
			{
				state = ROTATE_TURRET;
				path.clear();
			}
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
		ScanForTarget();
		break;
	}
		

	case TARGET_FOUND:
	{
		selectTarget();
		break;
	}

	case AIM:
	{
		RotateTurretToTarget();
		break;
	}
		
	case FIRE:
	{
		if (OutOfAmmo() == false)
		{
			float deltaX = eBaseCurrentTarget.x - getX();
			float deltaY = eBaseCurrentTarget.y - getY();

			float angleInDegree = atan2(deltaY, deltaX); //gets the angle of the player tank in relation to the enemy 
			angleInDegree = RAD2DEG(angleInDegree);

			float turretAngle = angleInDegree;
			turretAngle -= turretTh;

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

			if (turretAngle < 1.f || turretAngle > 359.f)
			{
				stopTurret();
				firingFlag = true;
			}
			else
			{
				firingFlag = false;
				state = AIM;
			}
			
		}
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
	firingFlag = false;

	for (auto it = eBaseLocations.begin(); it != eBaseLocations.end(); ++it)
	{
		updateWalls(*it);
		std::cout << "Output 3: " << it->x << " " << it->y << std::endl;

	}

	if (!eBaseLocations.empty())
	{
		eBaseLocations.clear();
	}

	DemarkEnemy();
	state = IDLE;
}

void SmartTank::collided()
{
	SetMoveTarget();
}

void SmartTank::markEnemy(Position p)
{
	eBaseCurrentTarget = sf::Vector2f(p.getX(), p.getY());

	if (tankSpottedFlag == false && OutOfAmmo() == false)
	{
		std::cout << "tank vision" << std::endl;
		clearMovement();
		state = AIM;
		tankSpottedFlag = true;
	}
	
}

void SmartTank::markBase(Position p)
{
	bool match = false;

	for (auto it = fBaseLocations.begin(); it != fBaseLocations.end(); ++it)
	{
		sf::Vector2f temp = sf::Vector2f(p.getX(), p.getY());
		if (temp == *it)
		{
			match = true;
		}
	}

	if (match == false)
	{
		fBaseLocations.push_back(sf::Vector2f(p.getX(), p.getY()));

		m->notPath(p.getX(), p.getY());
		SetCurrentPos();
		m->updatePath(path, *current);
	}
}

void SmartTank::markShell(Position p)
{
}

void SmartTank::markTarget(Position p)
{
	
	bool match = false;

	for (auto it = eBaseLocations.begin(); it != eBaseLocations.end(); ++it)
	{
		sf::Vector2f temp = sf::Vector2f(p.getX(), p.getY());
		if (temp == *it)
		{
			match = true;
		}
	}
	
	if (match == false)
	{
		eBaseLocations.push_back(sf::Vector2f(p.getX(), p.getY()));

		m->notPath(p.getX(), p.getY());
		SetCurrentPos();
		m->updatePath(path, *current);

		if (OutOfAmmo() == false)
		{
			eBaseSpottedFlag = true;
			clearMovement();

			state = TARGET_FOUND;
		}
	}
	
	
}

bool SmartTank::isFiring()
{
	return firingFlag;
}

void SmartTank::score(int thisScore, int enemyScore)
{
}

void SmartTank::DeleteBase(Position p)
{
	for (auto it = eBaseLocations.begin(); it != eBaseLocations.end(); ++it)
	{
		sf::Vector2f temp = sf::Vector2f(p.getX(), p.getY());
		if (temp == *it)
		{
			eBaseLocations.erase(it);
			updateWalls(p);

			if (tankSpottedFlag == false)
			{
				selectTarget();
			}

			break;
		}

	}

	for (auto it2 = fBaseLocations.begin(); it2 != fBaseLocations.end(); ++it2)
	{
		sf::Vector2f temp = sf::Vector2f(p.getX(), p.getY());
		if (temp == *it2)
		{
			fBaseLocations.erase(it2);
			updateWalls(p);

			break;
		}
	}


}

void SmartTank::DemarkEnemy()
{
	if (tankSpottedFlag == true)
	{
		std::cout << "lost tank" << std::endl;
		firingFlag = false;
		selectTarget();
		tankSpottedFlag = false;
	}
}

void SmartTank::RotateTurretToTarget()
{
	firingFlag = false;
	float deltaX = eBaseCurrentTarget.x - getX();
	float deltaY = eBaseCurrentTarget.y - getY();

	float angleInDegree = atan2(deltaY, deltaX); //gets the angle of the player tank in relation to the enemy 
	angleInDegree = RAD2DEG(angleInDegree);

	float turretAngle = angleInDegree;
	turretAngle -= turretTh;

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

	if (turretAngle < 0.5f || turretAngle > 359.5f)
	{
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
	float dir = turretTh;
	float currentTankOrientation = pos.getTh();

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
	firingFlag = false;
	movementTargetFound = false;
	SetCurrentPos();
	std::cout << "commencing a*" << std::endl;

	while (movementTargetFound == false && path.size() < 2)
	{
		path.clear();
		int randomX = rand() % 24 + 1;
		int randomY = rand() % 17 + 1;
		if (m->mapGrid[randomY][randomX]->isPath() == true)
		{
			movementTargetFound = m->Astar(path, *current, *m->mapGrid[randomY][randomX]);
		}
	}

	std::cout << "completed a*. path size: " << path.size() << std::endl;
	state = MOVE;
	RotationAngle();
}

void SmartTank::SetCurrentPos()
{
	const int xwidth = 780;
	const int yheight = 570;
	float x = getX();
	float y = getY();
	int nodeSize = xwidth / 26;
	int mx = (x / nodeSize);
	int my = (y / nodeSize);

	for (int i = 0; i < 19; i++) {
		for (int j = 0; j < 26; j++) {
			m->mapGrid[i][j]->setCurrent(false);
		}
	}
	m->mapGrid[my][mx]->setCurrent(true);

	current = m->mapGrid[my][mx];
}

void SmartTank::ScanForTarget()
{
	clearMovement();
	float currentRot = turretTh;

	if (scanFlag == false)
	{
		startPoint = currentRot;
		checkpoint = startPoint + 180.f;
		if (checkpoint > 360.f)
		{
			checkpoint -= 360.f;
		}
		scanFlag = true;
	}
	else
	{
		turretGoRight();
		if (scanCheckpoint == false)
		{
			if (currentRot > checkpoint - 1.f && currentRot < checkpoint + 1.f)
			{
				scanCheckpoint = true;
			}
		}
		else 
		{
			if (currentRot > startPoint - 1.f && currentRot < startPoint + 1.f)
			{
				scanFlag = false;
				scanCheckpoint = false;
				stopTurret();
				SetMoveTarget();
			}
		}
	}

}

void SmartTank::RotationAngle()
{
	if (path.size() > 0)
	{
		sf::Vector2f tempMovePoint = sf::Vector2f(path.front().getXPos(), path.front().getYPos());
		sf::Vector2f tempTankPos = sf::Vector2f(pos.getX(), pos.getY());

		sf::Vector2f length = tempMovePoint - tempTankPos;

		float magnitude = sqrt(length.x * length.x + length.y * length.y);
		float tankAngle = pos.getTh();

		sf::Vector2f direction = length / magnitude;

		angle = atan2f(-direction.x, -direction.y);
		angle = 90.f + RAD2DEG(angle);
		angle += tankAngle;

		if (angle > 360.f)
		{
			angle -= 360.f;
		}
	}
	else
	{
		std::cout << "has no path" << std::endl;
	}
}

void SmartTank::selectTarget()
{
	float dis = 10000.f;
	float tempDis = 0.f;
	float deltaX;
	float deltaY;

	for (int i = 0; i < eBaseLocations.size(); i++)
	{
		deltaX = getX() - eBaseLocations[i].x;
		deltaY = getY() - eBaseLocations[i].y;

		tempDis = sqrt((deltaX * deltaX) + (deltaY * deltaY));

		if (tempDis < dis)
		{
			dis = tempDis;
			eBaseCurrentTarget = eBaseLocations[i];
		}
	}
	state = AIM;

	if (eBaseLocations.size() == 0)
	{
		firingFlag = false;
		if (!path.empty())
		{
			state = MOVE;
		}
		else
		{
			state = ROTATE_TURRET;
		}
	}
}

bool SmartTank::OutOfAmmo()
{
	if (getNumberOfShells() == 0)
	{
		state = MOVE;
		return true;
	}
	return false;
}

void SmartTank::updateWalls(Position p)
{
	m->setPath(p.getX(), p.getY());

	for (auto it = eBaseLocations.begin(); it != eBaseLocations.end(); ++it)
	{
		m->notPath(it->x, it->y);
	}

	for (auto it2 = fBaseLocations.begin(); it2 != fBaseLocations.end(); ++it2)
	{
		m->notPath(it2->x, it2->y);
	}

	SetCurrentPos();
	m->updatePath(path, *current);
}

void SmartTank::updateWalls(sf::Vector2f pos)
{
	m->setPath(pos.x, pos.y);
}
