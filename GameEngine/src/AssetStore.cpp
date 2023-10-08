#include "AssetStore.h"
#include "Logger.h"
#include <sdl2/SDL_image.h>

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
	for (auto texture : textures) {
		SDL_DestroyTexture(texture.second);
	}
	textures.clear();
}

void AssetStore::AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filePath)
{
	SDL_Surface* surface = IMG_Load(filePath.c_str());
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	textures[assetId] = texture;

	Logger::Log("Asset Store Added Texture " + assetId);
}

SDL_Texture* AssetStore::GetTexture(const std::string& assetId) const
{
	return textures.at(assetId);
}
