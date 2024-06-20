#include "Asset.h"

#include "../FileDialog.h"

#include "../Core/Systems.h"

std::string Asset::GetAssetTypeName(AssetType type)
{
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
AssetType Asset::GetAssetFileType(const std::string& file) {
	auto extension = ROSE_GETSYSTEM(FileDialog).GetFileExtension(file);
	for (auto& c : extension) {
		c = std::tolower(c);
	}
	if (extension == ".lua") {
		return AssetType::Script;
	}
	if (extension == ".anim") {
		return AssetType::Animation;
	}
	if (extension == ".png" || extension == ".jpg") {
		return AssetType::Texture;
	}
	return AssetType::Empty;
}


AssetHandle::AssetHandle(AssetType type, Asset* asset)
	:type(type),
	asset(asset)
{
}
