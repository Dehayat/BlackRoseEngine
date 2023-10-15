#include <sstream>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <box2d/b2_world_callbacks.h>
#include <ryml/ryml.hpp>
#include "Game.h"
#include "Components/GUIDComponent.h"
#include "Components/PlayerComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/CameraComponent.h"
#include "Logger.h"

#ifdef _EDITOR
#include <imgui.h>
#include "Editor/TransformEditor.h"
#include "Editor/PhysicsEditor.h"
#include "Editor/RenderEditor.h"
#endif // _EDITOR



Game::Game() :allEntities() {
	Logger::Log("Game Constructed");

	sdl = std::make_unique<SdlContainer>(1200, (float)1200 * 9 / 16);
	assetStore = std::make_unique<AssetStore>();
	physics = std::make_unique<Physics>(0, -10);
	physics->InitDebugDrawer(sdl->GetRenderer());
	renderer = std::make_unique<Renderer>(sdl->GetRenderer());
	transformSystem.InitDebugDrawer(sdl->GetRenderer());
	registry.on_construct<Transform>().connect<&TransformSystem::TransformCreated>(transformSystem);
	registry.on_construct<PhysicsBody>().connect<&Physics::PhysicsBodyCreated>(physics.get());
	registry.on_destroy<PhysicsBody>().connect<&Physics::PhysicsBodyDestroyed>(physics.get());
#ifdef _DEBUG
	physics->EnableDebug(true);
	transformSystem.EnableDebug(true);
#endif // !_DEBUG

	auto x = SDL_GL_GetCurrentContext();

	dt = 0;
	msLastFrame = 0;
	isRunning = false;
}
Game::~Game() {
	Logger::Log("Game Destructed");
}


void Game::Setup()
{
	assetStore->AddTexture(sdl->GetRenderer(), "rose", "./assets/Rose.png", 512);
	assetStore->AddTexture(sdl->GetRenderer(), "hornet", "./assets/Hornet_Idle.png", 128);
	assetStore->AddTexture(sdl->GetRenderer(), "block", "./assets/Block.jpg", 64);
	assetStore->AddTexture(sdl->GetRenderer(), "big_ground", "./assets/BigGround.png", 128);


	//levelLoader.LoadLevel("Level.yaml", registry);
	levelLoader.LoadLevel("SavedLevel.yaml", registry);

	RegisterAllEntities();
	transformSystem.InitLoaded(registry, allEntities);
	renderer->InitLoaded(registry);
#ifdef _EDITOR
	levelTree.Init(registry);
#endif // _EDITOR

}
void Game::Run()
{
	Setup();
	isRunning = true;
	while (isRunning) {
		Update();
		Render();

		std::uint64_t waitTimeMs = SDL_GetTicks64() + FRAMETIME_MS - (SDL_GetTicks64() - msLastFrame);
		//while (SDL_GetTicks64() < waitTimeMs);
		dt = (SDL_GetTicks64() - msLastFrame) / 1000.0f;
		msLastFrame = SDL_GetTicks64();
	}
}
void Game::Update()
{
	transformSystem.Update(registry);

#ifdef _EDITOR
	if (imgui.ProcessEvents()) {
		isRunning = false;
	}
	input.Update(sdl->GetWindow());
#else
	if (sdl->ProcessEvents()) {
		isRunning = false;
	}
	input.Update(sdl->GetWindow());
	physics->Update(registry);

	//if (input.GetMouseButton(InputMouse::LEFT_BUTTON).justPressed) {
	//	const auto ground = registry.create();
	//	auto spawnPos = glm::vec3(input.GetMousePosition(), 1) * renderer->GetScreenToWorldMatrix();
	//	registry.emplace<Transform>(ground, glm::vec2(spawnPos.x, spawnPos.y), glm::vec2(1, 1), 0);
	//	registry.emplace<Sprite>(ground, "rose", 0, SDL_Color{ 255,255,255,255 });
	//	registry.emplace<PhysicsBody>(ground, glm::vec2(spawnPos.x, spawnPos.y), glm::vec2(0.25, 0.25));
	//}
	auto view2 = registry.view<Player, const Transform, PhysicsBody>();
	for (auto entity : view2) {
		const auto& pos = view2.get<Transform>(entity);
		auto& phys = view2.get<PhysicsBody>(entity);
		auto& player = view2.get<Player>(entity);
		if (input.GetKey(InputKey::LEFT).isPressed || input.GetKey(InputKey::A).isPressed) {
			player.input = -1;
		}
		else if (input.GetKey(InputKey::RIGHT).isPressed || input.GetKey(InputKey::D).isPressed) {
			player.input = 1;
		}
		else {
			player.input = 0;
		}
		auto vel = b2Vec2(player.speed * player.input, phys.body->GetLinearVelocity().y);
		phys.body->SetLinearVelocity(vel);
	}
#endif // _EDITOR
}

#ifdef _EDITOR


