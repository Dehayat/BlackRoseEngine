#include "Renderer.h"
#include "Transform.h"
#include "Logger.h"

Sprite::Sprite(std::string sprite, int layer, SDL_Color color) {
	this->sprite = sprite;
	this->layer = layer;
	this->color = color;
}
Camera::Camera(float height) {
	this->height = height;
	camToScreen = glm::mat3();
	worldToScreen = glm::mat3();
}


Renderer::Renderer(SDL_Renderer* sdl)
{
	this->sdl = sdl;
	this->worldToScreenMatrix = glm::mat3(1);
}
Renderer::~Renderer()
{
}
void Renderer::Render(entt::registry* registry, const AssetStore& assetStore)
{
	SDL_SetRenderDrawColor(sdl, 38, 77, 142, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(sdl);
	registry->sort<Sprite>([](const auto& lhs, const auto& rhs) {
		return lhs.layer < rhs.layer;
		});

	auto camPos = registry->get<Transform>(camera);
	float camHeight = 10;
	if (registry->valid(camera)) {
		auto& cam = registry->get<Camera>(camera);
		auto camHeight = cam.height;
	}
	else {
		Logger::Error("No Camera Assigned to renderer");
	}
	auto camMatrix = glm::mat3(
		glm::cos(glm::radians(camPos.rotation)), -glm::sin(glm::radians(camPos.rotation)), camPos.position.x,
		glm::sin(glm::radians(camPos.rotation)), glm::cos(glm::radians(camPos.rotation)), camPos.position.y,
		0, 0, 1
	);
	//assume ok
	auto invCamMatrix = glm::inverse(camMatrix);
	int sizeX, sizeY;
	SDL_GetRendererOutputSize(sdl, &sizeX, &sizeY);
	auto windowSize = glm::vec2(sizeX, sizeY);
	float windowAspect = windowSize.x / windowSize.y;
	float camWidth = windowAspect * camHeight;
	float scaleX = windowSize.x / (camWidth);
	float scaleY = windowSize.y / (camHeight);

	auto tr = glm::mat3(
		1, 0, camWidth / 2,
		0, -1, camHeight / 2,
		0, 0, 1);
	auto sc = glm::mat3(
		scaleX, 0, 0,
		0, scaleY, 0,
		0, 0, 1);
	//assume ok
	glm::mat3 camToScreen = tr * sc;
	auto worldToScreen = invCamMatrix * camToScreen;
	//cam.camToScreen = camToScreen;
	//cam.worldToScreen = worldToScreen;
	this->worldToScreenMatrix = worldToScreen;



	auto view2 = registry->view<const Sprite, const Transform>();
	for (auto entity : view2) {
		const auto& pos = view2.get<Transform>(entity);
		const auto& sp = view2.get<Sprite>(entity);
		const auto& texture = assetStore.GetTexture(sp.sprite);
		int texW;
		int texH;
		SDL_QueryTexture(texture.texture, nullptr, nullptr, &texW, &texH);
		auto viewMatrix = pos.matrix * worldToScreen;
		auto position = glm::vec2(viewMatrix[0][2], viewMatrix[1][2]);
		auto scale = glm::vec2(glm::sqrt(viewMatrix[0][0] * viewMatrix[0][0] + viewMatrix[1][0] * viewMatrix[1][0]), glm::sqrt(viewMatrix[0][1] * viewMatrix[0][1] + viewMatrix[1][1] * viewMatrix[1][1]));
		auto rotation = glm::degrees(std::atan2f(viewMatrix[1][0], viewMatrix[0][0]));

		int spriteSizeX = scale.x * ((float)texW / texture.ppu);
		int spriteSizeY = scale.y * ((float)texH / texture.ppu);
		SDL_FRect player = SDL_FRect{
			(position.x - spriteSizeX / 2),
			(position.y - spriteSizeY / 2),
			(float)spriteSizeX,
			(float)spriteSizeY,
		};
		SDL_SetTextureColorMod(texture.texture, sp.color.r, sp.color.g, sp.color.b);
		SDL_SetTextureBlendMode(texture.texture, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(texture.texture, sp.color.a);
		SDL_RenderCopyExF(sdl, texture.texture, nullptr, &player, rotation, nullptr, SDL_FLIP_NONE);
	}
}
void Renderer::Present()
{
	SDL_RenderPresent(sdl);
}

void Renderer::SetCamera(entt::entity cam)
{
	this->camera = cam;
}

entt::entity Renderer::GetCamera()
{
	return camera;
}

glm::mat3 Renderer::GetWorldToScreenMatrix()
{
	return worldToScreenMatrix;
}
glm::mat3 Renderer::GetScreenToWorldMatrix()
{
	return glm::inverse(worldToScreenMatrix);
}
