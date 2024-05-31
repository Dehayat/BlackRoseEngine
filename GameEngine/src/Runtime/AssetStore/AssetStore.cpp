#include "AssetStore/AssetStore.h"

#include <sdl2/SDL_image.h>

#include "Animation/AnimationImporter.h"
#include "SdlContainer.h"

#include "Systems.h"

#include "Debug/Logger.h"


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

	for (auto& asset : assets) {
		delete asset.second.asset;
	}
	assets.clear();
}

void AssetStore::AddTexture(const std::string& assetId, const std::string& filePath, int ppu)
{
	SDL_Renderer* renderer = GETSYSTEM(SdlContainer).GetRenderer();
	SDL_Surface* surface = IMG_Load(filePath.c_str());
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	auto textureAsset = new TextureAsset(texture, ppu);
	if (assets.find(assetId) != assets.end()) {
		delete assets[assetId].asset;
	}
	assets[assetId] = AssetHandle(AssetType::Texture, textureAsset);
	Logger::Log("Loaded Texture Asset " + assetId);
}

void AssetStore::LoadAnimation(const std::string& assetId, const std::string& filePath)
{
	auto animation = AnimationImporter::LoadAnimation(filePath);
	if (assets.find(assetId) != assets.end()) {
		delete assets[assetId].asset;
	}
	assets[assetId] = AssetHandle(AssetType::Animation, animation);
	Logger::Log("Loaded Animation Asset " + assetId);
}

AssetHandle AssetStore::GetAsset(const std::string& assetId) const
{
	if (assets.find(assetId) == assets.end()) {
		return AssetHandle();
	}
	return assets.at(assetId);
}