#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "Vector3.h"
#include "Mesh.h"
#include "Vertex.h"

struct GameObject
{
	enum GAMEOBJECT_TYPE
	{
		GO_NONE = 0,
		GO_BALL,
		GO_WALL,
		GO_WALL2,
		GO_WALL3,
		GO_START,
		GO_GOAL,
		GO_PILLAR,
		GO_POINT,
		GO_CUBE,
		GO_ASTEROID,
		GO_ASTEROID2,
		GO_SHIP, //player ship
		GO_BULLET, //player bullet
		GO_MISSILE, //player bullet
		GO_BLACKHOLE,
		GO_BLACKHOLE2,
		GO_SHIELD,
		GO_SHIELD2,
		GO_FIRE,
		GO_BEAMWAVE,
		GO_TOTAL, //must be last
	};

	GAMEOBJECT_TYPE type;
	Vector3 pos;
	Vector3 vel;
	Vector3 scale;
	Vector3 normal;
	Mesh* mesh;
	Color color;
	bool active;
	float mass;
	float time;

	//GameObject(GAMEOBJECT_TYPE typeValue = GO_POINT);
	GameObject(GAMEOBJECT_TYPE typeValue = GO_BALL);
	~GameObject();
};

#endif