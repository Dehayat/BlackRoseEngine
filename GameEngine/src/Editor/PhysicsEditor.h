#pragma once

#include "Physics/Physics.h"
#include "Core/DisableSystem.h"

#include "Components/PhysicsBodyComponent.h"
#include "Components/TransformComponent.h"

#include "Editor/ComponentEditor.h"

class PhysicsEditor :public IComponentEditor
{
public:
	void Editor(entt::entity entity)
	{
		auto& registry = ROSE_GETSYSTEM(Entities).GetRegistry();
		auto& physics = ROSE_GETSYSTEM(PhysicsSystem);
		auto& disable = ROSE_GETSYSTEM(DisableSystem);
		auto& trx = registry.get<TransformComponent>(entity);
		auto& phys = registry.get<PhysicsBodyComponent>(entity);
		if (!disable.IsEnabled(entity) && phys.body != nullptr) {
			physics.RemoveBody(phys);
		}
		if (disable.IsEnabled(entity) && phys.body == nullptr) {
			physics.AddBody(entity, phys);
		}
		if (phys.body != nullptr)
		{
			physics.CopyTransformToBody(phys, trx);
			auto pos = b2Vec2(trx.globalPosition.x, trx.globalPosition.y);
			phys.body->SetTransform(pos, radians(trx.globalRotation));
		}
		bool changeSize = ImGui::DragFloat("size x", &phys.size.x, 0.1f, 0.01f, 2000);
		changeSize |= ImGui::DragFloat("size y", &phys.size.y, 0.1f, 0.01f, 2000);
		if (changeSize)
		{
			if (phys.size.x <= 0.01)
			{
				phys.size.x = 0.01;
			}
			if (phys.size.y <= 0.01)
			{
				phys.size.y = 0.01;
			}
		}
		if (ImGui::Checkbox("Static", &phys.isStatic))
		{
			if (phys.isStatic)
			{
				phys.bodyDef.type = b2_staticBody;
				if (phys.body != nullptr) {
					phys.body->SetType(phys.bodyDef.type);
				}
			}
			else if (phys.isSensor)
			{
				phys.bodyDef.type = b2_dynamicBody;
				if (phys.body != nullptr) {
					phys.body->SetType(phys.bodyDef.type);
				}
			}
			else
			{
				phys.bodyDef.type = b2_dynamicBody;
				if (phys.body != nullptr) {
					phys.body->SetType(phys.bodyDef.type);
				}
			}
		}
		if (ImGui::Checkbox("Sensor", &phys.isSensor))
		{
			phys.fixture.isSensor = phys.isSensor;
			phys.body->GetFixtureList()[0].SetSensor(phys.isSensor);
			if (phys.isStatic)
			{
				phys.bodyDef.type = b2_staticBody;
			}
			else if (phys.isSensor)
			{
				phys.bodyDef.type = b2_dynamicBody;
				phys.body->SetType(phys.bodyDef.type);
			}
			else
			{
				phys.bodyDef.type = b2_dynamicBody;
				phys.body->SetType(phys.bodyDef.type);
			}
			if (phys.body != nullptr) {
				phys.body->SetType(phys.bodyDef.type);
			}
		}
		if (ImGui::Checkbox("Use Gravity", &phys.useGravity))
		{
			if (phys.body != nullptr)
			{
				if (phys.useGravity)
				{
					phys.body->SetGravityScale(1.0f);
				}
				else {
					phys.body->SetGravityScale(0.0f);
				}
			}
		}
	}
};