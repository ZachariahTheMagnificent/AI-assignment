#include "CheckIfVectorsCollide.h"

bool CheckIfVectorsCollide(const Vector3& orgin1, const Vector3& vector1, const Vector3& orgin2, const Vector3& vector2)
{
	//http://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect

	//Given that the intersection is equal to orgin1 + vector1 * t = orgin2 + vector2 * u:
	const auto t = (orgin2 - orgin1).Cross(vector2).z / vector1.Cross(vector2).z;
	const auto u = (orgin1 - orgin2).Cross(vector1).z / vector2.Cross(vector1).z;

	return t >= 0 && t <= 1 && u >= 0 && u <= 1;
}