#pragma once
#include <string>

enum class AssetType {
	Empty,
	Texture,
	Animation,
	Script
};

class Asset {
public:
	static std::string GetAssetTypeName(AssetType type);
	static AssetType GetAssetFileType(const std::string& file);
};

class AssetHandle {
public:
	Asset* asset;
	AssetType type;
	AssetHandle(AssetType type = AssetType::Empty, Asset* asset = nullptr);
};