#include "ImguiHelper.h"

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
