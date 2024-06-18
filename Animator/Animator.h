#pragma once
#include <vector>
#include <string>

#include <imgui.h>

#include "AssetStore/AssetPackage.h"

#include "SdlContainer.h"

#include "AnimationComponent.h"

#include "Debugging/Logger.h"

static std::string Label(const std::string& label, Guid guid) {
	return label + "##" + std::to_string(guid);
}

class Animator {

	std::vector<AssetPackage*> assetPackages;
	AssetFile* selectedAsset;
	AssetFile* selectedAnimation;
	AssetHandle animationHandle;
	AnimationComponent player;
	float animationDuration;
	float currentAnimationTime;
	bool isPlaying;
	int selectedFrame;
	bool scrollToSelected;
	int selectedEvent;
	bool scrollToEvent;
	Uint64 lastTime;
	const float maxDt;
	bool loadSelectedAnimaiton = false;
	std::string animationFile;

public:
	Animator();
	~Animator() {}

	AssetPackage* GetPackage(const std::string& filePath);

	void Render();
	void PackageLoaderEditor();
	void AnimationAssetEditor(ImVec2 size);
	void AnimationPlayerEditor(ImVec2 size);
	void AnimationViewportEditor(ImVec2 size);
	bool IsAssetSelected();
	void RenderSelectedAsset();
	int RenderFrame(Frame* frame, int id);
	int RenderEvent(AnimationEventData* eventData, int id);
	void RenderFrameImage(Frame* frame, int id, float fullWidth);
	int GetFrameCount();
	void GenerateFramesFromAtlas(float frameDuration);
	std::string SaveFile(const std::string& extension);
	std::string OpenFile(const std::string& extension);
};