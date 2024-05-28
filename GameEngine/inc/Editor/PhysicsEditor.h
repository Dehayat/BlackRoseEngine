#pragma once
#include "Components/PhysicsBodyComponent.h"
#include "Components/TransformComponent.h"
class PhysicsEditor {
public:
	static void DrawEditor(PhysicsBodyComponent& phys, TransformComponent& trx) {
		auto pos = b2Vec2(trx.position.x, trx.position.y);
		phys.body->SetTransform(pos, radians(trx.rotation));
		bool changeSize = ImGui::DragFloat("size x", &phys.size.x, 0.1f, 0.05f, 2000);
		changeSize |= ImGui::DragFloat("size y", &phys.size.y, 0.1f, 0.05f, 2000);
		if (changeSize) {
			phys.body->DestroyFixture(&phys.body->GetFixtureList()[0]);
			phys.shape.SetAsBox(phys.size.x, phys.size.y);
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