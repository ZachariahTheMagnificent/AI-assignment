#include "Random.h"

RNG::RNG()
{
	//use the weaker rng to seed the stronger rng
	strong_rng = std::mt19937(weak_rng());
}

int RNG::RandInt(const int min, const int max)
{
	std::uniform_int_distribution<int> ranged_rng(min, max);

	//Requires an rng engine to function
	return ranged_rng(strong_rng);
}

float RNG::RandFloat(const float min, const float max)
{
	std::uniform_real_distribution<float> ranged_rng(min, max);

	//Requires an rng engine to function
	return ranged_rng(strong_rng);
}