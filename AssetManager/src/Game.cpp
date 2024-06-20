#include "Game.h"

#include "Core/Log.h"
#include "Core/SdlContainer.h"
#include "Core/Guid.h"
#include "Core/Systems.h"
#include "Editor.h"
#include "AssetPipline/AssetStore.h"

#include "FileDialog.h"

Game::Game() {
	ROSE_LOG("Game Constructed");
	SetupBaseSystems();
	ROSE_CREATESYSTEM(Editor);
	ROSE_CREATESYSTEM(AssetStore);
}

Game::~Game() {
	ROSE_DESTROYSYSTEM(FileDialog);
	ROSE_DESTROYSYSTEM(Editor);
	ROSE_DESTROYSYSTEM(SdlContainer);
	ROSE_DESTROYSYSTEM(AssetStore);
	ROSE_LOG("Game Destructed");
}

void Game::SetupBaseSystems() {
	ROSE_CREATESYSTEM(FileDialog);
	ROSE_CREATESYSTEM(SdlContainer, 1200, (float)1200 * 9 / 16);
	isRunning = false;
}

void Game::Run()
{
	isRunning = true;
	while (isRunning) {
		Update();
		Render();
	}
}


void Game::Update()
{
	bool exitGame = ROSE_GETSYSTEM(Editor).ProcessEvents();
	if (exitGame) {
		isRunning = false;
	}
	bool isGameRunning = ROSE_GETSYSTEM(Editor).IsGameRunning();
}

void Game::Render()
{
	auto& editor = ROSE_GETSYSTEM(Editor);
	editor.RenderEditor();
}