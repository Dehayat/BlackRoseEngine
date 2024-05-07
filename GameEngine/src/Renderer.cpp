#include "Renderer.h"
#include <ryml/ryml_std.hpp>
#include <entt/entt.hpp>
#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/CameraComponent.h"
#include "Logger.h"

RendererSystem::RendererSystem(SDL_Renderer* sdl)
{
	camera = entt::entity(-1);
	this->sdl = sdl;
	this->worldToScreenMatrix = glm::mat3(1);
}
RendererSystem::~RendererSystem()
{
}
void RendererSystem::Render()
{
	entt::registry& registry = entt::locator<entt::registry>::value();
	AssetStore& assetStore = entt::locator<AssetStore>::value();
	SDL_SetRenderDrawColor(sdl, 38, 77, 142, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(sdl);
	registry.sort<SpriteComponent>([](const auto& lhs, const auto& rhs) {
		return lhs.layer < rhs.layer;
		});
	TransformComponent camPos;
	float camHeight = 10;
#ifdef _EDITOR
	camPos = editorCamTrx;
	camHeight = editorCam.height;
#else
	if (registry.valid(camera)) {
		auto& cam = registry.get<CameraComponent>(camera);
		camHeight = cam.height;
		camPos = registry.get<TransformComponent>(camera);
	}
	else {
		Logger::Error("No Camera Assigned to renderer");
	}
#endif
	auto camMatrix = glm::mat3(
		glm::cos(glm::radians(camPos.rotation)), -glm::sin(glm::radians(camPos.rotation)), camPos.position.x,
		glm::sin(glm::radians(camPos.rotation)), glm::cos(glm::radians(camPos.rotation)), camPos.position.y,
		0, 0, 1
	);
	auto invCamMatrix = glm::inverse(camMatrix);
	int sizeX, sizeY;
	SDL_GetRendererOutputSize(sdl, &sizeX, &sizeY);
	auto windowSize = glm::vec2(sizeX, sizeY);
	float windowAspect = windowSize.x / windowSize.y;
	float camWidth = windowAspect * camHeight;
	float scaleX = windowSize.x / (camWidth);
	float scaleY = windowSize.y / (camHeight);

	auto tr = glm::mat3(
		1, 0, camWidth / 2,
		0, -1, camHeight / 2,
		0, 0, 1);
	auto sc = glm::mat3(
		scaleX, 0, 0,
		0, scaleY, 0,
		0, 0, 1);
	glm::mat3 camToScreen = tr * sc;
	auto worldToScreen = invCamMatrix * camToScreen;
	this->worldToScreenMatrix = worldToScreen;



	auto view2 = registry.view<const SpriteComponent, const TransformComponent>();
	for (auto entity : view2) {
		const auto& pos = view2.get<TransformComponent>(entity);
		const auto& sp = view2.get<SpriteComponent>(entity);
		const auto& texture = assetStore.GetTexture(sp.sprite);
		if (texture == nullptr) {
			continue;
		}
		int texW;
		int texH;
		SDL_QueryTexture(texture->texture, nullptr, nullptr, &texW, &texH);
		auto viewMatrix = pos.matrix * worldToScreen;
		auto position = glm::vec2(viewMatrix[0][2], viewMatrix[1][2]);
		auto scale = glm::vec2(glm::sqrt(viewMatrix[0][0] * viewMatrix[0][0] + viewMatrix[1][0] * viewMatrix[1][0]), glm::sqrt(viewMatrix[0][1] * viewMatrix[0][1] + viewMatrix[1][1] * viewMatrix[1][1]));
		auto rotation = glm::degrees(std::atan2f(viewMatrix[1][0], viewMatrix[0][0]));

		int spriteSizeX = scale.x * ((float)texW / texture->ppu);
		int spriteSizeY = scale.y * ((float)texH / texture->ppu);
		SDL_FRect player = SDL_FRect{
			(position.x - spriteSizeX / 2),
			(position.y - spriteSizeY / 2),
			(float)spriteSizeX,
			(float)spriteSizeY,
		};
		SDL_SetTextureColorMod(texture->texture, sp.color.r * 255, sp.color.g * 255, sp.color.b * 255);
		SDL_SetTextureBlendMode(texture->texture, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(texture->texture, sp.color.a * 255);
		SDL_RenderCopyExF(sdl, texture->texture, nullptr, &player, rotation, nullptr, SDL_FLIP_NONE);
	}
}
void RendererSystem::Present()
{
	SDL_RenderPresent(sdl);
}
void RendererSystem::SetCamera(entt::entity cam)
{
	this->camera = cam;
}
entt::entity RendererSystem::GetCamera()
{
	return camera;
}
const glm::mat3 RendererSystem::GetWorldToScreenMatrix()
{
	return worldToScreenMatrix;
}
const glm::mat3 RendererSystem::GetScreenToWorldMatrix()
{
	return glm::inverse(worldToScreenMatrix);
}
void RendererSystem::InitLoaded()
{
	entt::registry& registry = entt::locator<entt::registry>::value();
	auto view = registry.view<const CameraComponent, const TransformComponent>();
	for (auto entity : view) {
		const auto& pos = view.get<TransformComponent>(entity);
		const auto& body = view.get<CameraComponent>(entity);
		if (body.startCamera) {
			SetCamera(entity);
#ifdef _EDITOR
			editorCam = body;
			editorCamTrx = pos;
#endif
			break;
		}
	}
}
