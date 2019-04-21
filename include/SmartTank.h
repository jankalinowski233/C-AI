#pragma once

#include "aitank.h"
#include "Node.h"
#include "Map.h"

class SmartTank : public AITank
{
private:
	bool forwards; //move tank forward

	//behaviour flags
	bool tankSpottedFlag = false;
	bool scanFlag = false;
	bool scanCheckpoint = false;
	bool eBaseSpottedFlag = false;
	bool fBaseSpottedFlag = false;
	bool lineOfSightFlag = false;
	bool firingFlag = false;
	bool movementTargetFound = false;

	//checkpoints to rotate turret
	float startPoint;
	float checkpoint;

	//vectors of buildings' positions
	std::vector<sf::Vector2f> eBaseLocations;
	std::vector<sf::Vector2f> fBaseLocations;
	//current target position
	sf::Vector2f eBaseCurrentTarget;

	sf::Vector2f movePoint; //destination
	//stopping distance
	float distance = 5.f;
	//angle to rotate towards
	float angle;

	//finite state machine
	enum TankState { IDLE, MOVE, ROTATE_TURRET, ROTATE_TANK, TARGET_FOUND, AIM, FIRE } state;

	//A* path
	std::list<Node> path;
	//A* nodes
	Node* current;
	Node* goal;

public:
	SmartTank();
	~SmartTank();

	void move(); //!< Move tank
	void reset(); //!< Reset tank
	void collided(); //!< Prevent tank from being stuck
	void markEnemy(Position p); //!< Record enemy position
	void markBase(Position p); //!< Record friendly base position
	void markShell(Position p); //!< Record shell position
	void markTarget(Position p); //!< Record enemy base position
	bool isFiring(); //!< Fire a shell
	void score(int thisScore, int enemyScore); //!< Handle score

	void DeleteBase(Position p); //!< Deletes the stored base from the vector
	void DemarkEnemy(); //!< Deletes enemy position

	void RotateTurretToTarget(); //!< Rotates turret towards current target
	void ResetTurretDir(); //!< Sets turret to always face forward
	
	void SetMoveTarget(); //!< Chooses new destination
	void SetCurrentPos(); //!< Marks current tank's position (current node)
	void ScanForTarget(); //!< Rotates turret 360* around the tank
	void RotationAngle(); //!< Rotates the tank towards next node in the path

	void selectTarget(); //!< Chooses target to aim at
	bool OutOfAmmo(); //!< Checks for ammo

	void updateWalls(Position p); //!< Updates wall locations
	void updateWalls(sf::Vector2f pos); //!< Updates wall locations

	//Node
	Node *n;
	//Map
	Map *m;

};