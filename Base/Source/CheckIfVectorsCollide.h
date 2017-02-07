#pragma once
#include "Vector3.h"

bool CheckIfVectorsCollide(const Vector3& orgin1, const Vector3& vector1, const Vector3& orgin2, const Vector3& vector2);

struct Line
{
	Vector3 orgin;
	Vector3 vector;
};