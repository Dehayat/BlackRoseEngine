#include "BaseGame.h"

#include "Core/Systems.h"
#include "Reflection/Reflection.h"
#include "Core/Log.h"

#include <FileDialog.h>
#include "Core/SdlContainer.h"
#include "AssetPipline/AssetStore.h"
#include "Core/Entity.h"
#include "Project/ProjectLoader.h"
#include "Levels/LevelLoader.h"

#include "Core/Transform.h"
#include "Physics/Physics.h"
#include "Renderer/Renderer.h"
#include "Input/InputSystem.h"
#include "Core/TimeSystem.h"
#include "Core/DisableSystem.h"
#include "Animation/AnimationSystem.h"
#include "Events/EntityEventSystem.h"
#include "Scripting/ScriptSystem.h"
#include "Core/LevelTree.h"
#include "Events/EventBus.h"

#include "Gameplay/CombatSystem.h"

BaseGame::BaseGame()
{
	SetupBaseSystems();
	SetupLowLevelSystems();
	ROSE_LOG("Game constructed");
}

BaseGame::~BaseGame()
{
	ROSE_DESTROYSYSTEM(CombatSystem);

	ROSE_DESTROYSYSTEM(PhysicsSystem);
	ROSE_DESTROYSYSTEM(TransformSystem);
	ROSE_DESTROYSYSTEM(ScriptSystem);
	ROSE_DESTROYSYSTEM(EntityEventSystem);
	ROSE_DESTROYSYSTEM(AnimationSystem);
	ROSE_DESTROYSYSTEM(RendererSystem);
	ROSE_DESTROYSYSTEM(InputSystem);
	ROSE_DESTROYSYSTEM(TimeSystem);
	ROSE_DESTROYSYSTEM(EventBus);
	ROSE_DESTROYSYSTEM(LevelTree);
	ROSE_DESTROYSYSTEM(DisableSystem);

	ROSE_DESTROYSYSTEM(ProjectLoader);
	ROSE_DESTROYSYSTEM(AssetStore);
	ROSE_DESTROYSYSTEM(LevelLoader);
	ROSE_DESTROYSYSTEM(EntitySystem);
	ROSE_DESTROYSYSTEM(ReflectionSystem);
	ROSE_DESTROYSYSTEM(FileDialog);
	ROSE_DESTROYSYSTEM(SdlContainer);

	ROSE_LOG("Game destrcuted");
}

void BaseGame::SetupBaseSystems()
{
	ROSE_CREATESYSTEM(SdlContainer, 1200, (float)1200 * 9 / 16);
	ROSE_CREATESYSTEM(FileDialog);
	ROSE_CREATESYSTEM(ReflectionSystem);
	ROSE_CREATESYSTEM(EntitySystem);
	ROSE_CREATESYSTEM(LevelLoader);
	ROSE_CREATESYSTEM(AssetStore);
	ROSE_CREATESYSTEM(ProjectLoader);
}

void BaseGame::SetupLowLevelSystems()
{
	ROSE_CREATESYSTEM(DisableSystem);
	ROSE_CREATESYSTEM(LevelTree);
	ROSE_CREATESYSTEM(EventBus);
	ROSE_CREATESYSTEM(TimeSystem);
	ROSE_CREATESYSTEM(InputSystem);
	ROSE_CREATESYSTEM(RendererSystem);
	ROSE_CREATESYSTEM(AnimationSystem);
	ROSE_CREATESYSTEM(EntityEventSystem);
	ROSE_CREATESYSTEM(ScriptSystem);
	ROSE_CREATESYSTEM(TransformSystem);
	ROSE_CREATESYSTEM(PhysicsSystem, 0, -10);

	ROSE_CREATESYSTEM(CombatSystem);
}

void BaseGame::Setup()
{
	LoadProject("Sekiro.pro");
	ROSE_GETSYSTEM(RendererSystem).InitLoaded();
}

void BaseGame::LoadProject(const std::string& projectName)
{
	LevelLoader& levelLoader = ROSE_GETSYSTEM(LevelLoader);
	levelLoader.UnloadLevel();
	auto project = ROSE_GETSYSTEM(ProjectLoader).LoadProject(projectName);
	if(project != nullptr)
	{
		auto startLevelIndex = project->GetStartLevel();
		if(startLevelIndex != -1)
		{
			levelLoader.LoadLevel(project->GetLevelFile(startLevelIndex));
		}
	} else
	{
		ROSE_ERR("Couldn't open project");
	}
}