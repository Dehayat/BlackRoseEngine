#pragma once
#include <string>
#include <imgui.h>

void Imgui_InputText(const char* label, std::string& buffer, int length, ImGuiInputTextFlags flags = 0);