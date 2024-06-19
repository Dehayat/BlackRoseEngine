#pragma once
#include <random>

typedef std::uint64_t Guid;

class GuidGenerator {

	static std::random_device rd;
	static std::mt19937_64 gen;
	static std::uniform_int_distribution<Guid> dis;
public:
	static Guid New();
};