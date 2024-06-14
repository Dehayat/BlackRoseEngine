#include "Scripting/PlayerScript.h"

void PlayerScript::Update(entt::entity owner) {
	auto& timeSystem = GETSYSTEM(TimeSystem);
	float dt = timeSystem.GetdeltaTime();
	if (!isAttacking && walkDir != 0) {
		auto& transform = GETSYSTEM(Entities).GetRegistry().get<TransformComponent>(owner);
		auto& player = GETSYSTEM(Entities).GetRegistry().get<PlayerComponent>(owner);
		transform.globalPosition.x += walkDir * dt * player.speed;
		transform.UpdateLocals();
	}
}

void PlayerScript::OnEvent(entt::entity owner, const EntityEvent& entityEvent) {

	if (entityEvent.name == "EnteringSensor") {
		Logger::Log("We did it boys");
	}
	if (entityEvent.name == "ExitingSensor") {
		Logger::Log("You blew it!");
	}

	auto& registry = GETSYSTEM(Entities).GetRegistry();
	int oldWalkDir = walkDir;
	auto& trx = registry.get<TransformComponent>(owner);
	if (entityEvent.name == "LeftKeyPressed") {
		if (!isAttacking) {
			walkDir = -1;
			trx.scale.x = -1;
			trx.UpdateGlobals();
		}
	}
	if (entityEvent.name == "LeftKeyReleased") {
		if (walkDir == -1) {
			walkDir = 0;
		}
	}
	if (entityEvent.name == "RightKeyPressed") {
		if (!isAttacking) {
			walkDir = 1;
			trx.scale.x = 1;
			trx.UpdateGlobals();
		}
	}
	if (entityEvent.name == "RightKeyReleased") {
		if (walkDir == 1) {
			walkDir = 0;
		}
	}
	if (!isAttacking && entityEvent.name == "LeftMousePressed") {
		if (registry.all_of<AnimationComponent, PlayerComponent>(owner)) {
			auto& anim = registry.get<AnimationComponent>(owner);
			auto& player = registry.get<PlayerComponent>(owner);
			anim.Play(player.attackAnim);
			isAttacking = true;
		}
	}
	if (entityEvent.name == "AnimationFinished") {
		if (isAttacking) {
			isAttacking = false;
			if (registry.all_of<AnimationComponent, PlayerComponent>(owner)) {
				auto& anim = registry.get<AnimationComponent>(owner);
				auto& player = registry.get<PlayerComponent>(owner);
				anim.Play(player.idleAnim);
				oldWalkDir = 0;
			}
		}
	}
	if (!isAttacking && oldWalkDir != walkDir) {
		if (registry.all_of<AnimationComponent, PlayerComponent>(owner)) {
			auto& anim = registry.get<AnimationComponent>(owner);
			auto& player = registry.get<PlayerComponent>(owner);
			if (walkDir == 0) {
				anim.Play(player.idleAnim);
			}
			else {
				anim.Play(player.runAnim);
			}
		}
	}
}
