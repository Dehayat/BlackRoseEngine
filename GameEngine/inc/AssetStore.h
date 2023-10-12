#pragma once
#include <sdl2/SDL.h>
#include <map>
#include <string>

struct TextureAsset {
	SDL_Texture* texture;
	const int ppu;
	TextureAsset(SDL_Texture* texture, int ppi) :texture(texture), ppu(ppi) {}
};

class AssetStore {
private:
	std::map<std::string, TextureAsset> textures;

public:
	AssetStore();
	~AssetStore();

	void ClearAssets();
	void AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filePath, int ppu = 100);
	const TextureAsset* GetTexture(const std::string& assetId) const;

};