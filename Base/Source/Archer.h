#pragma once
#include <vector>
#include <string>

#include "Vector3.h"
#include "Message.h"
#include "Healer.h"

using std::vector;
using std::string;

class Archer : public Healable
{
private:

public:
	Archer();
	~Archer();
	messages dmgArcher(float dmg);


	
};
