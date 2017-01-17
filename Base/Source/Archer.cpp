#include "Archer.h"



Archer::Archer()
{
	maxHP = 100;
	hp = maxHP;
	Vector3 defaultpos{ 0, 0, 0 };
	pos = defaultpos;
}
Archer::~Archer()
{

}
 messages Archer::dmgArcher(float dmg)
{
	hp -= dmg;

	if (hp <= 0)
	{
		hp = 0;
	}
	if (hp <= 0.4*maxHP)
	{
		return messages::Damaged;
	}
	else
	{
		return messages::NO_MESSAGE;
	}
}

