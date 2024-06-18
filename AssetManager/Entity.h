#pragma once
#include <unordered_map>
#include <random>

typedef std::uint64_t Guid;

class Entities {
private:
	std::random_device rd;
	std::mt19937_64 gen;
	std::uniform_int_distribution<Guid> dis;

public:
	Guid GenerateGuid() {
		gen = std::mt19937_64(rd());
		return dis(gen);
	}
};