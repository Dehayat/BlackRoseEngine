#pragma once
#include <ryml/ryml.hpp>

struct Player {
	float speed;
	int input = 0;
	Player(float speed = 10) {
		this->speed = speed;
		input = 0;
	}
	Player(ryml::NodeRef node) {
		speed = 10;
		input = 0;
	}
};