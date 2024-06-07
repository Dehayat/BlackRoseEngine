#pragma once
#include <ryml/ryml.hpp>
#include <string>

struct PlayerComponent {
	float speed;
	std::string idleAnim;
	std::string runAnim;
	PlayerComponent(float speed = 10) {
		this->speed = speed;
	}
	PlayerComponent(ryml::NodeRef& node) {
		speed = 10;
		if (!node.is_map()) {
			return;
		}
		if (node.has_child("speed")) {
			node["speed"] >> speed;
		}
		if (node.has_child("idleAnim")) {
			node["idleAnim"] >> idleAnim;
		}
		if (node.has_child("runAnim")) {
			node["runAnim"] >> runAnim;
		}
	}
	void Serialize(ryml::NodeRef& node) {
		node |= ryml::MAP;
		auto spNode = node.append_child();
		spNode.set_key("speed");
		node["speed"] << speed;
		node["idleAnim"] << idleAnim;
		node["runAnim"] << runAnim;
	}
};