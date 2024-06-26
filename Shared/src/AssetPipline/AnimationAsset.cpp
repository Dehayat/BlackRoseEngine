#include "AnimationAsset.h"
#include "../Core/Log.h"


Animation::Animation(int spriteWidth, int spriteHeight, std::string spriteTexture, bool isLooping)
	:spriteFrameWidth(spriteWidth),
	spriteFrameHeight(spriteHeight),
	isLooping(isLooping),
	texture(spriteTexture) {
}

SDL_Rect Animation::GetSourceRect(int frame) {
	if (frame >= frames.size()) {
		ROSE_ERR("frame outside animation bounds");
		return SDL_Rect();
	}
	SDL_Rect rect{
		frames[frame]->framePosition * spriteFrameWidth,
		0,
		spriteFrameWidth,
		spriteFrameHeight
	};
	return rect;
}

void Animation::AddFrame(Frame* frame) {
	frames.push_back(frame);
}

void Animation::AddEvent(AnimationEventData* animationEvent) {
	animationEvents.push_back(animationEvent);
}
