#pragma once
#include <imgui.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "Renderer/Renderer.h"

#include "Core/Systems.h"
#include "Core/Reflection.h"

#include "Components/SpriteComponent.h"
#include "Components/CameraComponent.h"

#include "Editor/ComponentEditor.h"

class SpriteEditor :public IComponentEditor {
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
		auto& sprite = registry.get<SpriteComponent>(entity);
		ImGui::ColorEdit4("Color", (float*)(&sprite.color), ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_Float);
		ImGui::InputInt("Layer", &sprite.layer);
		if (sprite.sprite.capacity() < 21) {
			sprite.sprite.reserve(21);
		}
		ImGui::InputText("sprite", &sprite.sprite[0], 21, ImGuiInputTextFlags_CallbackResize, ResizeStringCallback, &sprite.sprite);
		ROSE_INIT_VARS(SpriteComponent);
		Reflector<SpriteComponent> m;
		Logger::Log(*(std::string*)m.GetVar(&sprite, "sprite"));
	}
};

class CameraEditor :public IComponentEditor {
public:
	void Editor(entt::entity entity) {
		auto& registry = GETSYSTEM(Entities).GetRegistry();
		auto& camera = registry.get<CameraComponent>(entity);
		ImGui::DragFloat("Camera Height", &camera.height, 0.2f, 0.1f, 100.f);
		ImGui::Checkbox("start Camera", &camera.startCamera);
	}
	static void DrawGizmos(SDL_Renderer* sdl, RendererSystem& renderer, const CameraComponent& camera, const TransformComponent& trx) {
		int sizeX, sizeY;
		SDL_GetRendererOutputSize(sdl, &sizeX, &sizeY);
		auto windowSize = glm::vec2(sizeX, sizeY);
		float windowAspect = windowSize.x / windowSize.y;
		float camWidth = windowAspect * camera.height;

		float halfWidth = camWidth / 2.0f;
		float halfHeight = camera.height / 2.0f;
		auto viewMatrix = trx.matrixL2W;
		auto pos = glm::vec2(viewMatrix[0][2], viewMatrix[1][2]);
		auto rotation = glm::degrees(std::atan2f(viewMatrix[1][0], viewMatrix[0][0]));

		// Convert rotation angle to radians
		float radians = glm::radians(rotation);

		// Calculate the cosines and sines of the rotation angle
		float cosTheta = std::cos(radians);
		float sinTheta = std::sin(radians);

		// Initialize an array to store the coordinates of the box's points
		glm::vec2 boxPoints[4];

		// Calculate the coordinates of the four corners of the rotated box
		boxPoints[0] = pos + glm::vec2(cosTheta * halfWidth - sinTheta * halfHeight, sinTheta * halfWidth + cosTheta * halfHeight);
		boxPoints[1] = pos + glm::vec2(-cosTheta * halfWidth - sinTheta * halfHeight, -sinTheta * halfWidth + cosTheta * halfHeight);
		boxPoints[2] = pos + glm::vec2(-cosTheta * halfWidth + sinTheta * halfHeight, -sinTheta * halfWidth - cosTheta * halfHeight);
		boxPoints[3] = pos + glm::vec2(cosTheta * halfWidth + sinTheta * halfHeight, sinTheta * halfWidth - cosTheta * halfHeight);
		for (int i = 0; i < 4; i++) {
			boxPoints[i] = glm::vec3(boxPoints[i], 1) * renderer.GetWorldToScreenMatrix();
		}
		for (int i = 0; i < 4; i++) {
			auto target = boxPoints[0];
			if (i < 3) {
				target = boxPoints[i + 1];
			}
			thickLineRGBA(sdl, boxPoints[i].x, boxPoints[i].y, target.x, target.y, 5, 255, 255, 255, 255);
		}
	}
};