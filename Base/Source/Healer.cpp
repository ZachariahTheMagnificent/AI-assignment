#include "Healer.h"


Healer::Healer()
{
	maxHP = 100;
	hp = maxHP;
	Vector3 defaultpos{ 0, 0, 0 };
	pos = defaultpos;
}
Healer::~Healer()
{

}

messages Healer::dmgHealer(float dmg)
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

void Healer::healSomeone(Healable &healsomebody)
{
	if (hp > 0)
	{
		healsomebody.heal(healValue);
	}
}

bool Healable::fullyHealed()
{
	if (hp == maxHP)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Healable::heal(float healValue)
{
	hp += healValue;
	if (hp > maxHP)
	{
		hp = maxHP;
	}
}
float Healable::getHP()
{
	return hp;
}