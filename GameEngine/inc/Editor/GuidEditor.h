#pragma once
#include <imgui.h>

#include "Entity.h"

#include "Systems.h"

#include "Components/GUIDComponent.h"

#include "Editor/ComponentEditor.h"

class GuidEditor :public IComponentEditor {
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
		auto& guid = registry.get<GUIDComponent>(entity);
		if (guid.name.capacity() < 21) {
			guid.name.reserve(21);
		}
		ImGui::InputText(("Name##" + std::to_string(guid.id)).c_str(), &guid.name[0], 21, ImGuiInputTextFlags_CallbackResize, ResizeStringCallback, &guid.name);
	}
};