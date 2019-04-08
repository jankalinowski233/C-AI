#include "SmartTank.h"
#include <iostream>

SmartTank::SmartTank()
{
	state = IDLE;
	SetMoveTarget();
}

SmartTank::~SmartTank()
{
}

void SmartTank::move()
{

	switch (state)
	{
	case IDLE:
	{
		break;
	}
		
	case MOVE:
	{
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

		float distance = 5.f;
		sf::Vector2f tempVec = movePoint - sf::Vector2f(pos.getX(), pos.getY());

		float m = sqrt(tempVec.x * tempVec.x + tempVec.y * tempVec.y);

		if (m < distance)
		{
			state = ROTATE_TURRET;
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
		firingFlag = true;
		tankSpottedFlag = false;
		break;
	}
		
	}

}

void SmartTank::reset()
{
	SetMoveTarget();
}

void SmartTank::collided()
{
	if (eBaseLocations.size() == 0)
	{
		SetMoveTarget();
	}
	
}

void SmartTank::markEnemy(Position p)
{
	if (tankSpottedFlag == false)
	{
		clearMovement();
		eBaseCurrentTarget = sf::Vector2f(p.getX(), p.getY());
		state = AIM;
		tankSpottedFlag = true;
	}
	
}

void SmartTank::markBase(Position p)
{
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

		eBaseSpottedFlag = true;
		clearMovement();

		float deltaX = p.getX() - getX();
		float deltaY = p.getY() - getY(); 

		state = TARGET_FOUND;
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
			break;
		}

	}

	if (tankSpottedFlag == false)
	{
		selectTarget();
	}
}

void SmartTank::DemarkEnemy()
{
	if (tankSpottedFlag == true)
	{
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
	state = MOVE;
	float x = (float)(rand() % 780 + 10);
	float y = (float)(rand() % 580 + 10);
	//float th = (float)(rand() % 359);

	movePoint = sf::Vector2f(x, y);
	std::cout << "Moving to: " << movePoint.x << " " << movePoint.y << std::endl;
	RotationAngle();
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
	sf::Vector2f tempMovePoint = movePoint;
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
		std::cout << eBaseLocations[i].x << " " << eBaseLocations[i].y << std::endl;
		
	}
	std::cout << std::endl;
	state = AIM;

	if (eBaseLocations.size() == 0)
	{
		firingFlag = false;
		state = MOVE;
	}
}