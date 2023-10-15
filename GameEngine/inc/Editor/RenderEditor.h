#pragma once
#ifdef _EDITOR
#include <imgui.h>
#include "Components/SpriteComponent.h"
class SpriteEditor {
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
	static void DrawEditor(Sprite& sprite) {
		ImGui::ColorEdit4("Color", (float*) (&sprite.color), ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_Float);
		if (sprite.sprite.capacity() < 20) {
			sprite.sprite.reserve(20);
		}
		ImGui::InputText("sprite", &sprite.sprite[0], sprite.sprite.capacity(), ImGuiInputTextFlags_CallbackResize, ResizeStringCallback, &sprite.sprite);
	}
};
#endif
