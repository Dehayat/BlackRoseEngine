#pragma once
#include <random>


struct GUIDComponent {
	std::uint64_t id;
	GUIDComponent() {
		this->id = Generate();
	}
	GUIDComponent(std::uint64_t id) {
		this->id = id;
	}

	static std::uint64_t Generate() {
		std::random_device rd;
		std::mt19937_64 gen(rd());
		std::uniform_int_distribution<uint64_t> dis;
		return dis(gen);
	}
};