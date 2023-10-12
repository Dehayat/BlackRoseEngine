#pragma once
#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <ryml/ryml.hpp>
#include "AssetStore.h"

#ifdef _EDITOR
#include <imgui.h>
#include"Logger.h"
#endif

#ifdef _EDITOR
static int ResizeStringCallback(ImGuiInputTextCallbackData* data)
{
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
	{
		auto str = (std::string*)(data->UserData);
		str->resize(data->BufTextLen);
	}
	return 0;
}
#endif

struct Sprite {
	std::string sprite;
	int layer;
	SDL_Color color;
	Sprite(std::string sprite, int layer = 0, SDL_Color color = SDL_Color{ 255,255,255,255 });
	Sprite(ryml::NodeRef node);
#ifdef _EDITOR
	float colorFloat[4];
	void DrawEditor() {
		colorFloat[0] = color.r / 255.0f;
		colorFloat[1] = color.g / 255.0f;
		colorFloat[2] = color.b / 255.0f;
		colorFloat[3] = color.a / 255.0f;
		ImGui::ColorEdit4("Color", colorFloat, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_Float);
		color = SDL_Color{ (unsigned char)(colorFloat[0] * 255), (unsigned char)(colorFloat[1] * 255), (unsigned char)(colorFloat[2] * 255), (unsigned char)(colorFloat[3] * 255) };
		if (sprite.capacity() < 20) {
			sprite.reserve(20);
		}
		ImGui::InputText("sprite", &sprite[0], sprite.capacity(), ImGuiInputTextFlags_CallbackResize, ResizeStringCallback, &sprite);
	}
#endif
};

struct Camera {
	float height;
	glm::mat3 camToScreen;
	glm::mat3 worldToScreen;
	bool startCamera;
	Camera(float height = 10);
	Camera(ryml::NodeRef node);
};


class Renderer
{
	SDL_Renderer* sdl;
	entt::entity camera;
	glm::mat3 worldToScreenMatrix;
public:
	Renderer(SDL_Renderer* sdl);
	~Renderer();
	void Render(entt::registry* registry, const AssetStore& assetStore);
	void Present();
	void SetCamera(entt::entity cam);
	entt::entity GetCamera();
	glm::mat3 GetWorldToScreenMatrix();
	glm::mat3 GetScreenToWorldMatrix();
	void InitLoaded(entt::registry& registry);
};

