#pragma once

#include "Core/Systems.h"

#include "Core/Entity.h"

#include "Core/Transform.h"

#include "ComponentEditor.h"
#include "Editor/DefaultEditor.h"

class TransformEditor :public IComponentEditor {
public:

	void Editor(entt::entity entity)
	{
		ROSE_INIT_VARS(TransformComponent);
		DefaultComponentEditor<TransformComponent>::Render(entity);
		//ImGui::Text("Position");
		//ImGui::DragFloat("X Position", &trx.position.x, 0.2f);
		//ImGui::DragFloat("Y Position", &trx.position.y, 0.2f);
		//ImGui::Text("Scale");
		//ImGui::DragFloat("X Scale", &trx.scale.x, 0.2f);
		//ImGui::DragFloat("Y Scale", &trx.scale.y, 0.2f);
		//ImGui::DragFloat("Rotation", &trx.rotation, 5.f);
	}

	static void SetParent(entt::registry& registry, TransformComponent& trx, entt::entity newParent)
	{
		trx.parent = newParent;
		trx.hasParent = false;
		trx.level = 0;
		trx.matrixL2W = TransformSystem::CalcMatrix(trx);
		if (registry.valid(newParent)) {
			trx.hasParent = true;
			trx.matrixL2W = TransformSystem::CalcMatrix(trx);
			auto& parentTrx = registry.get<TransformComponent>(trx.parent);
			trx.matrixL2W = trx.matrixL2W * parentTrx.matrixL2W;
			trx.level = parentTrx.level + 1;
		}
	}
};