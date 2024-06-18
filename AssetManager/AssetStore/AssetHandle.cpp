#include "AssetHandle.h"

#include <FileDialog.h>

#include "../Systems.h"

AssetType Asset::GetAssetFileType(const std::string& file) {
	auto extension = GETSYSTEM(FileDialog).GetFileExtension(file);
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
