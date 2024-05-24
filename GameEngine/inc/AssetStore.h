#pragma once
#include <map>
#include <string>

#include <sdl2/SDL.h>

#include "Animation/AnimationAsset.h"

struct TextureAsset {
	SDL_Texture* texture;
	const int ppu;
	TextureAsset(SDL_Texture* texture, int ppi) :texture(texture), ppu(ppi) {}
};

class AssetStore {
private:
	std::map<std::string, TextureAsset> textures;
	std::map<std::string, Animation*> animations;

public:
	AssetStore();
	~AssetStore();

	void ClearAssets();
	void AddTexture(const std::string& assetId, const std::string& filePath, int ppu = 100);
	const TextureAsset* GetTexture(const std::string& assetId) const;
	void LoadAnimation(const std::string& assetId, const std::string& filePath);
	Animation* GetAnimation(const std::string& assetId) const;

};