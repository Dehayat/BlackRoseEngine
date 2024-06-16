#pragma once
#include <imgui.h>

#include "Core/Entity.h"

#include "Core/Systems.h"

#include "Components/SpriteComponent.h"
#include "Components/CameraComponent.h"
#include "Components/AnimationComponent.h"

#include "Editor/ComponentEditor.h"

class AnimationEditor :public IComponentEditor {
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
		auto& anim = registry.get<AnimationComponent>(entity);
		if (anim.animation.capacity() < 21) {
			anim.animation.reserve(21);
		}
		ImGui::InputText("Animation", &anim.animation[0], 21, ImGuiInputTextFlags_CallbackResize, ResizeStringCallback, &anim.animation);
	}
};