#include "AssetStore.h"

#include <sdl2/SDL_image.h>

#include "AnimationImporter.h"
#include "../Core/SdlContainer.h"

#include "../Core/Systems.h"
#include "../Core/Log.h"

#include "AssetPackage.h"

#include "ScriptAsset.h"


AssetStore::AssetStore()
{
}

AssetStore::~AssetStore()
{
	UnloadAllAssets();
}

void AssetStore::UnloadAllAssets()
{
	for (auto& asset : assets) {
		delete asset.second.asset;
		asset.second.asset = nullptr;
	}
	assets.clear();
}

void AssetStore::AddTexture(const std::string& assetId, const std::string& filePath, int ppu)
{
	SDL_Renderer* renderer = ROSE_GETSYSTEM(SdlContainer).GetRenderer();
	SDL_Surface* surface = IMG_Load(filePath.c_str());
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	auto textureAsset = new TextureAsset(texture, ppu);
	if (assets.find(assetId) != assets.end()) {
		delete assets[assetId].asset;
		assets[assetId].type = AssetType::Texture;
		assets[assetId].asset = textureAsset;
	}
	else {
		assets[assetId] = AssetHandle(AssetType::Texture, textureAsset);
	}
	ROSE_LOG("Loaded Texture Asset %s", assetId.c_str());
}

void AssetStore::LoadAnimation(const std::string& assetId, const std::string& filePath)
{
	auto animation = AnimationImporter::LoadAnimation(filePath);
	if (assets.find(assetId) != assets.end()) {
		delete assets[assetId].asset;
		assets[assetId].type = AssetType::Animation;
		assets[assetId].asset = animation;
	}
	else {
		assets[assetId] = AssetHandle(AssetType::Animation, animation);
	}
	ROSE_LOG("Loaded Animation Asset %s", assetId.c_str());
}

void AssetStore::LoadScript(const std::string& assetId, const std::string& filePath)
{
	FileResource fileHandle = FileResource(filePath);
	std::string fileString = std::string("\0", SDL_RWsize(fileHandle.file));
	SDL_RWread(fileHandle.file, &fileString[0], sizeof(fileString[0]), fileString.size());

	auto script = new ScriptAsset(fileString);
	if (assets.find(assetId) != assets.end()) {
		delete assets[assetId].asset;
		assets[assetId].type = AssetType::Script;
		assets[assetId].asset = script;
	}
	else {
		assets[assetId] = AssetHandle(AssetType::Script, script);
	}
	ROSE_LOG("Loaded Script Asset %s", assetId.c_str());
}

AssetHandle AssetStore::GetAsset(const std::string& assetId) const
{
	if (assets.find(assetId) == assets.end()) {
		return AssetHandle();
	}
	return assets.at(assetId);
}

void AssetStore::LoadPackage(const std::string& filePath)
{
	auto pkg = new AssetPackage();
	if (pkg->Load(filePath)) {
		for (auto assetFile : pkg->assets) {
			switch (assetFile->assetType)
			{
			case AssetType::Texture:
			{
				auto metaData = (TextureMetaData*)(assetFile->metaData);
				AddTexture(metaData->name, assetFile->filePath, metaData->ppu);
				break;
			}
			case AssetType::Script:
			{
				auto metaData = (AssetMetaData*)(assetFile->metaData);
				LoadScript(metaData->name, assetFile->filePath);
				break;
			}
			default:
				break;
			}
		}
		delete pkg;
	}
	else {
		ROSE_ERR("Failed to load asset package %s", filePath.c_str());
	}
}


AssetHandle AssetStore::NewAnimation(const std::string& assetId)
{
	auto animation = new Animation(32, 32, "", false);
	if (assets.find(assetId) != assets.end()) {
		delete assets[assetId].asset;
		assets[assetId].type = AssetType::Animation;
		assets[assetId].asset = animation;
	}
	else {
		assets[assetId] = AssetHandle(AssetType::Animation, animation);
	}
	ROSE_LOG("Created new Animation Asset %s", assetId.c_str());
	return assets[assetId];
}

void AssetStore::SaveAnimation(const std::string& assetId, const std::string& filePath)
{
	auto animation = (Animation*)GetAsset(assetId).asset;
	if (animation != nullptr) {
		bool saved = AnimationImporter::SaveAnimation(animation, filePath);
	}
}