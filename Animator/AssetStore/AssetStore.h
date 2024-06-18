#pragma once
#include <map>
#include <string>

#include "TextureAsset.h"

#include "AssetHandle.h"

class AssetStore {
private:
	std::map<std::string, AssetHandle> assets;

public:
	AssetStore();
	~AssetStore();

	void ClearAssets();
	void AddTexture(const std::string& assetId, const std::string& filePath, int ppu = 100);
	void LoadAnimation(const std::string& assetId, const std::string& filePath);
	void SaveAnimation(const std::string& assetId, const std::string& filePath);
	AssetHandle GetAsset(const std::string& assetId) const;
	void LoadPackage(const std::string& filePath);
	AssetHandle NewAnimation(const std::string& assetId);
};