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

	void Attack(ArcherSystem& archer_system)
	{
		for (auto& archer : archer_system.GetArchers())
		{

		}
	}
};