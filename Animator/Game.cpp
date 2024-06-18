#include "Game.h"

#include "Debugging/Logger.h"
#include "SdlContainer.h"
#include "Entity.h"
#include "AssetStore/AssetStore.h"
#include "Systems.h"
#include "Editor.h"

Game::Game() {
	Logger::Log("Game Constructed");
	SetupBaseSystems();
	CREATESYSTEM(Editor);
}

Game::~Game() {
	Logger::Log("Game Destructed");
}

void Game::SetupBaseSystems() {
	CREATESYSTEM(SdlContainer, 1200, (float)1200 * 9 / 16);
	CREATESYSTEM(Entities);
	CREATESYSTEM(AssetStore);
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
	bool exitGame = GETSYSTEM(Editor).ProcessEvents();
	if (exitGame) {
		isRunning = false;
	}
	bool isGameRunning = GETSYSTEM(Editor).IsGameRunning();
}

void Game::Render()
{
	auto& editor = GETSYSTEM(Editor);
	editor.RenderEditor();
}