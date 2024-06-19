#include "TextureAsset.h"

TextureAsset::~TextureAsset() {
	if (texture != nullptr && SDL_WasInit(0) != 0) {
		SDL_DestroyTexture(texture);
	}
}

TextureAsset::TextureAsset(SDL_Texture* texture, int ppi) :texture(texture), ppu(ppi) {}
