#include "Renderer/Renderer.h"

#include <entt/entity/registry.hpp>

#include "Core/SdlContainer.h"
#include "AssetPipline/AssetStore.h"
#include "Core/Systems.h"

#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/CameraComponent.h"
#include "Components/DisableComponent.h"


#include "Core/Log.h"

RendererSystem::RendererSystem()
{
	SdlContainer& sdlRenderer = ROSE_GETSYSTEM(SdlContainer);
	this->sdlRenderer = sdlRenderer.GetRenderer();
	camera = NoEntity();
	this->worldToScreenMatrix = glm::mat3(1);
#ifdef _EDITOR
	editorViewHeight = 10;
	editorViewPos = {0,0};
#endif
}
RendererSystem::~RendererSystem()
{
}
void RendererSystem::Render()
{
	EntitySystem& entities = entt::locator<EntitySystem>::value();
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
#ifdef _EDITOR
	auto camToWorldMatrix = glm::mat3(
		1, 0, editorViewPos.x,
		0, 1, editorViewPos.y,
		0, 0, 1
	);
	editorViewHeight = clamp(editorViewHeight, 0.01f, 50.f);
	camHeight = editorViewHeight;
#else
	auto camToWorldMatrix = glm::mat3(
		glm::cos(glm::radians(camPos->globalRotation)), -glm::sin(glm::radians(camPos->globalRotation)), camPos->globalPosition.x,
		glm::sin(glm::radians(camPos->globalRotation)), glm::cos(glm::radians(camPos->globalRotation)), camPos->globalPosition.y,
		0, 0, 1
	);
#endif
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
	aspectRatio = windowAspectRatio;

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



	auto view2 = registry.view<SpriteComponent, const TransformComponent>(entt::exclude<DisableComponent>).use<SpriteComponent>();
	for(auto entity : view2)
	{
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

		SDL_SetTextureBlendMode(texture->texture, SDL_BLENDMODE_BLEND);

		vec2 spriteExtents = vec2(texW, texH) / (float)texture->ppu / 2.0f;
		vec2 v1 = TransformComponent::GetPosition(viewMatrix, {-spriteExtents.x,spriteExtents.y});
		vec2 v2 = TransformComponent::GetPosition(viewMatrix, {spriteExtents.x,spriteExtents.y});
		vec2 v3 = TransformComponent::GetPosition(viewMatrix, {spriteExtents.x,-spriteExtents.y});
		vec2 v4 = TransformComponent::GetPosition(viewMatrix, {-spriteExtents.x,-spriteExtents.y});


		SDL_Color color = {sp.color.r * 255, sp.color.g * 255, sp.color.b * 255, sp.color.a * 255};
		SDL_Vertex vertex_1 = {{v1.x,v1.y}, color, {0, 0}};
		SDL_Vertex vertex_2 = {{v2.x,v2.y}, color, {1, 0}};
		SDL_Vertex vertex_3 = {{v3.x,v3.y}, color, {1, 1}};
		SDL_Vertex vertex_4 = {{v4.x,v4.y}, color, {0, 1}};

		if(sp.sourceRect != nullptr)
		{
			int texW;
			int texH;
			SDL_QueryTexture(texture->texture, nullptr, nullptr, &texW, &texH);
			vec2 pos = {sp.sourceRect->x,sp.sourceRect->y};
			vec2 size = {sp.sourceRect->w,sp.sourceRect->h};
			pos.x /= texW;
			pos.y /= texH;
			size.x /= texW;
			size.y /= texH;
			vertex_1.tex_coord = {pos.x,pos.y};
			vertex_2.tex_coord = {pos.x + size.x,pos.y};
			vertex_3.tex_coord = {pos.x + size.x,pos.y + size.y};
			vertex_4.tex_coord = {pos.x,pos.y + size.y};
		}
		SDL_Vertex vertices[] = {
			vertex_1,
			vertex_2,
			vertex_3,
			vertex_4,
		};

		int indecies[] = {
			0,
			1,
			2,
			0,
			2,
			3
		};

		SDL_RenderGeometry(sdlRenderer, texture->texture, vertices, 4, indecies, 6);
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
	EntitySystem& entities = entt::locator<EntitySystem>::value();
	entt::registry& registry = entities.GetRegistry();
	auto view = registry.view<CameraComponent, TransformComponent>();
	for(auto entity : view)
	{
		auto& pos = view.get<TransformComponent>(entity);
		auto& body = view.get<CameraComponent>(entity);
		if(body.startCamera)
		{
			SetCamera(entity);
#ifdef _EDITOR
			editorViewHeight = body.height;
			editorViewPos = pos.globalPosition;
#endif _EDITOR
			break;
		}
	}
}

float RendererSystem::GetAspectRatio()
{
	return aspectRatio;
}
