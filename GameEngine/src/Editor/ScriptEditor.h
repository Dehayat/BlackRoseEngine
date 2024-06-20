#pragma once
#pragma once
#include <imgui.h>

#include "Core/Entity.h"

#include "Core/Systems.h"

#include "Components/ScriptComponent.h"
#include "Scripting/ScriptSystem.h"

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
		auto& registry = ROSE_GETSYSTEM(Entities).GetRegistry();
		auto& scriptComp = registry.get<ScriptComponent>(entity);
		ImGui::Text("Scripts");
		std::string removeScript = "";
		for (auto& script : scriptComp.scripts) {
			if (ImGui::BeginChild(script.c_str(), ImVec2(0,35),true)) {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5, 0.2, 0.2, 1));
				if (ImGui::Button(("Remove##" + script).c_str())) {
					removeScript = script;
				}
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::Text(script.c_str());
			}
			ImGui::EndChild();
		}
		if (removeScript != "") {
			ROSE_GETSYSTEM(ScriptSystem).RemoveScript(entity, removeScript);
		}
		ImGui::Separator();
		static std::string scriptName = "";
		if (scriptName.capacity() < 21) {
			scriptName.reserve(21);
		}
		ImGui::InputText("script", &scriptName[0], 21, ImGuiInputTextFlags_CallbackResize, ResizeStringCallback, &scriptName);
		if (ImGui::Button("AddScript")) {
			scriptComp.scripts.insert(scriptName);
			ROSE_GETSYSTEM(ScriptSystem).RefreshScript(entity);
			scriptName = "";
		}
	}
};