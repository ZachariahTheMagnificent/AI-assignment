#pragma once
#include "Vector3.h"
#include "Map.h"
#include "Base.h"
#include "Rabbit.h"
#include "Arrow.h"
#include <vector>

//Defenders of the base
class Archer
{
public:
	enum class A_STATE
	{
		UNRECRUITED,
		GRAB_TOOL,
		HUNT,
		DEFEND,
		REPOSITION,
		END
	};

	Archer(Map& map, Base& base, RabbitSystem& rabbit_system, ArrowSystem& arrow_system)
	:
	map(map),
	base(base),
	rabbit_system(rabbit_system),
	arrow_system(arrow_system),
	state(A_STATE::UNRECRUITED),
	position(GetRandomPositionOutsideBase())
	{
	}

	bool GoingThroughBase()
	{
		const float base_start_region_x = base.StartingRegionX();
		const float base_end_region_x = base.EndingRegionX();
		const float base_start_region_y = base.StartingRegionY();
		const float base_end_region_y = base.EndingRegionY();

		const Vector3 base_bottom_left(base_start_region_x, base_start_region_y, 0);
		const Vector3 base_top_left(base_start_region_x, base_end_region_y, 0);
		const Vector3 base_bottom_right(base_end_region_x, base_start_region_y, 0);
		const Vector3 base_top_right(base_end_region_x, base_end_region_y, 0);

		const Vector3 top_wall_vector = base_top_right - base_top_left;
		const Vector3 bottom_wall_vector = base_bottom_right - base_bottom_left;
		const Vector3 left_wall_vector = base_top_left - base_bottom_left;
		const Vector3 right_wall_vector = base_top_right - base_bottom_right;

		const Vector3 rabbit_to_roam = roam_position - position;

		return CheckIfVectorsCollide(base_top_left, top_wall_vector, position, rabbit_to_roam) ||
			CheckIfVectorsCollide(base_bottom_left, bottom_wall_vector, position, rabbit_to_roam) ||
			CheckIfVectorsCollide(base_bottom_left, left_wall_vector, position, rabbit_to_roam) ||
			CheckIfVectorsCollide(base_bottom_right, right_wall_vector, position, rabbit_to_roam);
	}
	void SetRoamingPosition()
	{
		while (true)
		{
			roam_position = GetRandomPositionOutsideBase();

			if (position.x >= base.StartingRegionX() && position.x <= base.EndingRegionX() && position.y >= base.StartingRegionY() && position.y <= base.EndingRegionY())
			{
				break;
			}
			else if (GoingThroughBase() == false)
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
		case A_STATE::UNRECRUITED:
			//If there are tools for the archers to grab.
			if (base.GetNumBows() > 0)
			{
				state = A_STATE::GRAB_TOOL;
			}
			break;
		case A_STATE::GRAB_TOOL:
		{
			if (base.GetNumBows() == 0)
			{
				state = A_STATE::UNRECRUITED;
				break;
			}
			const Vector3 archer_to_base = (base.GetPosition() - position);
			const float epsilon = 1;

			//if archer is close to base
			if (archer_to_base.LengthSquared() <= epsilon * epsilon)
			{
				base.GrabBow();
				if (map.IsDaytime())
				{
					SetRoamingPosition();
					state = A_STATE::HUNT;
				}
				else
				{
					state = A_STATE::REPOSITION;
				}
			}

			const Vector3 direction = archer_to_base.Normalized();
			position += direction * speed * time;
			break;
		}
		case A_STATE::HUNT:
		{
			const Vector3 archer_to_roam = roam_position - position;
			const float epsilon = 1;

			//if nighttime
			if (!map.IsDaytime())
			{
				state = A_STATE::REPOSITION;
			}

			//If reach roaming position
			else if (archer_to_roam.LengthSquared() <= epsilon * epsilon)
			{
				SetRoamingPosition();
			}

			const Vector3 direction = archer_to_roam.Normalized();
			position += direction * speed * time;

			for (auto& rabbit : rabbit_system.GetRabbits())
			{
				if (rabbit.GetState() == Rabbit::A_STATE::DEAD)
				{
					continue;
				}
				const auto archer_to_rabbit = rabbit.Pos() - position;

				if (archer_to_rabbit.LengthSquared() <= radius*radius)
				{
					arrow_system.CreateArrow(position, archer_to_rabbit.Normalized());
				}
			}

			break;
		}
		case A_STATE::DEFEND:
			if (map.IsDaytime())
			{
				state = A_STATE::HUNT;
				break;
			}
			break;
		case A_STATE::REPOSITION:
			if (map.IsDaytime())
			{
				state = A_STATE::HUNT;
				break;
			}
			break;
		}
	}

	const Vector3& GetPosition() const
	{
		return position;
	}

	A_STATE GetState() const
	{
		return state;
	}

private:
	Base& base;
	Map& map;
	RabbitSystem& rabbit_system;
	ArrowSystem& arrow_system;
	const float speed = 7;
	const float radius = 30;
	Vector3 position;
	A_STATE state;
	Vector3 roam_position;
};

class ArcherSystem
{
public:
	static const unsigned TOTAL_ARCHERS = 27;

	ArcherSystem(Map& map, Base& base, RabbitSystem& rabbit_system, ArrowSystem& arrow_system)
	{
		for (unsigned i = 0; i < TOTAL_ARCHERS; ++i)
		{
			archers.push_back(Archer(map, base, rabbit_system, arrow_system));
		}
	}
	~ArcherSystem()
	{
	}

	void Update(const double delta_time)
	{
		for (auto& archer : archers)
		{
			archer.Update(delta_time);
		}
	}

	std::vector<Archer>& GetArchers()
	{
		return archers;
	}

private:
	std::vector<Archer> archers;
};