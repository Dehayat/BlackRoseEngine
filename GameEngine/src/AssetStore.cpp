#include "AssetStore.h"
#include <sdl2/SDL_image.h>
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
		for (auto texture : textures) {
			SDL_DestroyTexture(texture.second.texture);
		}
		textures.clear();
	}
}

void AssetStore::AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filePath, int ppu)
{
	SDL_Surface* surface = IMG_Load(filePath.c_str());
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	textures.emplace(assetId, TextureAsset(texture, ppu));

	Logger::Log("Asset Store Added Texture " + assetId);
}

const TextureAsset* AssetStore::GetTexture(const std::string& assetId) const
{
	if (textures.find(assetId) == textures.end()) {
		return nullptr;
	}
	return &(textures.at(assetId));
}
