#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "Vector3.h"

struct GameObject
{
	Vector3 pos;
	Vector3 vel;
	Vector3 scale { 1.f,1.f,1.f };
	float mass { 1.f };

	GameObject ( ) = default;
	~GameObject ( ) = default;
};

#endif