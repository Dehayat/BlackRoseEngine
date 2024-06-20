#include "AnimationImporter.h"

#include <SDL2/SDL.h>

#include "../Core/FileResource.h"
#include "../Core/Assert.h"
#include "../Core/Systems.h"
#include "../Core/Log.h"
#include "../Reflection/Reflection.h"

std::string AnimationImporter::ReadToEndOfLine(const std::string& fileString, int startPos) {
	std::string line = "";
	int pos = startPos;
	while (pos < fileString.size() && (fileString[pos] != '\n' && fileString[pos] != '\r')) {
		line += fileString[pos];
		pos++;
	}
	return line;
}

std::string AnimationImporter::ReadToSpace(const std::string& fileString, int startPos) {
	std::string token = "";
	int pos = startPos;
	while (pos < fileString.size() && fileString[pos] != ' ') {
		token += fileString[pos];
		pos++;
	}
	return token;
}

Animation* AnimationImporter::LoadAnimation(const std::string& fileName) {
	FileResource fileHandle = FileResource(fileName);
	ROSE_ASSERT(fileHandle.file != nullptr);
	std::string fileString = std::string("\0", SDL_RWsize(fileHandle.file));
	SDL_RWread(fileHandle.file, &fileString[0], sizeof(fileString[0]), fileString.size());

	auto texturePos = fileString.find("Texture", 0);
	std::string texture = ReadToEndOfLine(fileString, texturePos + 8);

	auto SizePos = fileString.find("Size", 0);
	std::string widthString = ReadToSpace(fileString, SizePos + 5);
	std::string heightString = ReadToEndOfLine(fileString, SizePos + 5 + widthString.size() + 1);
	int width = std::stoi(widthString);
	int height = std::stoi(heightString);

	bool isLooping = false;
	if (fileString.find("Loop") != std::string::npos) {
		isLooping = true;
	}

	auto animation = new Animation(width, height, texture, isLooping);

	auto framePos = fileString.find("Frame");
	while (framePos != std::string::npos) {
		std::string framePositionString = ReadToSpace(fileString, framePos + 6);
		std::string frameDurationString = ReadToEndOfLine(fileString, framePos + 6 + framePositionString.size() + 1);
		auto framePosition = std::stoi(framePositionString);
		float frameDuration = std::stof(frameDurationString);
		animation->AddFrame(new Frame{ framePosition,frameDuration });
		framePos = fileString.find("Frame", framePos + 1);
	}

	auto eventPos = fileString.find("Event");
	while (eventPos != std::string::npos) {
		std::string eventName = ReadToSpace(fileString, eventPos + 6);
		std::string eventTimeString = ReadToEndOfLine(fileString, eventPos + 6 + eventName.size() + 1);
		float eventTime = std::stof(eventTimeString);
		animation->AddEvent(new AnimationEventData{ eventTime,eventName });
		eventPos = fileString.find("Event", eventPos + 1);
	}
	return animation;
}


bool AnimationImporter::SaveAnimation(Animation* animation, const std::string& fileName)
{
	auto animationReader = ROSE_GETSYSTEM(ReflectionSystem).GetInfo<Animation>();
	FileResource fileHandle = FileResource(fileName, "w+");
	if (fileHandle.file == nullptr) {
		return false;
	}
	auto texture = (std::string*)animationReader->GetVar(animation, "texture");
	auto spriteFrameWidth = (int*)animationReader->GetVar(animation, "spriteFrameWidth");
	auto spriteFrameHeight = (int*)animationReader->GetVar(animation, "spriteFrameHeight");
	auto isLooping = (bool*)animationReader->GetVar(animation, "isLooping");
	std::string textureString = "Texture " + *texture + "\n";
	SDL_RWwrite(fileHandle.file, &(textureString[0]), sizeof(textureString[0]), textureString.size());
	std::string size = "Size " + std::to_string(*spriteFrameWidth) + " " + std::to_string(*spriteFrameHeight) + "\n";
	SDL_RWwrite(fileHandle.file, &size[0], sizeof(size[0]), size.size());
	if (*isLooping) {
		std::string loop = "Loop\n";
		SDL_RWwrite(fileHandle.file, &loop[0], sizeof(loop[0]), loop.size());
	}
	for (auto frameData : animation->frames) {
		std::string frame = "Frame " + std::to_string(frameData->framePosition) + " " + std::to_string(frameData->frameDuration) + "\n";
		SDL_RWwrite(fileHandle.file, &frame[0], sizeof(frame[0]), frame.size());
	}
	for (auto eventData : animation->animationEvents) {
		if (eventData->eventName == "") {
			ROSE_LOG("Ignoring empty event");
			continue;
		}
		if (eventData->eventName.find("Event") != -1) {
			ROSE_LOG("Event name cant the word Event");
			continue;
		}
		std::string eventString = "Event " + eventData->eventName + " " + std::to_string(eventData->eventTime) + "\n";
		SDL_RWwrite(fileHandle.file, &eventString[0], sizeof(eventString[0]), eventString.size());
	}
	return true;
}