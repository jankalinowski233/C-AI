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
			SetMoveTarget();
		}
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
		std::cout << "FIRE" << std::endl;
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
	SetMoveTarget();
}

void SmartTank::markEnemy(Position p)
{
	tankSpottedFlag = true;

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

		float deltaX = getX() - p.getX();
		float deltaY = getY() - p.getY();

		float angleInDegree = atan2(deltaY, deltaX) * 180 / PI; //gets the angle of the player tank in relation to the enemy 
		//turretAngle = angleInDegree + 180;
		//std::cout << "Target spotted at (" << p.getX() << ", " << p.getY() << ")\n";

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

void SmartTank::RotateTurretToTarget()
{
	float deltaX = eBaseCurrentTarget.x - getX();
	float deltaY = eBaseCurrentTarget.y - getY();

	float angleInDegree = atan2(deltaY, deltaX); //gets the angle of the player tank in relation to the enemy 
	angleInDegree = RAD2DEG(angleInDegree);

	float turretAngle = angleInDegree;
	turretAngle -= turretTh;

	if (turretAngle < 2.5 && turretAngle > -2.5)
	{
		stopTurret();
		state = FIRE;
	}
	else if (turretAngle < -2.5)
	{
		turretGoLeft();
	}
	else if (turretAngle > 2.5)
	{
		turretGoRight();
	}

		
}

void SmartTank::SetMoveTarget()
{
	state = MOVE;
	float x = (float)(rand() % 780 + 10);
	float y = (float)(rand() % 580 + 10);
	//float th = (float)(rand() % 359);

	movePoint = sf::Vector2f(x, y);
	std::cout << "Moving to: " << movePoint.x << " " << movePoint.y << std::endl;
	RotationAngle();
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

	if (angle > 360)
	{
		angle -= 360;
	}

}

void SmartTank::selectTarget()
{
	float dis = 10000;
	float tempDis = 0;
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
		//reset state
	}
}