#pragma once
#ifndef RABBIT_H_
#define RABBIT_H_
#include "Vector3.h"
#include "Map.h"
#include "Base.h"
#include "Random.h"
#include "CheckIfVectorsCollide.h"
#include <vector>

//Defenders of the base
class Rabbit
{
public:
	enum class A_STATE
	{
		ROAMING,
		STOP,
		DEAD,
		END
	};

	const float Rabbitspeed = 10.0f;

	Rabbit(Map& map, Base& base, RNG &rng)
		:
		map(map),
		base(base),
		rand(rng)
	{
		state = A_STATE::ROAMING;
		position = GetRandomPositionOutsideBase();
		SetRoamingPosition();
		rabbitVel = Vel(Rabbitspeed);
		stoptimer = 0.0f;
	}

	bool GoingThroughBase()
	{
		const float base_start_region_x = base.StartingRegionX();
		const float base_end_region_x = base.EndingRegionX();
		const float base_start_region_y = base.StartingRegionY();
		const float base_end_region_y = base.EndingRegionY();
		const Vector3 base_bottom_left(base_start_region_x, base_end_region_y, 0);
		const Vector3 base_top_left(base_start_region_x, base_end_region_y, 0);
		const Vector3 base_bottom_right(base_end_region_x, base_end_region_y, 0);
		const Vector3 base_top_right(base_end_region_x, base_end_region_y, 0);
		const Vector3 top_wall_vector = base_top_right - base_top_left;
		const Vector3 bottom_wall_vector = base_bottom_right - base_bottom_left;
		const Vector3 left_wall_vector = base_top_left - base_bottom_left;
		const Vector3 right_wall_vector = base_top_right - base_bottom_right;

		const Vector3 rabbit_to_roam = roam_position - position;

		return CheckIfVectorsCollide(base_top_left, top_wall_vector, position, roam_position) ||
			CheckIfVectorsCollide(base_bottom_left, bottom_wall_vector, position, roam_position) ||
			CheckIfVectorsCollide(base_bottom_left, left_wall_vector, position, roam_position) ||
			CheckIfVectorsCollide(base_bottom_right, right_wall_vector, position, roam_position);
	}
	void SetRoamingPosition()
	{
		while (true)
		{
			roam_position = GetRandomPositionOutsideBase();

			if (GoingThroughBase() == false)
			{
				break;
			}
		}
	}
	//Set a random position outside the walls for archers to roam to when hunting
	Vector3 GetRandomPositionOutsideBase()
	{
		Vector3 random_position;

		//get the perimeter of the base.
		const float base_starting_region_x = base.StartingRegionX();
		const float base_starting_region_y = base.StartingRegionY();
		const float base_ending_region_x = base.EndingRegionX();
		const float base_ending_region_y = base.EndingRegionY();

		const float map_starting_region_x = map.StartingRegionX();
		const float map_starting_region_y = map.StartingRegionY();
		const float map_ending_region_x = map.EndingRegionX();
		const float map_ending_region_y = map.EndingRegionY();

		while (true)
		{
			//generate a value within the map perimeter.
			random_position.x = Math::RandFloatMinMax(map_starting_region_x, map_ending_region_x);

			//If value is outside base
			if (random_position.x < base_starting_region_x || random_position.x > base_ending_region_x)
			{
				break;
			}
		}
		while (true)
		{
			random_position.y = Math::RandFloatMinMax(map_starting_region_y, map_ending_region_y);
			if (random_position.y < base_starting_region_y || random_position.y > base_ending_region_y)
			{
				break;
			}
		}

		return random_position;
	}

	void Update(const double time)
	{
		switch (state)
		{
			case A_STATE::ROAMING:
			{
				position += rabbitVel*time;
				if (Dir().Dot(rabbitVel) < 0)
				{
					roam_position = GetRandomPositionOutsideBase();
					rabbitVel = Vel(Rabbitspeed);
				}
				const int randStop = rand.RandInt(0,100);
				if (randStop == 1 || randStop == 2)
				{
					//state = A_STATE::STOP;
				}
				break;
			}
			case A_STATE::STOP:
			{
				stoptimer += time;
				if (stoptimer >= 5.0f)
				{
					stoptimer = 0;
					state = A_STATE::ROAMING;
				}
			}
			case A_STATE::DEAD:
			{
				break;
			}
		}
	}
	Vector3 Pos()
	{
		return position;
	}
private:
	Base& base;
	Map& map;
	Vector3 position;
	Vector3 rabbitVel;
	A_STATE state;
	Vector3 roam_position;
	RNG& rand;
	double stoptimer;

	Vector3 Dir()
	{
		try
		{
			return (roam_position - position).Normalized();
		}
		catch (DivideByZero divide_by_zero)
		{
			return Vector3(0, 1, 0);
		}
	}
	Vector3 Vel(const float speed)
	{
		return Dir()*speed;
	}
};

class RabbitSystem
{
public:
	static const unsigned TOTAL_RABBITS= 10;

	RabbitSystem(Map& map, Base& base, RNG& rng)
	{
		for (unsigned i = 0; i < TOTAL_RABBITS; ++i)
		{
			rabbit.push_back(Rabbit(map, base, rng));
		}
	}
	~RabbitSystem()
	{
	}

	std::vector<Rabbit>& GetRabbits()
	{
		return rabbit;
	}

private:
	std::vector<Rabbit> rabbit;
};
#endif