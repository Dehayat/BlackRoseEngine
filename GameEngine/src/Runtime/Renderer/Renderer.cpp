#include "Renderer/Renderer.h"

#include <entt/entt.hpp>

#include "Core/SdlContainer.h"
#include "AssetPipline/AssetStore.h"
#include "Core/Systems.h"
#include "Core/DisableSystem.h"

#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/CameraComponent.h"


#include "Core/Log.h"

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
	registry.sort<SpriteComponent>([](const auto& lhs, const auto& rhs)
		{
			return lhs.layer < rhs.layer;
		});
	TransformComponent* camPos = nullptr;
	float camHeight = 10;
	if(registry.valid(camera))
	{
		camPos = &registry.get<TransformComponent>(camera);
		camHeight = registry.get<CameraComponent>(camera).height;
	}
	if(camPos == nullptr || !registry.get<CameraComponent>(camera).startCamera)
	{
		auto view = registry.view<CameraComponent, TransformComponent>();
		for(auto entity : view)
		{
			auto& pos = view.get<TransformComponent>(entity);
			auto& body = view.get<CameraComponent>(entity);
			if(body.startCamera)
			{
				SetCamera(entity);
				break;
			}
		}
	}
	if(camPos == nullptr)
	{
		auto view = registry.view<CameraComponent, TransformComponent>();
		for(auto entity : view)
		{
			ROSE_ERR("No Starting Camera Assigned");
			SetCamera(entity);
			continue;
		}
	}
	if(registry.valid(camera))
	{
		camPos = &registry.get<TransformComponent>(camera);
		camHeight = registry.get<CameraComponent>(camera).height;
	} else
	{
		ROSE_ERR("No Camera Found");
		return;
	}
	auto camToWorldMatrix = glm::mat3(
		glm::cos(glm::radians(camPos->rotation)), -glm::sin(glm::radians(camPos->rotation)), camPos->position.x,
		glm::sin(glm::radians(camPos->rotation)), glm::cos(glm::radians(camPos->rotation)), camPos->position.y,
		0, 0, 1
	);
	/*if (camPos->hasParent) {
		auto& parentPos = registry.get<TransformComponent>(camPos->parent.value());
		camToWorldMatrix = camToWorldMatrix;
	}*/
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



	auto view2 = registry.view<SpriteComponent, const TransformComponent>().use<SpriteComponent>();
	for(auto entity : view2)
	{
		if(!ROSE_GETSYSTEM(DisableSystem).IsEnabled(entity))
		{
			continue;
		}
		int x = view2.size_hint();
		const auto& pos = view2.get<TransformComponent>(entity);
		auto& sp = view2.get<SpriteComponent>(entity);
		auto spriteHandle = assetStore.GetAsset(sp.sprite);
		if(spriteHandle.type != AssetType::Texture)
		{
			continue;
		}
		auto texture = static_cast<TextureAsset*>(spriteHandle.asset);
		if(texture == nullptr)
		{
			continue;
		}
		int texW, texH;
		SDL_Rect* sourceRect = sp.sourceRect;
		if(sourceRect != nullptr)
		{
			texH = sourceRect->h;
			texW = sourceRect->w;
		} else
		{
			SDL_QueryTexture(texture->texture, nullptr, nullptr, &texW, &texH);
		}
		auto viewMatrix = pos.matrixL2W * worldToScreenMatrix;
		auto position = TransformComponent::GetPosition(viewMatrix);
		auto rotation = TransformComponent::GetRotation(viewMatrix);
		auto scale = TransformComponent::GetScale(viewMatrix);
		int spriteSizeX = scale.x * ((float)texW / texture->ppu);
		int spriteSizeY = scale.y * ((float)texH / texture->ppu);
		SDL_FRect player = SDL_FRect{
			(position.x - spriteSizeX / 2),
			(position.y - spriteSizeY / 2),
			(float)spriteSizeX,
			(float)spriteSizeY,
		};
		sp.destRect = player;
		SDL_SetTextureColorMod(texture->texture, sp.color.r * 255, sp.color.g * 255, sp.color.b * 255);
		SDL_SetTextureBlendMode(texture->texture, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(texture->texture, sp.color.a * 255);
		SDL_RendererFlip flip = SDL_FLIP_NONE;
		if(pos.scaleSign.x < 0 && pos.scaleSign.y < 0)
		{
		} else if(pos.scaleSign.x < 0)
		{
			flip = SDL_FLIP_VERTICAL;
		} else if(pos.scaleSign.y < 0)
		{
			flip = SDL_FLIP_VERTICAL;
		}
		SDL_RenderCopyExF(sdlRenderer, texture->texture, sourceRect, &player, rotation, nullptr, flip);

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
entt::entity RendererSystem::GetCamera() const
{
	return camera;
}
const glm::mat3 RendererSystem::GetWorldToScreenMatrix() const
{
	return worldToScreenMatrix;
}
const glm::mat3 RendererSystem::GetScreenToWorldMatrix() const
{
	return glm::inverse(worldToScreenMatrix);
}
void RendererSystem::InitLoaded()
{
	Entities& entities = entt::locator<Entities>::value();
	entt::registry& registry = entities.GetRegistry();
	auto view = registry.view<CameraComponent, TransformComponent>();
	for(auto entity : view)
	{
		auto& pos = view.get<TransformComponent>(entity);
		auto& body = view.get<CameraComponent>(entity);
		if(body.startCamera)
		{
			SetCamera(entity);
			break;
		}
	}
}
