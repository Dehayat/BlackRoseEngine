#pragma once
#include <random>


struct GUID {
	std::uint64_t id;
	GUID(std::uint64_t id) {
		this->id = id;
	}

	static std::uint64_t Generate() {
		std::random_device rd;
		std::mt19937_64 gen(rd());
		std::uniform_int_distribution<uint64_t> dis;
		return dis(gen);
	}
};