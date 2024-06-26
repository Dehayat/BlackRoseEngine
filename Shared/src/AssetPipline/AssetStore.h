#pragma once
#include <map>
#include <vector>
#include <string>

#include "AnimationAsset.h"
#include "TextureAsset.h"

#include "Asset.h"

class AssetStore
{
private:
	std::map<std::string, AssetHandle> assets;

public:
	AssetStore();
	~AssetStore();

	void UnloadAllAssets();
	void AddTexture(const std::string& assetId, const std::string& filePath, int ppu = 100);
	void LoadAnimation(const std::string& assetId, const std::string& filePath);
	void LoadScript(const std::string& assetId, const std::string& filePath);
	AssetHandle GetAsset(const std::string& assetId) const;
	std::vector<std::pair<std::string, AssetHandle>> GetAssetOfType(AssetType assetType) const;
	void LoadPackage(const std::string& filePath);
	AssetHandle NewAnimation(const std::string& assetId);
	void SaveAnimation(const std::string& assetId, const std::string& filePath);
};