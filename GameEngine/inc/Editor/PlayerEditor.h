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
		if (player.idleAnim.capacity() < 21) {
			player.idleAnim.reserve(21);
		}
		ImGui::InputText("idle Animation", &player.idleAnim[0], 21, ImGuiInputTextFlags_CallbackResize, ResizeStringCallback, &player.idleAnim);

		if (player.runAnim.capacity() < 21) {
			player.runAnim.reserve(21);
		}
		ImGui::InputText("run Animation", &player.runAnim[0], 21, ImGuiInputTextFlags_CallbackResize, ResizeStringCallback, &player.runAnim);
	}
};