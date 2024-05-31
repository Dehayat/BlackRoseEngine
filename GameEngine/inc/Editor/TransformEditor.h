#pragma once

#include "Systems.h"

#include "Entity.h"

#include "Transform.h"

#include "ComponentEditor.h"

class TransformEditor :public IComponentEditor {
public:

	void Editor(entt::entity entity)
	{
		auto& registry = GETSYSTEM(Entities).GetRegistry();
		auto& trx = registry.get<TransformComponent>(entity);
		ImGui::Text("Position");
		ImGui::DragFloat("X Position", &trx.position.x, 0.2f);
		ImGui::DragFloat("Y Position", &trx.position.y, 0.2f);
		ImGui::Text("Scale");
		ImGui::DragFloat("X Scale", &trx.scale.x, 0.2f);
		ImGui::DragFloat("Y Scale", &trx.scale.y, 0.2f);
		ImGui::DragFloat("Rotation", &trx.rotation, 5.f);
	}

	static void SetParent(entt::registry& registry, TransformComponent& trx, std::optional<entt::entity> newParent)
	{
		trx.parent = newParent;
		trx.hasParent = false;
		trx.level = 0;
		trx.matrix = TransformSystem::CalcMatrix(trx);
		if (newParent && registry.valid(newParent.value())) {
			trx.hasParent = true;
			trx.matrix = TransformSystem::CalcMatrix(trx);
			auto& parentTrx = registry.get<TransformComponent>(trx.parent.value());
			trx.matrix = trx.matrix * parentTrx.matrix;
			trx.level = parentTrx.level + 1;
		}
	}
};