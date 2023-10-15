#pragma once
#include <string>
#include <ryml/ryml.hpp>
#include <glm/glm.hpp>
using namespace glm;

struct Camera {
	float height;
	bool startCamera;

	glm::mat3 worldToScreen;
	glm::mat3 camToScreen;

	Camera(float height = 10) {
		this->height = height;
		camToScreen = glm::mat3();
		worldToScreen = glm::mat3();
		startCamera = false;
	}
	Camera(ryml::NodeRef node)
	{
		height = 10;
		startCamera = false;

		camToScreen = glm::mat3();
		worldToScreen = glm::mat3();

		if (node.is_map()) {
			if (node.has_child("height")) {
				node["height"] >> this->height;
			}
			if (node.has_child("startCamera")) {
				node["startCamera"] >> this->startCamera;
			}
		}
	}
	void Serialize(ryml::NodeRef node)
	{
		node |= ryml::MAP;
		node["height"] << height;
		if (startCamera) {
			node["startCamera"] << startCamera;
		}
	}
};