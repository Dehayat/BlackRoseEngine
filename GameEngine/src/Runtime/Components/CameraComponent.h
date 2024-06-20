#pragma once
#include <string>
#include <ryml/ryml.hpp>
#include <glm/glm.hpp>

#include "Reflection/Reflection.h"

using namespace glm;

struct CameraComponent {
	float height;
	bool startCamera;

	glm::mat3 worldToScreen;
	glm::mat3 camToScreen;

	CameraComponent(float height = 10) {
		this->height = height;
		camToScreen = glm::mat3();
		worldToScreen = glm::mat3();
		startCamera = false;
	}
	CameraComponent(ryml::NodeRef node)
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
	ROSE_EXPOSE_VARS(CameraComponent,(height)(startCamera))
};