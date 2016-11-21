#pragma once
#include "Vector3.h"
#include "Map.h"
#include "Base.h"
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

	Archer(Map& map, Base& base)
	:
	map(map),
	base(base)
	{
	}

	//Set a random position outside the walls for archers to roam to when hunting
	void SetRoamingPosition()
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
			const Vector3 archer_to_base = (base.GetPosition() - position);
			const float epsilon = 1;

			//if archer is close to base
			if (archer_to_base.LengthSquared() == epsilon * epsilon)
			{
				base.GrabBow();
				//if nighttime
				if (map.GetCurrTime() > 50)
				{
					state = A_STATE::REPOSITION;
				}
				else
				{
					SetRoamingPosition();
					state = A_STATE::HUNT;
				}
			}
			const float speed = 7;
			const Vector3 direction = archer_to_base.Normalized();
			position += direction * speed * time;
			break;
		}
		case A_STATE::HUNT:
		{
			const Vector3 archer_to_roam = roam_position - position;
			const float epsilon = 1;
			//if nighttime
			if (map.GetCurrTime() > 50)
			{
				state = A_STATE::REPOSITION;
			}
			//If reach roaming position
			else if (archer_to_roam.LengthSquared() == epsilon * epsilon)
			{
				SetRoamingPosition();
			}
			const float speed = 7;
			const Vector3 direction = archer_to_roam.Normalized();
			position += direction * speed * time;

			//rabbit hunting code here
			//for (auto& entity)

			break;
		}
		case A_STATE::DEFEND:
			break;
		case A_STATE::REPOSITION:
			break;
		}
	}

private:
	Base& base;
	Map& map;
	Vector3 position;
	A_STATE state;
	Vector3 roam_position;
};

class ArcherSystem
{
public:
	static const unsigned TOTAL_ARCHERS = 27;

	ArcherSystem(Map& map, Base& base)
	{
		for (unsigned i = 0; i < TOTAL_ARCHERS; ++i)
		{
			archers.push_back(Archer(map, base));
		}
	}
	~ArcherSystem()
	{
	}

	std::vector<Archer>& GetArchers()
	{
		return archers;
	}

private:
	std::vector<Archer> archers;
};