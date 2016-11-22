#pragma once
#include "Vector3.h"

class Base
{
public:
	Base(const float half_size_x, const float half_size_y)
	:
	half_size_x(half_size_x),
	half_size_y(half_size_y)
	{
	}

	Vector3 GetPosition()
	{
		return position;
	}

	void Update(const double time)
	{
		if (time == 1.f)
		{
			++num_bows;
		}
	}

	//Used by archers
	void GrabBow()
	{
		--num_bows;
	}

	unsigned GetNumBows()
	{
		return num_bows;
	}

	float StartingRegionX()
	{
		return position.x - half_size_x;
	}

	float StartingRegionY()
	{
		return position.y - half_size_y;
	}

	float EndingRegionX()
	{
		return position.x + half_size_x;
	}

	float EndingRegionY()
	{
		return position.y + half_size_y;
	}

private:
	Vector3 position;
	float half_size_x;
	float half_size_y;
	unsigned num_bows;
};