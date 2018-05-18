#include "DieRoller.h"

void DieRoller::SeedRNG(unsigned int seedVal)
{
	rng.seed(seedVal);
}

DieRoller::DieRoller()
{

}

int DieRoller::RollDN(int N)
{
	int random = 1;

	//create a uniform integer distribution between 1 and N
	std::uniform_int_distribution<int> D1(1, N);

	//roll a random number using our distribution and rng
	random = D1(rng);

	return random;
}