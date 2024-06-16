#pragma once
#pragma once
#include <imgui.h>

#include "Entity.h"

#include "Systems.h"

#include "Components/ScriptComponent.h"

#include "Editor/ComponentEditor.h"

class ScriptEditor :public IComponentEditor {
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
		auto& scriptComp = registry.get<ScriptComponent>(entity);
		if (scriptComp.script.capacity() < 21) {
			scriptComp.script.reserve(21);
		}
		ImGui::InputText("script", &scriptComp.script[0], 21, ImGuiInputTextFlags_CallbackResize, ResizeStringCallback, &scriptComp.script);
	}
};