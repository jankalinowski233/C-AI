#include "SmartTank.h"
#include <iostream>

SmartTank::SmartTank()
{
}

SmartTank::~SmartTank()
{
}

void SmartTank::move()
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
}

void SmartTank::reset()
{
	SetMoveTarget();
}

void SmartTank::collided()
{
	SetMoveTarget();
}

void SmartTank::markEBase(Position p)
{
	eBaseLocations.push_back(sf::Vector2f(p.getX(), p.getY()));

	eBaseSpottedFlag = true;

	float deltaX = getX() - p.getX();
	float deltaY = getY() - p.getY();

	float angleInDegree = atan2(deltaY, deltaX) * 180 / PI; //gets the angle of the player tank in relation to the enemy 
	//turretAngle = angleInDegree + 180;
	//std::cout << "Target spotted at (" << p.getX() << ", " << p.getY() << ")\n";
}

void SmartTank::markEnemy(Position p)
{
}

void SmartTank::markBase(Position p)
{
}

void SmartTank::markShell(Position p)
{
}

void SmartTank::markTarget(Position p)
{
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

	float deltaX = getX() - eBaseCurrentTarget.x;
	float deltaY = getY() - eBaseCurrentTarget.y;

	float angleInDegree = atan2(deltaY, deltaX) * 180 / PI; //gets the angle of the player tank in relation to the enemy 
															//turretAngle = angleInDegree + 180;
}

void SmartTank::SetMoveTarget()
{
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
}