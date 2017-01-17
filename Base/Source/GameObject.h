#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "Vector3.h"

struct GameObject
{
	enum GAMEOBJECT_TYPE
	{
		GO_NONE = 0,
		GO_BALL,
		GO_CUBE,
		GO_TOTAL, //must be last
	};
	GAMEOBJECT_TYPE type;
	Vector3 pos;
	Vector3 vel;
	Vector3 scale;
	bool active;
	float mass;

	GameObject(GAMEOBJECT_TYPE typeValue = GO_BALL);
	~GameObject();

	Vector3 GetPosition()
	{
		return pos;
	}
	void SetPosition(float x, float y, float z)
	{
		pos.x = x;
		pos.y = y;
		pos.z = z;
	}
};

#endif