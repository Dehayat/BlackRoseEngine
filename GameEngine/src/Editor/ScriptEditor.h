#pragma once
#include <functional>

#include <imgui.h>

#include "ImguiHelper.h"

#include "Core/Entity.h"

#include "Core/Systems.h"

#include "Components/ScriptComponent.h"
#include "Scripting/ScriptSystem.h"

#include "Editor/ComponentEditor.h"


class ScriptEditor:public IComponentEditor
{
public:
	void Editor(entt::entity entity)
	{
		auto& registry = ROSE_GETSYSTEM(EntitySystem).GetRegistry();
		auto& scriptComp = registry.get<ScriptComponent>(entity);
		ImGui::Text("Scripts");
		std::string removeScript = "";
		for(auto& script : scriptComp.scripts)
		{
			if(ImGui::BeginChild(script.c_str(), ImVec2(0, 35), true))
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5, 0.2, 0.2, 1));
				if(ImGui::Button(("Remove##" + script).c_str()))
				{
					removeScript = script;
				}
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::Text(script.c_str());
			}
			ImGui::EndChild();
		}
		if(removeScript != "")
		{
			ROSE_GETSYSTEM(ScriptSystem).RemoveScript(entity, removeScript);
		}
		ImGui::Separator();
		static std::string scriptName = "Add Script";
		if(scriptName.capacity() < 21)
		{
			scriptName.reserve(21);
		}
		Imgui_AssetDropDown("Add Script", AssetType::Script, [entity, &scriptComp](AssetInfo asset)
			{
				scriptComp.scripts.insert(asset.first);
				ROSE_GETSYSTEM(ScriptSystem).RefreshScript(entity);
			});
	}
};