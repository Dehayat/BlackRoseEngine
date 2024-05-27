#include "AssetStore.h"

#include <sdl2/SDL_image.h>
#include <entt/entt.hpp>

#include "SdlContainer.h"
#include "Animation/AnimationImporter.h"

#include "Logger.h"


AssetStore::AssetStore()
{
	Logger::Log("Asset Store created");
}

AssetStore::~AssetStore()
{
	ClearAssets();
	Logger::Log("Asset Store destroyed");
}

void AssetStore::ClearAssets()
{
	if (SDL_WasInit(0) != 0) {
		for (auto& texture : textures) {
			SDL_DestroyTexture(texture.second->texture);
			delete texture.second;
		}
		textures.clear();
	}
	for (auto& animation : animations) {
		delete animation.second;
	}
	animations.clear();
}

void AssetStore::AddTexture(const std::string& assetId, const std::string& filePath, int ppu)
{
	if (textures.find(assetId) != textures.end()) {
		return;
	}
	SDL_Renderer* renderer = entt::locator<SdlContainer>::value().GetRenderer();
	SDL_Surface* surface = IMG_Load(filePath.c_str());
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	textures.emplace(assetId, new TextureAsset(texture, ppu));

	Logger::Log("Asset Store Added Texture " + assetId);
}

const TextureAsset* AssetStore::GetTexture(const std::string& assetId) const
{
	if (textures.find(assetId) == textures.end()) {
		return nullptr;
	}
	return textures.at(assetId);
}

void AssetStore::LoadAnimation(const std::string& assetId, const std::string& filePath)
{
	if (animations.find(assetId) != animations.end()) {
		return;
	}
	animations.emplace(assetId, AnimationImporter::LoadAnimation(filePath));
}

Animation* AssetStore::GetAnimation(const std::string& assetId) const
{
	if (animations.find(assetId) == animations.end()) {
		return nullptr;
	}
	return animations.at(assetId);
}
