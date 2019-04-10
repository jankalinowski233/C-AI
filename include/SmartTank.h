#pragma once

#include "aitank.h"
#include "Node.h"
#include "Map.h"

class SmartTank : public AITank
{
private:
	bool forwards;

	bool tankSpottedFlag = false;
	bool scanFlag = false;
	bool scanCheckpoint = false;
	bool eBaseSpottedFlag = false;
	bool fBaseSpottedFlag = false;
	bool lineOfSightFlag = false;
	bool firingFlag = false;
	bool movementTargetFound = false;

	float startPoint;
	float checkpoint;

	std::vector<sf::Vector2f> eBaseLocations;
	std::vector<sf::Vector2f> fBaseLocations;
	sf::Vector2f eBaseCurrentTarget;

	sf::Vector2f movePoint;
	float distance = 5.f;
	float angle;

	enum TankState { IDLE, MOVE, ROTATE_TURRET, ROTATE_TANK, TARGET_FOUND, AIM, FIRE } state;

	std::list<Node> path;
	Node* current;
	Node* goal;

public:
	SmartTank();
	~SmartTank();

	void move();
	void reset();
	void collided();
	void markEnemy(Position p);
	void markBase(Position p);
	void markShell(Position p);
	void markTarget(Position p);
	bool isFiring();
	void score(int thisScore, int enemyScore);

	void DeleteBase(Position p);
	void DemarkEnemy();

	void RotateTurretToTarget();
	void ResetTurretDir();

	void SetMoveTarget();
	void SetCurrentPos();
	void ScanForTarget();
	void RotationAngle();

	void selectTarget();

	void updateWalls(Position p);

	Node *n;
	Map *m;

};