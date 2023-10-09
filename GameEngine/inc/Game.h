#pragma once
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <box2d/box2d.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "EventSystem/EventBus.h"
#include "AssetStore.h"
#include "ImguiSystem.h"
#include "Logger.h"

const int FPS = 60;
const int FRAMETIME_MS = 1000 / FPS;

struct KeyData {
	bool justPressed;
	bool justReleased;
	bool isPressed;
	KeyData() {
		justPressed = false;
		justReleased = false;
		isPressed = false;
	}
};

struct InputData {
	KeyData keys[4];
};



class DebugDraw : public b2Draw
{
	SDL_Renderer* renderer;
	glm::mat3 matrix;
public:
	DebugDraw(SDL_Renderer* renderer) :matrix(1) {
		this->renderer = renderer;
	}
	void SetMatrix(glm::mat3 worldToScreen) {
		matrix = worldToScreen;
	}
	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
		Logger::Log("Drawing empty");
		SDL_SetRenderDrawColor(renderer, color.r * 255, color.g * 255, color.b * 255, color.a * 255);
		std::unique_ptr<Sint16[]> vx(new Sint16[vertexCount]);
		std::unique_ptr<Sint16[]> vy(new Sint16[vertexCount]);

		for (int i = 0;i < vertexCount;i++) {
			glm::vec3 pos = glm::vec3(vertices[i].x, vertices[i].y, 1);
			pos = pos * matrix;
			b2Vec2 nextVertex;
			if (i == vertexCount - 1) {
				nextVertex = vertices[0];
			}
			else {
				nextVertex = vertices[i + 1];
			}
			glm::vec3 nextPos = glm::vec3(vertices[i].x, vertices[i].y, 1);
			nextPos = nextPos * matrix;
			vx[i] = pos[0];
			vy[i] = pos[1];
		}
		polygonRGBA(renderer, vx.get(), vy.get(), vertexCount, color.r * 255, color.g * 255, color.b * 255, color.a/2 * 255);
	}
	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
		Logger::Log("Drawing Fill");
		SDL_SetRenderDrawColor(renderer, color.r * 255, color.g * 255, color.b * 255, color.a/2 * 255);
		std::unique_ptr<Sint16[]> vx(new Sint16[vertexCount]);
		std::unique_ptr<Sint16[]> vy(new Sint16[vertexCount]);

		for (int i = 0;i < vertexCount;i++) {
			glm::vec3 pos = glm::vec3(vertices[i].x, vertices[i].y, 1);
			pos = pos * matrix;
			b2Vec2 nextVertex;
			if (i == vertexCount - 1) {
				nextVertex = vertices[0];
			}
			else {
				nextVertex = vertices[i + 1];
			}
			glm::vec3 nextPos = glm::vec3(vertices[i].x, vertices[i].y, 1);
			nextPos = nextPos * matrix;
			vx[i] = pos[0];
			vy[i] = pos[1];
		}
		filledPolygonRGBA(renderer, vx.get(), vy.get(), vertexCount, color.r * 255, color.g * 255, color.b * 255, color.a/2 * 255);
	}
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
		Logger::Log("Drawing Seg");
		glm::vec3 pos = glm::vec3(p1.x, p1.y, 1);
		pos = pos * matrix;
		glm::vec3 nextPos = glm::vec3(p2.x, p2.y, 1);
		nextPos = nextPos * matrix;
		thickLineRGBA(renderer, pos[0], pos[1], nextPos[0], nextPos[1], 10, color.r * 255, color.g * 255, color.b * 255, color.a/2 * 255);
	}

	void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) {
		Logger::Log("Drawing Circle");
		glm::vec3 pos = glm::vec3(center.x, center.y, 1);
		glm::mat3 posMat = glm::mat3(radius, 0, pos.x, 0, radius, pos.y, 0, 0, 1);
		posMat = posMat * matrix;
		circleRGBA(renderer, posMat[0][2], posMat[1][2], posMat[0][0], color.r * 255, color.g * 255, color.b * 255, color.a/2 * 255);
	}

	void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color)
	{
		Logger::Log("Drawing Circle Fill");
		glm::vec3 pos = glm::vec3(center.x, center.y, 1);
		glm::mat3 posMat = glm::mat3(radius, 0, pos.x, 0, radius, pos.y, 0, 0, 1);
		posMat = posMat * matrix;
		filledCircleRGBA(renderer, posMat[0][2], posMat[1][2], posMat[0][0], color.r * 255, color.g * 255, color.b * 255, color.a/2 * 255);
	}
	void DrawTransform(const b2Transform& xf) {
		Logger::Log("Drawing transform");
		glm::vec3 pos = glm::vec3(xf.p.x, xf.p.y, 1);
		pos = pos * matrix;
		filledCircleRGBA(renderer, pos[0], pos[1], 5, 50, 0, 0, 50);
	}

	void DrawPoint(const b2Vec2& p, float size, const b2Color& color) {
		Logger::Log("Drawing Point");
		glm::vec3 pos = glm::vec3(p.x, p.y, 1);
		pos = pos * matrix;
		filledCircleRGBA(renderer, pos[0], pos[1], 5, color.r * 255, color.g * 255, color.b * 255, color.a/2 * 255);
	}
};


class Game {
private:
	SDL_Window* window;
	DebugDraw* debugDrawer;
	SDL_Renderer* renderer;
	bool isRunning;
	unsigned int msLastFrame;
	float dt;
	std::unique_ptr<EventBus> eventBus;
	int windowWidth;
	int windowHeight;
	entt::registry registry;
	std::unique_ptr<AssetStore> assetStore;
	entt::entity player;
	entt::entity camera;
	InputData input;
	ImguiSystem imgui;
	std::unique_ptr<b2World> physicsWorld;


public:

	Game();
	~Game();
	void Setup();
	void Run();
	void ProcessEvents();
	void Update();
	void Render();
	void UpdateInputSystem();
};
