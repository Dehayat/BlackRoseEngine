#pragma once
#include <sdl2/SDL.h>

#include "AssetHandle.h"

struct TextureAsset : Asset {
	SDL_Texture* texture;
	const int ppu;
	TextureAsset(SDL_Texture* texture, int ppi) :texture(texture), ppu(ppi) {}
	~TextureAsset() {
		if (texture != nullptr && SDL_WasInit(0) != 0) {
			SDL_DestroyTexture(texture);
		}
	}
};
