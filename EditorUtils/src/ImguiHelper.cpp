#include "ImguiHelper.h"

#include <entt/locator/locator.hpp>

#include <AssetPipline/AssetStore.h>
#include "Core/Systems.h"

static int ResizeStringCallback(ImGuiInputTextCallbackData* data)
{
	if(data->EventFlag == ImGuiInputTextFlags_CallbackResize)
	{
		auto str = (std::string*)(data->UserData);
		str->resize(data->BufTextLen);
	}
	return 0;
}


void Imgui_InputText(const char* label, std::string& buffer, int length, ImGuiInputTextFlags flags)
{
	if(buffer.capacity() < length + 1)
	{
		buffer.reserve(length + 1);
	}
	ImGui::InputText(label, &buffer[0], length + 1, ImGuiInputTextFlags_CallbackResize, ResizeStringCallback, &buffer);
}



void Imgui_AssetDropDown(const std::string& label, AssetType assetType, std::function<void(AssetInfo&)> func)
{

	if(ImGui::BeginCombo(("##" + label).c_str(), label.c_str()))
	{
		for(auto& asset : ROSE_GETSYSTEM(AssetStore).GetAssetOfType(assetType))
		{
			bool isSelected = false;
			if(ImGui::Selectable(asset.first.c_str(), &isSelected))
			{
				func(asset);
			}
		}
		ImGui::EndCombo();
	}
}