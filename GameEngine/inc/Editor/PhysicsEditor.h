#pragma once
#include "Components/PhysicsBodyComponent.h"
#include "Components/TransformComponent.h"

#include "Editor/ComponentEditor.h"

class PhysicsEditor :public IComponentEditor {
public:
	void Editor(entt::entity entity) {
		auto& registry = GETSYSTEM(Entities).GetRegistry();
		auto& trx = registry.get<TransformComponent>(entity);
		auto& phys = registry.get<PhysicsBodyComponent>(entity);
		auto pos = b2Vec2(trx.position.x, trx.position.y);
		phys.body->SetTransform(pos, radians(trx.rotation));
		bool changeSize = ImGui::DragFloat("size x", &phys.size.x, 0.1f, 0.05f, 2000);
		changeSize |= ImGui::DragFloat("size y", &phys.size.y, 0.1f, 0.05f, 2000);
		if (changeSize) {
			if (phys.size.x <= 0.01) {
				phys.size.x = 0.01;
			}
			if (phys.size.y <= 0.01) {
				phys.size.y = 0.01;
			}
			phys.body->DestroyFixture(&phys.body->GetFixtureList()[0]);
			phys.shape.SetAsBox(phys.size.x / 2, phys.size.y / 2);
			phys.body->CreateFixture(&phys.fixture);
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