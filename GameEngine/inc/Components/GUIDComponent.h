#pragma once
#include <random>
#include "Entity.h"


struct GUIDComponent {
	Guid id;
	GUIDComponent() {
		this->id = Generate();
	}
	GUIDComponent(Guid id) {
		this->id = id;
	}

	static Guid Generate() {
		std::random_device rd;
		std::mt19937_64 gen(rd());
		std::uniform_int_distribution<Guid> dis;
		return dis(gen);
	}
};