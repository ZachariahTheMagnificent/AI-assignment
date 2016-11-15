#include <random>

class Random
{
public:
	Random();

	int RandInt(const int min, const int max);
	float RandFloat(const float min, const float max);

private:
	//Weaker rng. Does not require seed
	std::random_device weak_rng;
	//Stronger rng. Requires seed
	std::mt19937 strong_rng;
};