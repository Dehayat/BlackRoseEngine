#include "Game.h"
#include "BaseGame.h"

#include "Core/SdlContainer.h"
#include "Core/Systems.h"

#include "Core/Transform.h"
#include "Physics/Physics.h"
#include "Renderer/Renderer.h"
#include "Input/InputSystem.h"
#include "Core/TimeSystem.h"
#include "Core/DisableSystem.h"
#include "Animation/AnimationSystem.h"
#include "Events/EntityEventSystem.h"
#include "Scripting/ScriptSystem.h"

Game::Game():BaseGame()
{
	isRunning = false;
}

Game::~Game()
{
}

void Game::Run()
{
	Setup();
	isRunning = true;
	while(isRunning)
	{
		Update();
		Render();
	}
}

void Game::Update()
{
	bool exitGame = ROSE_GETSYSTEM(SdlContainer).ProcessEvents();

	if(exitGame)
	{
		isRunning = false;
	}
	ROSE_GETSYSTEM(TimeSystem).Update();
	ROSE_GETSYSTEM(TransformSystem).Update();
	ROSE_GETSYSTEM(InputSystem).Update();
	ROSE_GETSYSTEM(PhysicsSystem).Update();
	ROSE_GETSYSTEM(AnimationSystem).Update();
	ROSE_GETSYSTEM(EntityEventSystem).Update();
	ROSE_GETSYSTEM(ScriptSystem).Update();
}

void Game::Render()
{
	RendererSystem& renderer = ROSE_GETSYSTEM(RendererSystem);
	renderer.Render();
	renderer.Present();
}