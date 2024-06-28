#pragma once
#include <string>
#include <functional>

#include <imgui.h>

#include "AssetPipline/Asset.h"

typedef std::pair<std::string, AssetHandle> AssetInfo;

void Imgui_InputText(const char* label, std::string& buffer, int length, ImGuiInputTextFlags flags = 0);
void Imgui_AssetDropDown(const std::string& label, AssetType assetType, std::function<void(AssetInfo&)> func);