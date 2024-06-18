#pragma once

enum class AssetType {
	Empty,
	Texture,
	Animation,
	Script
};

class Asset {

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