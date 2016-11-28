#pragma once
#include "Vector3.h"
#include "Random.h"

class Base
{
public:
	Base(RNG& rng, const float position_x, const float position_y, const float half_size_x, const float half_size_y)
	:
	rng(rng),
	position(position_x, position_y, 0),
	half_size_x(half_size_x),
	half_size_y(half_size_y),
	//num_bows(0),
	time_until_bow_spawn(rng.RandFloat(MIN_BOW_SPAWN, MAX_BOW_SPAWN))
	{
	}

	Vector3 GetPosition()
	{
		return position;
	}

	void Update(const double time)
	{
		if (num_bows < MAX_BOWS)
		{
			time_until_bow_spawn -= time;
			if (time_until_bow_spawn <= 0)
			{
				++num_bows;
				time_until_bow_spawn = rng.RandFloat(MIN_BOW_SPAWN, MAX_BOW_SPAWN);
			}
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
	RNG& rng;
	Vector3 position;
	float half_size_x;
	float half_size_y;
	unsigned num_bows;
	const unsigned MAX_BOWS = 7;
	const float MIN_BOW_SPAWN = 2;
	const float MAX_BOW_SPAWN = 5;
	double time_until_bow_spawn;
};