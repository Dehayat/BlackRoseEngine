#pragma once
#pragma once
#include <imgui.h>

#include "Entity.h"

#include "Systems.h"

#include "Components/ScriptComponent.h"

#include "Editor/ComponentEditor.h"

class ScriptEditor :public IComponentEditor {
public:
	void Editor(entt::entity entity) {
		auto& registry = GETSYSTEM(Entities).GetRegistry();
	}
};