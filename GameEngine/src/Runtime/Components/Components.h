#pragma once

#include <ryml/ryml.hpp>

class IComponent {
	virtual void Serialize(ryml::NodeRef node) = 0;
};