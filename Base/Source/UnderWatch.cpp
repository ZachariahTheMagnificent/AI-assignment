#include "UnderWatch.h"

void UnderWatch::damageArcher(float dmg)
{
	messages MSG;
	MSG = archer.dmgArcher(dmg);

	switch (MSG)
	{
	case NO_MESSAGE:
		break;
	case Damaged:
		healer.healSomeone(archer);
		break;
	}
}
void UnderWatch::damageHealer(float dmg)
{
	messages MSG;
	MSG = healer.dmgHealer(dmg);

	switch (MSG)
	{
	case NO_MESSAGE:

		break;
	case Damaged:
		//tank.castTaunt();
		break;
	}
}
//void UnderWatch::damageTank(float dmg)
//{
//	messages MSG = tank.damage();
//	switch (MSG)
//	{
//	case NO_MESSAGE:
//
//		break;
//	case Damaged:
//		tank.healSomeone(tank);
//		break;
//	case Died:
//		std::cout << "tank has died!" << std::endl;
//		break;
//	}
//}
Archer& UnderWatch::GetArcher()
{
	return archer;
}

Healer& UnderWatch::GetHealer()
{
	return healer;
}