entt::entity selected = entt::entity(-1);
entt::entity created = entt::entity(-1);
bool entityList[100];
vec2 lastPos;
void Editor(entt::registry& registry, InputSystem& input, Renderer& renderer, LevelEditor::LevelTree& tree, LevelLoader& levelLoader, Physics& physics) {

	if (input.GetMouseButton(MIDDLE_BUTTON).justPressed) {
		lastPos = input.GetMousePosition();
	}
	else if (input.GetMouseButton(MIDDLE_BUTTON).isPressed) {
		auto curPos = input.GetMousePosition();
		if (glm::distance(curPos, lastPos) > 3) {
			glm::vec2 lastPosCam = glm::vec3(lastPos, 1) * renderer.GetScreenToWorldMatrix();
			glm::vec2 curPosCam = glm::vec3(curPos, 1) * renderer.GetScreenToWorldMatrix();
			renderer.GetEditorCamTrx().position += lastPosCam - curPosCam;
			lastPos = curPos;
		}
	}

	ImGui::SetNextWindowSize(ImVec2(200, 400));
	ImGui::Begin("Tools");
	const char* listbox_items[] = { "Create Entity","Move Entity" };
	static int listbox_item_current = 0;
	ImGui::ListBox("Tool", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 2);
	if (listbox_item_current == 0) {
		auto mousePos = glm::vec3(input.GetMousePosition(), 1) * renderer.GetScreenToWorldMatrix();
		if (input.GetMouseButton(LEFT_BUTTON).justPressed) {
			Logger::Log("create");
			created = registry.create();
			tree.AddEntity(created);
			selected = created;
			registry.emplace<GUID>(created);
			registry.emplace<Transform>(created, glm::vec2(mousePos.x, mousePos.y), glm::vec2(1, 1), 0);
		}
		if (created != entt::entity(-1)) {
			if (input.GetMouseButton(LEFT_BUTTON).justReleased) {
				created = entt::entity(-1);
			}
			if (input.GetMouseButton(LEFT_BUTTON).isPressed) {
				registry.get<Transform>(created).position = glm::vec2(mousePos.x, mousePos.y);
			}
		}
	}
	if (listbox_item_current == 1) {
		auto mousePos = glm::vec3(input.GetMousePosition(), 1) * renderer.GetScreenToWorldMatrix();
		if (selected != entt::entity(-1)) {
			if (input.GetMouseButton(LEFT_BUTTON).isPressed) {
				registry.get<Transform>(selected).position = glm::vec2(mousePos.x, mousePos.y);
			}
		}
	}
	auto view = registry.view<const GUID, Transform>();
	tree.Editor(registry, selected, entityList);
	if (ImGui::Button("Save Level")) {
		levelLoader.SaveLevel("SavedLevel.yaml", registry);
	}
	ImGui::End();


	if (selected != entt::entity(-1)) {
		if (registry.any_of<Transform>(selected)) {
			ImGui::Begin("Transform component");
			TransformEditor::DrawEditor(registry.get<Transform>(selected));
			ImGui::End();
		}
		if (registry.any_of<PhysicsBody>(selected)) {
			ImGui::Begin("PhysicsBody component");
			auto trx = registry.get<Transform>(selected);
			PhysicsEditor::DrawEditor(registry.get<PhysicsBody>(selected), trx);
			ImGui::End();
			if (ImGui::Button("Remove PhysicsBody Component")) {
				registry.remove<PhysicsBody>(selected);
			}
		}
		else {
			if (ImGui::Button("Add PhysicsBody Component")) {
				registry.emplace<PhysicsBody>(selected);
			}
		}
		if (registry.any_of<Sprite>(selected)) {
			ImGui::Begin("Sprite component");
			SpriteEditor::DrawEditor(registry.get<Sprite>(selected));
			ImGui::End();
			if (ImGui::Button("Remove Sprite Component")) {
				registry.remove<Sprite>(selected);
			}
		}
		else {
			if (ImGui::Button("Add Sprite Component")) {
				registry.emplace<Sprite>(selected, "block");
			}
		}
		if (registry.any_of<Camera>(selected)) {
			ImGui::Begin("Camera component");
			CameraEditor::DrawEditor(registry.get<Camera>(selected));
			ImGui::End();
			if (ImGui::Button("Remove Camera Component")) {
				registry.remove<Camera>(selected);
			}
		}
		else {
			if (ImGui::Button("Add Camera Component")) {
				registry.emplace<Camera>(selected);
			}
		}
	}
}

#endif // _EDITOR
void Game::Render()
{

	renderer->Render(&registry, *assetStore);
#ifdef _DEBUG
	physics->DebugRender(renderer->GetWorldToScreenMatrix());
	//transformSystem.DebugRender(renderer->GetWorldToScreenMatrix(), registry);
	transformSystem.GetDebugRenderer().SetMatrix(renderer->GetWorldToScreenMatrix());

#endif // _DEBUG
#ifdef _EDITOR
	auto camEntity = renderer->GetCamera();
	auto cam = registry.get<Camera>(camEntity);
	auto camTrx = registry.get<Transform>(camEntity);
	CameraEditor::DrawGizmos(sdl->GetRenderer(),*renderer, cam, camTrx);

	if (selected != entt::entity(-1)) {
		transformSystem.GetDebugRenderer().DrawTransform(registry.get<Transform>(selected));
	}
#endif
	renderer->Present();

#ifdef _EDITOR
	imgui.Render();
	//ImGui::ShowDemoWindow();
	Editor(registry, input, *renderer, levelTree, levelLoader, *physics);
	//Imgui Code
	imgui.Present();
#endif // _EDITOR

}

void Game::RegisterAllEntities()
{
	allEntities.clear();
	auto view = registry.view<const GUID>();
	for (auto entity : view) {
		const auto& guid = view.get<GUID>(entity);
		allEntities.emplace(guid.id, entity);
	}
}
