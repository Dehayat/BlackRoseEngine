#pragma once
#include "Scripting/Script.h"

#include <entt/entt.hpp>

#include "TimeSystem.h"
#include "Entity.h"

#include "Systems.h"

#include "Components/TransformComponent.h"
#include "Components/PlayerComponent.h"
#include "Components/AnimationComponent.h"

class PlayerScript : public Script {
private:
	int walkDir = 0;
	bool isAttacking = false;
public:
	virtual void Update(entt::entity owner) override;
	virtual void OnEvent(entt::entity owner, const EntityEvent& entityEvent) override;
};