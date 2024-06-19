#pragma once
#include <sdl2/SDL.h>

#include "Asset.h"

struct TextureAsset : Asset {
	SDL_Texture* texture;
	const int ppu;
	TextureAsset(SDL_Texture* texture, int ppi);
	~TextureAsset();
};
