#pragma once
#pragma once
#include <imgui.h>

#include "Entity.h"

#include "Systems.h"

#include "Components/PlayerComponent.h"

#include "Editor/ComponentEditor.h"

class PlayerEditor :public IComponentEditor {
	static int ResizeStringCallback(ImGuiInputTextCallbackData* data)
	{
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
		{
			auto str = (std::string*)(data->UserData);
			str->resize(data->BufTextLen);
		}
		return 0;
	}
public:
	void Editor(entt::entity entity) {
		auto& registry = GETSYSTEM(Entities).GetRegistry();
		auto& player = registry.get<PlayerComponent>(entity);
		ImGui::SliderFloat("Speed", &player.speed, 0, 10);
	}
};