#pragma once

#include "Physics/Physics.h"

#include "Components/PhysicsBodyComponent.h"
#include "Components/TransformComponent.h"

#include "Editor/ComponentEditor.h"

class PhysicsEditor :public IComponentEditor {
public:
	void Editor(entt::entity entity) {
		auto& registry = GETSYSTEM(Entities).GetRegistry();
		auto& physics = GETSYSTEM(PhysicsSystem);
		auto& trx = registry.get<TransformComponent>(entity);
		auto& phys = registry.get<PhysicsBodyComponent>(entity);
		physics.CopyTransformToBody(phys, trx);
		auto pos = b2Vec2(trx.globalPosition.x, trx.globalPosition.y);
		phys.body->SetTransform(pos, radians(trx.globalRotation));
		bool changeSize = ImGui::DragFloat("size x", &phys.size.x, 0.1f, 0.01f, 2000);
		changeSize |= ImGui::DragFloat("size y", &phys.size.y, 0.1f, 0.01f, 2000);
		if (changeSize) {
			if (phys.size.x <= 0.01) {
				phys.size.x = 0.01;
			}
			if (phys.size.y <= 0.01) {
				phys.size.y = 0.01;
			}
		}
		if (ImGui::Checkbox("Static", &phys.isStatic)) {
			if (phys.isStatic) {
				phys.body->SetType(b2BodyType::b2_staticBody);
			}
			else {
				phys.body->SetType(b2BodyType::b2_dynamicBody);
			}
		}
	}
};