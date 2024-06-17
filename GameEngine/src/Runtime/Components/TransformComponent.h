#pragma once
#include <glm/glm.hpp>
#include <entt/entity/entity.hpp>
#include <ryml/ryml.hpp>

#include "Core/Entity.h"

#include "Core/Reflection.h"

#include "Components/Components.h"

using namespace glm;

struct TransformComponent :IComponent {
	vec2 position;
	vec2 scale;
	float rotation;
	Guid parentGUID;

	bool hasParent;
	int level;
	entt::entity parent;
	mat3 matrixL2W;
	vec2 globalPosition;
	vec2 globalScale;
	vec2 scaleSign;
	float globalRotation;

	static vec2 GetScale(mat3 matrix);
	static float GetRotation(mat3 matrix);
	static float GetRotation(mat3 matrix, float angle);
	static vec2 GetPosition(mat3 matrix, vec2 localPos = vec2(0, 0));
	static vec2 GetDir(mat3 matrix, vec2 localDir);
	static mat3 MakeRotMatrix(float angle);
	mat3 MakeScaleMatrix(vec2 scale);

	TransformComponent(vec2 position = vec2(0, 0), vec2 scale = vec2(1, 1), float rotation = 0, entt::entity parent = NoEntity());
	TransformComponent(ryml::NodeRef& node);

	void Serialize(ryml::NodeRef node);
	void CalcMatrix();
	void UpdateGlobals();
	void UpdateLocals();

	ROSE_EXPOSE_VARS(TransformComponent,(position)(scale)(rotation))
};