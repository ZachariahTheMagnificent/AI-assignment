#pragma once
#include <vector>
#include <string>

#include "Vector3.h"
#include "Message.h"
#include "GameObject.h"

using std::vector;
using std::string;

class Healable : public GameObject
{
public:
	virtual bool fullyHealed();
	virtual void heal(float healValue);
	float getHP();
	float minHP();

protected:
	float maxHP;
	float hp;
};

class Healer : public Healable
{
private:
	const float healValue = 40;

public:
	Healer();
	~Healer();
	messages dmgHealer(float dmg);
	void healSomeone(Healable &healsomebody);

};
