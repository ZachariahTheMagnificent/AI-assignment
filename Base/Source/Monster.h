#pragma once
#include "Archer.h"

class Monster
{
public:
	enum class M_STATE
	{
		ATK_ARCHER,
		ATK_STRUCTURE,
		FLEE,
		DEAD,
		END
	};
	const float Monsterspeed = 10.0f;

	Monster(Map& map, Base& base, RNG &rng, ArcherSystem &archerSystem)
		:
		map(map),
		base(base),
		rand(rng),
		archerSystem(archerSystem)
	{
		state = M_STATE::ATK_STRUCTURE;
		position = GetRandomPositionOutsideBase();
		monsterVel = Vel(Monsterspeed);

	}

	//void Attack(ArcherSystem& archer_system)
	//{
	//	for (auto& archer : archer_system.GetArchers())
	//	{

	//	}
	//}

	void Update(const double time)
	{
		switch (state)
		{
		case M_STATE::ATK_STRUCTURE:
		{
			base.GetPosition();
			break;
		}
		case M_STATE::ATK_ARCHER:
		{
			for (auto& archer : archerSystem.GetArchers())
			{
				archer.GetPosition();
			}
		}
		case M_STATE::FLEE:
		{
			position -= monsterVel*time;
		}
		case M_STATE::DEAD:
		{
			time_until_revive -= time;
			if (time_until_revive <= 0)
			{
				Revive();
			}
			break;
		}
		}
	}

	Vector3 Pos()
	{
		return position;
	}
	M_STATE GetState() const
	{
		return state;
	}
private:
	Base& base;
	Map& map;
	ArcherSystem& archerSystem;
	Vector3 position;
	Vector3 monsterVel;
	M_STATE state;
	Vector3 roam_position;
	RNG& rand;
	double stoptimer;
	double stoptime;
	double time_until_revive;

	void Revive()
	{
		if (state == M_STATE::DEAD)
		{
			state = M_STATE::ATK_STRUCTURE;
			position = GetRandomPositionOutsideBase();
			monsterVel = Vel(Monsterspeed);
			stoptimer = 0.0f;
		}
	}

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
};

class MonsterSystem
{
public:
	static const unsigned TOTAL_MONSTERS = 20;

	MonsterSystem(Map& map, Base& base, RNG& rng, ArcherSystem& archer)
		:
		map(map),
		base(base)
	{
		for (unsigned i = 0; i < TOTAL_MONSTERS; ++i)
		{
			monsters.push_back(Monster(map, base, rng, archer));
		}
	}
	~MonsterSystem()
	{
	}

	void Update(const double delta_time)
	{
		if (map.IsDaytime())
		{
			for (auto& monster : monsters)
			{
				monster.Update(delta_time);
			}
		}
	}

	std::vector<Monster>& GetMonsters()
	{
		return monsters;
	}

private:
	Map& map;
	Base& base;
	std::vector<Monster> monsters;
};

