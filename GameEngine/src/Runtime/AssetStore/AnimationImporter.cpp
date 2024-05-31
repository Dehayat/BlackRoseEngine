#include "Animation/AnimationImporter.h"

#include <SDL2/SDL.h>

#include "FileResource.h"

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
