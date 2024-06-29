#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "Core/Entity.h"
#include "Renderer/Renderer.h"
#include "Input/InputSystem.h"
#include "Core/Systems.h"

#include "Components/TransformComponent.h"
#include <Core/Log.h>

class RotateTool
{
	entt::entity entity = NoEntity();
	SDL_Renderer* renderer = nullptr;
	bool isRotating = false;
	vec2 mouseStartPos;
	float mouseStartRot;
	float entityStartRot;
	vec2 gizmosCenter;
	float gizmosRadius;


	bool IsInsideCircle(glm::vec2& mousePos)
	{
		if(distance(mousePos, gizmosCenter) < gizmosRadius)
		{
			return true;
		}
		return false;
	}


public:
	RotateTool(SDL_Renderer* renderer = nullptr)
	{
		this->renderer = renderer;
	}
	void SetSelectedEntity(entt::entity entity)
	{
		this->entity = entity;
	}
	void RenderGizmos()
	{

		if(entity == NoEntity())
		{
			return;
		}
		auto& renderSystem = ROSE_GETSYSTEM(RendererSystem);
		auto matrix = renderSystem.GetWorldToScreenMatrix();
		auto& trx = ROSE_GETSYSTEM(EntitySystem).GetComponent<TransformComponent>(entity);
		auto orig = TransformComponent::GetPosition(trx.matrixL2W * matrix, vec2(0, 0));
		gizmosCenter = orig;
		gizmosRadius = 75;
		if(isRotating)
		{
			aacircleRGBA(renderer, orig.x, orig.y, 60, 200, 200, 50, 255);
			filledCircleRGBA(renderer, orig.x, orig.y, 60, 200, 200, 50, 100);
		} else
		{
			aacircleRGBA(renderer, orig.x, orig.y, 60, 200, 50, 50, 255);
			filledCircleRGBA(renderer, orig.x, orig.y, 60, 200, 50, 50, 50);
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
		auto& renderSystem = ROSE_GETSYSTEM(RendererSystem);
		auto matrix = renderSystem.GetWorldToScreenMatrix();
		auto screenToWorldMat = inverse(renderSystem.GetWorldToScreenMatrix());
		if(input.GetMouseButton(LEFT_BUTTON).justPressed)
		{
			if(IsInsideCircle(mousePos))
			{
				entityStartRot = trx.globalRotation;
				mouseStartPos = mousePos;
				auto mouseStartDir = TransformComponent::GetPosition(screenToWorldMat, mousePos) - trx.globalPosition;
				mouseStartDir = normalize(mouseStartDir);
				mouseStartRot = degrees(atan2(mouseStartDir.x, mouseStartDir.y));
				isRotating = true;
			}
		}
		if(isRotating)
		{
			if(input.GetMouseButton(LEFT_BUTTON).isPressed)
			{
				auto mouseDir = TransformComponent::GetPosition(screenToWorldMat, mousePos) - trx.globalPosition;
				mouseDir = normalize(mouseDir);
				auto angle = degrees(atan2(mouseDir.x, mouseDir.y));
				if(input.GetKey(LSHIFT).isPressed)
				{
					auto steppedAngleChange = ((int)(angle - mouseStartRot) / 15) * 15;
					trx.globalRotation = entityStartRot - steppedAngleChange;
				} else
				{
					trx.globalRotation = entityStartRot - (angle - mouseStartRot);
				}
				trx.UpdateLocals();

			} else
			{
				isRotating = false;
			}
		}
	}
};