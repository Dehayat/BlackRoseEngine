#pragma once
#ifdef _EDITOR
#include "Transform.h"

class TransformEditor {
public:
	static void SetParent(entt::registry& registry,Transform& trx, std::optional<entt::entity> newParent)
	{
		trx.parent = newParent;
		trx.hasParent = false;
		trx.level = 0;
		trx.matrix = TransformSystem::CalcMatrix(trx);
		if (newParent && registry.valid(newParent.value())) {
			trx.hasParent = true;
			trx.matrix = TransformSystem::CalcMatrix(trx);
			auto& parentTrx = registry.get<Transform>(trx.parent.value());
			trx.matrix = trx.matrix * parentTrx.matrix;
			trx.level = parentTrx.level + 1;
		}
	}
	static void DrawEditor(Transform& trx) {
		ImGui::DragFloat("Position X", &trx.position.x, 0.2f);
		ImGui::DragFloat("Position y", &trx.position.y, 0.2f);
		ImGui::DragFloat("Scale x", &trx.scale.x, 0.2f);
		ImGui::DragFloat("Scale y", &trx.scale.y, 0.2f);
		ImGui::DragFloat("Rotation", &trx.rotation, 5.f);
	}
};
#endif