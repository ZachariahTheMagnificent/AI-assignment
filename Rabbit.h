#pragma once
#include "Vector3.h"
#include "Map.h"
#include "Base.h"
#include "Random.h"
#include <vector>

//Defenders of the base
class Rabbit
{
public:
	enum class A_STATE
	{
		ROAMING,
		DEAD,
		END
	};

	Rabbit(Map& map, Base& base, RNG &rng)
		:
		map(map),
		base(base),
		rand(rng)
	{
		state = A_STATE::ROAMING;
		SetRoamingPosition();
	}

	//Set a random position outside the walls for archers to roam to when hunting
	Vector3 GetRandomPositionOutsideBase()
	{
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
			roam_position.x = Math::RandFloatMinMax(map_starting_region_x, map_ending_region_x);

			//If value is outside base
			if (roam_position.x < base_starting_region_x || roam_position.x > base_ending_region_x)
			{
				break;
			}
		}
		while (true)
		{
			roam_position.y = Math::RandFloatMinMax(map_starting_region_y, map_ending_region_y);
			if (roam_position.y < base_starting_region_y || roam_position.y > base_ending_region_y)
			{
				break;
			}
		}
		const float Rabbitspeed = 1.0f;
		rabbitVel = Vel(Rabbitspeed);
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
					SetRoamingPosition();
				}
				break;
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