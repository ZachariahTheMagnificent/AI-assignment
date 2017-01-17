#pragma once
#include <vector>
#include "Healer.h"
#include "Archer.h"

class UnderWatch
{
private:
	Archer archer;
	Healer healer;

public:
	void damageArcher(float dmg);
	void damageHealer(float dmg);
	Archer& GetArcher();
	Healer& GetHealer();
};