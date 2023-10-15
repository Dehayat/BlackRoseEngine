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
		if (node.is_map() && node.has_child("speed")) {
			node["speed"] >> speed;
		}
		input = 0;
	}
	void Serialize(ryml::NodeRef node) {
		node |= ryml::MAP;
		auto spNode = node.append_child();
		spNode.set_key("speed");
		node["speed"] << speed;
	}
};