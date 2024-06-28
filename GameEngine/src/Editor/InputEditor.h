#pragma once

#include <imgui.h>
#include <entt/entity/registry.hpp>

#include "Core/Entity.h"
#include "Core/Systems.h"

#include "Components/InputComponent.h"

#include "Editor/ComponentEditor.h"


class InputEditor:public IComponentEditor
{
public:
	void Editor(entt::entity entity)
	{
		auto& registry = ROSE_GETSYSTEM(EntitySystem).GetRegistry();
		auto& inputComp = registry.get<InputComponent>(entity);
		ImGui::Text("Input Keys");
		int removeKey = -1;
		for(auto& key : inputComp.inputKeys)
		{
			if(ImGui::BeginChild(GetKeyName(key).c_str(), ImVec2(0, 35), true))
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5, 0.2, 0.2, 1));
				if(ImGui::Button(("Remove##" + GetKeyName(key)).c_str()))
				{
					removeKey = key;
				}
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::Text(GetKeyName(key).c_str());
			}
			ImGui::EndChild();
		}
		if(removeKey != -1)
		{
			inputComp.inputKeys.erase((InputKey)removeKey);
		}
		ImGui::Separator();
		std::string addKeyText = "Add Input Key";
		if(ImGui::BeginCombo("##InputKeys", addKeyText.c_str()))
		{
			for(int i = 0; i < keyCount; i++)
			{
				bool isSelected = false;
				if(ImGui::Selectable(GetKeyName((InputKey)i).c_str(), &isSelected))
				{
					inputComp.inputKeys.insert((InputKey)i);
				}
			}
			ImGui::EndCombo();
		}


		ImGui::Text("Input Mouse");
		int removeMouseButton = -1;
		for(auto& mouseButton : inputComp.inputMouseButtons)
		{
			if(ImGui::BeginChild(GetMouseButtonName(mouseButton).c_str(), ImVec2(0, 35), true))
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5, 0.2, 0.2, 1));
				if(ImGui::Button(("Remove##" + GetMouseButtonName(mouseButton)).c_str()))
				{
					removeMouseButton = mouseButton;
				}
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::Text(GetMouseButtonName(mouseButton).c_str());
			}
			ImGui::EndChild();
		}
		if(removeMouseButton != -1)
		{
			inputComp.inputMouseButtons.erase((InputMouse)removeMouseButton);
		}
		ImGui::Separator();
		std::string addmouseButtonText = "Add Input mouseButton";
		if(ImGui::BeginCombo("##InputmouseButtons", addmouseButtonText.c_str()))
		{
			for(int i = 0; i < mouseButtonCount; i++)
			{
				bool isSelected = false;
				if(ImGui::Selectable(GetMouseButtonName((InputMouse)i).c_str(), &isSelected))
				{
					inputComp.inputMouseButtons.insert((InputMouse)i);
				}
			}
			ImGui::EndCombo();
		}
	}
};