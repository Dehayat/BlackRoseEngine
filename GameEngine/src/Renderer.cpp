#include "Renderer.h"
#include <ryml/ryml_std.hpp>
#include <entt/entt.hpp>
#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/CameraComponent.h"
#include "Logger.h"
#include "SdlContainer.h"

RendererSystem::RendererSystem()
{
	SdlContainer& sdlRenderer = entt::locator<SdlContainer>::value();
	this->sdlRenderer = sdlRenderer.GetRenderer();
	camera = entt::entity(-1);
	this->worldToScreenMatrix = glm::mat3(1);
}
RendererSystem::~RendererSystem()
{
}
void RendererSystem::Render()
{
	Entities& entities = entt::locator<Entities>::value();
	entt::registry& registry = entities.GetRegistry();
	AssetStore& assetStore = entt::locator<AssetStore>::value();
	SDL_SetRenderDrawColor(sdlRenderer, 94, 35, 35, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(sdlRenderer);
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
	auto camToWorldMatrix = glm::mat3(
		glm::cos(glm::radians(camPos.rotation)), -glm::sin(glm::radians(camPos.rotation)), camPos.position.x,
		glm::sin(glm::radians(camPos.rotation)), glm::cos(glm::radians(camPos.rotation)), camPos.position.y,
		0, 0, 1
	);
	auto worldToCamMatrix = glm::inverse(camToWorldMatrix);
	int windowSizeX, windowSizeY;
	SDL_GetRendererOutputSize(sdlRenderer, &windowSizeX, &windowSizeY);
	auto windowSize = glm::vec2(windowSizeX, windowSizeY);
	float windowAspectRatio = windowSize.x / windowSize.y;
	float camWidth = windowAspectRatio * camHeight;
	float camToScreenScaleX = windowSize.x / (camWidth);
	float camToScreenScaleY = windowSize.y / (camHeight);

	auto camToScreenTranslationMatrix = glm::mat3(
		1, 0, camWidth / 2,
		0, -1, camHeight / 2,
		0, 0, 1);
	auto camToScreenScaleMatrix = glm::mat3(
		camToScreenScaleX, 0, 0,
		0, camToScreenScaleY, 0,
		0, 0, 1);
	glm::mat3 camToScreenMatrix = camToScreenTranslationMatrix * camToScreenScaleMatrix;
	auto worldToScreenMatrix = worldToCamMatrix * camToScreenMatrix;
	this->worldToScreenMatrix = worldToScreenMatrix;



	auto view2 = registry.view<const SpriteComponent, const TransformComponent>();
	for (auto entity : view2) {
		int x = view2.size_hint();
		const auto& pos = view2.get<TransformComponent>(entity);
		const auto& sp = view2.get<SpriteComponent>(entity);
		const auto& texture = assetStore.GetTexture(sp.sprite);
		if (texture == nullptr) {
			continue;
		}
		int texW, texH;
		SDL_Rect* sourceRect = sp.sourceRect;
		if (sourceRect != nullptr) {
			texH = sourceRect->h;
			texW = sourceRect->w;
		}
		else {
			SDL_QueryTexture(texture->texture, nullptr, nullptr, &texW, &texH);
		}
		auto viewMatrix = pos.matrix * worldToScreenMatrix;
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
		SDL_RenderCopyExF(sdlRenderer, texture->texture, sourceRect, &player, rotation, nullptr, SDL_FLIP_NONE);
	}
}
void RendererSystem::Present()
{
	SDL_RenderPresent(sdlRenderer);
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
	Entities& entities = entt::locator<Entities>::value();
	entt::registry& registry = entities.GetRegistry();
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
