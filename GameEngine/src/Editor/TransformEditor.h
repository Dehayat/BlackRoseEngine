#pragma once
#include "Core/Entity.h"

#include "Core/Transform.h"

#include "Editor/DefaultEditor.h"

class TransformEditor {
public:

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