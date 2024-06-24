#pragma once
#include <optional>
#include <box2d/box2d.h>
#include <glm/glm.hpp>
#include <ryml/ryml.hpp>

#include "Reflection/Reflection.h"
#include "Reflection/Serialize.h"

using namespace glm;

struct PhysicsBodyComponent
{
	vec2 size;
	bool isStatic;
	bool isSensor;
	bool useGravity;

	vec2 globalSize;
	b2Body* body;
	b2PolygonShape shape;
	b2FixtureDef fixture;
	b2BodyDef bodyDef;

	PhysicsBodyComponent(vec2 size = vec2(1.f, 1.f), bool isStatic = false, bool isSensor = false, bool useGravity = true)
	{
		this->size = size;
		this->isStatic = isStatic;
		this->isSensor = isSensor;
		this->useGravity = useGravity;

		this->body = nullptr;
		globalSize = vec2();
	}
	PhysicsBodyComponent(ryml::NodeRef node)
	{
		size = {1.f,1.f};
		this->isStatic = false;
		this->isSensor = false;
		this->useGravity = true;

		globalSize = vec2();
		this->body = nullptr;

		ROSE_DESER(PhysicsBodyComponent);
	}
	void Serialize(ryml::NodeRef node)
	{
		ROSE_SER(PhysicsBodyComponent);
	}

	ROSE_EXPOSE_VARS(PhysicsBodyComponent, (size)(isStatic)(isSensor)(useGravity))
};