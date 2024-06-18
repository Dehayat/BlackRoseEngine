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
	static std::string GetAssetTypeName(AssetType type) {
		switch (type)
		{
		case AssetType::Empty:
			return "None";
		case AssetType::Texture:
			return "Texture";
			break;
		case AssetType::Animation:
			return "Animation";
			break;
		case AssetType::Script:
			return "Script";
			break;
		default:
			return "Unknown";
			break;
		}
	}
	static AssetType GetAssetFileType(const std::string& file);
};

class AssetHandle {
public:
	Asset* asset;
	AssetType type;
	AssetHandle(AssetType type = AssetType::Empty, Asset* asset = nullptr)
		:type(type),
		asset(asset)
	{
	}
};