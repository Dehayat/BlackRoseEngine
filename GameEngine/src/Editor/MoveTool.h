#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "Core/Entity.h"
#include "Core/SdlContainer.h"
#include "Renderer/Renderer.h"
#include "Input/InputSystem.h"
#include "Core/Systems.h"

#include "Components/TransformComponent.h"
#include <Core/Log.h>

class MoveTool
{
	entt::entity entity = NoEntity();
	SDL_Renderer* renderer = nullptr;
	SDL_FRect xAxisGizmos;
	SDL_FRect yAxisGizmos;
	SDL_FRect bothAxisGizmos;
	bool isMovingX = false;
	bool isMovingY = false;
	vec2 mouseStartPos;
	vec2 entityStartPos;


	bool IsInsideRect(glm::vec2& mousePos, SDL_FRect rect)
	{
		ROSE_LOG("%f %f - %f %f %f %f", mousePos.x, mousePos.y, rect.x, rect.y, rect.w, rect.h);
		if(mousePos.x > rect.x && mousePos.x<rect.x + rect.w && mousePos.y>rect.y && mousePos.y < rect.y + rect.h)
		{
			return true;
		}
		return false;
	}

	bool IsOverBox(vec2 mousePos)
	{
		if(IsInsideRect(mousePos, bothAxisGizmos))
		{
			return true;
		}
		return false;
	}

public:
	MoveTool(SDL_Renderer* renderer = nullptr)
	{
		this->renderer = renderer;
	}
	void SetSelectedEntity(entt::entity entity)
	{
		this->entity = entity;
	}
	void RenderGizmos()
	{

		auto& renderSystem = ROSE_GETSYSTEM(RendererSystem);
		auto matrix = renderSystem.GetWorldToScreenMatrix(), GetSelectedEntity();
		auto& trx = ROSE_GETSYSTEM(EntitySystem).GetComponent<TransformComponent>(entity);
		auto orig = TransformComponent::GetPosition(trx.matrixL2W * matrix, vec2(0, 0));
		auto xAxis = orig + vec2(100, 0);
		xAxisGizmos = {orig.x,orig.y - 8, 100, 16};
		auto yAxis = orig + vec2(0, -100);
		yAxisGizmos = {orig.x - 8,orig.y - 100, 16, 100};
		bothAxisGizmos = {orig.x,orig.y - 20, 20, 20};
		if(isMovingX && isMovingY)
		{
			boxRGBA(renderer, orig.x, orig.y, orig.x + 20, orig.y - 20, 200, 200, 50, 200);
		} else
		{
			boxRGBA(renderer, orig.x, orig.y, orig.x + 20, orig.y - 20, 50, 150, 150, 200);
		}
		if(isMovingX)
		{
			thickLineRGBA(renderer, orig.x, orig.y, xAxis.x, xAxis.y, 6, 200, 200, 50, 255);
		} else
		{
			thickLineRGBA(renderer, orig.x, orig.y, xAxis.x, xAxis.y, 6, 50, 50, 150, 255);
		}
		if(isMovingY)
		{
			thickLineRGBA(renderer, orig.x, orig.y, yAxis.x, yAxis.y, 6, 200, 200, 50, 255);
		} else
		{
			thickLineRGBA(renderer, orig.x, orig.y, yAxis.x, yAxis.y, 6, 50, 150, 50, 255);
		}
	}
	void Update()
	{
		if(entity == NoEntity())
		{
			return;
		}
		auto& input = ROSE_GETSYSTEM(InputSystem);
		auto mousePos = input.GetMousePosition();
		auto& trx = ROSE_GETSYSTEM(EntitySystem).GetComponent<TransformComponent>(entity);
		if(input.GetMouseButton(LEFT_BUTTON).justPressed)
		{
			if(IsInsideRect(mousePos, bothAxisGizmos))
			{
				isMovingX = true;
				isMovingY = true;
				mouseStartPos = mousePos;
				entityStartPos = trx.globalPosition;
			} else if(IsInsideRect(mousePos, xAxisGizmos))
			{
				isMovingX = true;
				mouseStartPos = mousePos;
				entityStartPos = trx.globalPosition;
			} else if(IsInsideRect(mousePos, yAxisGizmos))
			{
				isMovingY = true;
				mouseStartPos = mousePos;
				entityStartPos = trx.globalPosition;
			}
		}
		if((isMovingX || isMovingY))
		{
			if(input.GetMouseButton(LEFT_BUTTON).isPressed)
			{
				auto offset = mousePos - mouseStartPos;
				auto& renderSystem = ROSE_GETSYSTEM(RendererSystem);
				auto screenToWorldMat = inverse(renderSystem.GetWorldToScreenMatrix());
				auto translate = TransformComponent::GetDir(screenToWorldMat, offset);
				if(!isMovingX)
				{
					translate.x = 0;
				}
				if(!isMovingY)
				{
					translate.y = 0;
				}
				trx.globalPosition = entityStartPos + translate;
				trx.UpdateLocals();
				ROSE_LOG("Mouse moving by: %f,%f", translate.x, translate.y);
			} else
			{
				isMovingX = false;
				isMovingY = false;
			}
		}
	}
};