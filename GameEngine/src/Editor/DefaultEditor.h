#pragma once
#include <imgui.h>

#include "ImguiHelper.h"

#include "Core/Entity.h"

#include "Core/Systems.h"

#include "Reflection/Reflection.h"

#include "ComponentEditor.h"

template<typename T>
class DefaultTypeEditor
{
public:
	static void Render(int entity, T* object)
	{
		Reflector<T> m;
		ImGui::PushID(entity);
		for(auto& varName : m.GetVarNames())
		{
			if(m.GetType(varName) == InfoTypes::INT)
			{
				ImGui::InputInt(varName.c_str(), (int*)m.GetVar(object, varName));
			}
			if(m.GetType(varName) == InfoTypes::STRING)
			{
				auto& str = *(std::string*)m.GetVar(object, varName);
				Imgui_InputText(varName.c_str(), str, 30);
			}
			if(m.GetType(varName) == InfoTypes::COLOR)
			{
				ImGui::ColorEdit4(varName.c_str(), (float*)m.GetVar(object, varName), ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_Float);
			}
			if(m.GetType(varName) == InfoTypes::FLOAT)
			{
				ImGui::InputFloat(varName.c_str(), (float*)m.GetVar(object, varName));
			}
			if(m.GetType(varName) == InfoTypes::VEC2)
			{
				ImGui::InputFloat2(varName.c_str(), (float*)m.GetVar(object, varName));
			}
			if(m.GetType(varName) == InfoTypes::BOOL)
			{
				ImGui::Checkbox(varName.c_str(), (bool*)m.GetVar(object, varName));
			}
			if(m.GetType(varName) == InfoTypes::GUID)
			{
				ImGui::LabelText(varName.c_str(), "%lld", *((int64*)m.GetVar(object, varName)));
			}
		}
		ImGui::PopID();
	}
};

template<typename T>
class DefaultComponentEditor:IComponentEditor
{
public:
	static void Render(entt::entity entity)
	{
		T& component = ROSE_GETSYSTEM(EntitySystem).GetRegistry().get<T>(entity);
		DefaultTypeEditor<T>::Render((int)entity, &component);
	}

	void Editor(entt::entity entity) override
	{
		T& component = ROSE_GETSYSTEM(EntitySystem).GetRegistry().get<T>(entity);
		DefaultTypeEditor<T>::Render((int)entity, &component);
	}
};