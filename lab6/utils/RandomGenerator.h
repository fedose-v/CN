#pragma once
#include <random>
#include <ctime>

class RandomGenerator
{
private:
	std::mt19937 m_gen;
	std::uniform_int_distribution<> m_dist;

public:
	RandomGenerator()
		: m_gen(std::random_device{}())
		, m_dist(1, 10)
	{
	}

	bool ShouldDropPacket()
	{
		return m_dist(m_gen) <= 5;
	}
};